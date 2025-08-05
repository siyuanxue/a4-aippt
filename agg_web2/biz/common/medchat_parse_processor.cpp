#include <agg_web2/biz/common/medchat_parse_processor.h>
#include <agg_web2/helper/medical_chat_helper.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <agg2/common/agg_common.h>
#include <agg2/util/url_util.h>
#include <agg2/helper/a4_context_helper.h>
#include <agg2/helper/test_user_context.h>
#include <A4/common/http_packet_factory.h>
#include <A4/util/string_util.h>
#include <A4/util/file_util.h>
#include <agg2/common/rapidjson.h>
#include <messer_utils/json/JsonParser.h>
#include <autil/legacy/fast_jsonizable.h>

AGG_BEGIN_NAMESPACE(biz);
AGG_LOG_SETUP(biz, MedChatParseProcessor);

const std::string PARAM_BTS_GROUP = "bts_group";
const std::string PARAM_MUST_QUERY = "must_query";
const std::string EXTRA_PARAM_FILE = "extra_param_file";

MedChatParseProcessor::MedChatParseProcessor()
    : mMustQuery(true), mBtsGroupSwitch(true)
{
}

MedChatParseProcessor::~MedChatParseProcessor() 
{
}

void MedChatParseProcessor::InitResponse(AggContext *aggContext) {
    AggUserContext * aggUserContext = new AggWebUserContext;
    aggContext->SetAggUserContext(aggUserContext);    
    // get param
    std::string q;
    std::string hit;
    std::string start;
    std::string uid;
    std::string sessionid;
    std::string reqid;
    std::string wskip;
    aggContext->GetRequestParam("query", q);
    aggContext->GetRequestParam("hit", hit);
    aggContext->GetRequestParam("start", start);
    aggContext->GetRequestParam("uid", uid);
    aggContext->GetRequestParam("sessionid", sessionid);
    aggContext->GetRequestParam("reqid", reqid);
    aggContext->GetRequestParam("wskip", wskip);

    ChatResponsePtr responsePtr(new ChatResponse()); 
    ChatResponse &response = *responsePtr;
    response.version = 1;
    response.target = "respond";
    response.communication.uid = uid;
    response.communication.sessionid = sessionid; 
    response.communication.reqid = reqid; 
    response.communication.resid = 1;
    response.data.query = q;
    response.data.isMed = false;

    response.data.status = "init";
    response.error_code = 0;
    response.error_msg = "";

    AggWebContextHelper::SetChatResponse(aggContext, responsePtr);
}

bool MedChatParseProcessor::DoInit(const String2String& params)
{
    std::string extraParamFile;
    PARSE_PROCESSOR_PARAM(params, EXTRA_PARAM_FILE, extraParamFile);
    std::string content;
    if (!FileUtil::ReadLocalFile(extraParamFile, content)) {
        AGG_LOG(ERROR, "ReadLocalFile %s failed", extraParamFile.c_str());
        //return false;
    }
    if (!content.empty()) {
        try {
            autil::legacy::FromJsonString(mExtraParams, content);
        } catch(autil::legacy::WrongFormatJsonException ex) {
            AGG_LOG(ERROR, "jsonize extra param failed from config[%s],"
                    " exception is [%s]", content.c_str(), ex.what());
            return false;
        }
    }
    AGG_LOG(INFO,"extra param size:%d",mExtraParams.size());
    std::string mustQueryStr;
    PARSE_PROCESSOR_PARAM_UNRET(params, PARAM_MUST_QUERY, mustQueryStr);
    AGG_LOG(DEBUG, "must_query = [%s]", mustQueryStr.c_str());
    if (mustQueryStr == "0" || mustQueryStr == "no"
            || mustQueryStr == "No" || mustQueryStr == "NO") {
        mMustQuery = false;
    } else {
        mMustQuery = true;
    }

    std::string btsGroupStr;
    PARSE_PROCESSOR_PARAM_UNRET(params, PARAM_BTS_GROUP, btsGroupStr);
    AGG_LOG(DEBUG, "bts_group = [%s]", btsGroupStr.c_str());
    if (btsGroupStr == "false") {
        mBtsGroupSwitch = false;
    } else {
        mBtsGroupSwitch = true;
    }

    return true;
}

int MedChatParseProcessor::Process(Context *context)
{
    AggContext* aggContext = new AggContext;
    context->SetUserContext(aggContext);
    aggContext->SetContext(context);   
    String2String paramMap;
    if (!Parse(context, paramMap)) {        
        MedicalChatHelper::SendErrorResponsePacket(aggContext, 400, "", String2String());
        return PR_REQUEST_EXIT;
    }
    auto iterExtra = mExtraParams.begin();
    for (; iterExtra != mExtraParams.end(); iterExtra++) {
        if (paramMap.find(iterExtra->first) == paramMap.end()) {
            paramMap[iterExtra->first] = iterExtra->second;
        }
    }
    aggContext->SetRequestParamMap(paramMap);
    
    AggRequest* aggRequest = new AggRequest;
    aggContext->SetAggRequest(aggRequest);
    
    /* AggRequest */
    String2StringConstIter iter = paramMap.find(REQUEST_QUERY_KEY);
    if (mMustQuery) {   // 强制要求请求串中一定有query字段，并且不为空
        if (iter == paramMap.end() || iter->second.empty()) {
            AGG_LOG(ERROR, "query field must be set");
            MedicalChatHelper::SendErrorResponsePacket(aggContext, 400, "", String2String());
            return PR_REQUEST_EXIT;
        }
        aggRequest->_query = iter->second;
    } else {   // 不对query字段做强制性要求
        if (iter != paramMap.end() && !iter->second.empty()) {
            aggRequest->_query = iter->second;
        }
    }

    iter = paramMap.find(REQUEST_START_KEY);
    if (iter != paramMap.end() && !iter->second.empty()) {
        aggRequest->_start = StringUtil::StringToUint32(iter->second);
        if (aggRequest->_start == INVALID_INT_VALUE) {
            AGG_LOG(ERROR, "start field is invalid, %s", iter->second.c_str());
            MedicalChatHelper::SendErrorResponsePacket(aggContext, 400, "", String2String());
            return PR_REQUEST_EXIT;
        }
    } else {
        aggRequest->_start = DEFAULT_START;
    }

    iter = paramMap.find(REQUEST_HIT_KEY);
    if (iter != paramMap.end() && !iter->second.empty()) {
        aggRequest->_hit = StringUtil::StringToUint32(iter->second);
        if (aggRequest->_hit == INVALID_INT_VALUE) {
            AGG_LOG(ERROR, "hit field is invalid, %s", iter->second.c_str());
            MedicalChatHelper::SendErrorResponsePacket(aggContext, 400, "", String2String());
            return PR_REQUEST_EXIT;
        }

        // 特殊逻辑, 压测和处理作弊流量
        iter = paramMap.find(REQUEST_FLOW_LEVEL_KEY);
        if (iter != paramMap.end() && (iter->second == "400" || iter->second == "100")) {
            aggRequest->_hit = std::min(uint32_t(aggRequest->_hit), uint32_t(DEFAULT_HIT_NUM));
        }
    } else {
        aggRequest->_hit = DEFAULT_HIT_NUM;
    }

    iter = paramMap.find(REQUEST_VENDOR_KEY);
    if (iter != paramMap.end() && !iter->second.empty()) {
        aggRequest->_vendor = iter->second;
    } else {
        aggRequest->_vendor = DEFAULT_VENDOR;
    }

    iter = paramMap.find(REQUEST_IP_KEY);
    if (iter != paramMap.end() && !iter->second.empty()) {
        aggRequest->_ip = iter->second;
    } else {
        aggRequest->_ip = DEFAULT_IP;
    }
    
    /* Debug */
    iter = paramMap.find(REQUEST_DEBUG_KEY);
    if (iter != paramMap.end()) {
        DebugInfo* debugInfo = new DebugInfo(aggContext);
        if (!debugInfo->Init(iter->second)) {
            AGG_LOG(ERROR, "init debug info failed");
        }
        debugInfo->SetNeedTrace(true);
        if (debugInfo->IsPkUrl()) {
            aggContext->SetStatus(CACHE_SWITCHER, false);
        }
        aggContext->SetDebugInfo(debugInfo);
    }

    /* 强制设置BucketInfo */
    BucketInfo* bucketInfo = new BucketInfo("agg", false);
    aggContext->SetBucketInfo(bucketInfo);
    iter = paramMap.find(REQUEST_BUCKET_KEY);
    if (iter != paramMap.end()) {
        if (!bucketInfo->Init(iter->second)) {
            AGG_LOG(ERROR, "init bucket info failed");
        } 
    }

    /* 分组实验开关 */
    bool btsGroupSwtich = true; 
    // 通过配置关闭
    if (mBtsGroupSwitch == false) {
        btsGroupSwtich = false;
    }
    // 通过实验关闭
    std::string bucketGroupExps;
    GET_BUCKET_VALUE(bucketInfo, "bts_exps", bucketGroupExps);
    if (bucketGroupExps == "off") {
        btsGroupSwtich = false;
    }
    // 通过Param关闭
    iter = paramMap.find(REQUEST_BTS_GROUP_KEY);
    if (iter != paramMap.end()) {
        if (iter->second == "false") {
            btsGroupSwtich = false;
        } 
    }
        
    std::string bucketGroupStr;
    if (GetBucketGroup(context, bucketGroupStr)) {
        if (!bucketInfo->Group(bucketGroupStr)) {
            AGG_LOG(ERROR, "init bucket group failed");
        } else { 
            if (btsGroupSwtich) {
                bucketInfo->SetGroupSwitcher(true);
            }
        }
    }

    std::string interleavingTest, interleavingValue;
    A4ContextHelper::GetInterleavingTest(context, interleavingTest);
    A4ContextHelper::GetInterleavingValue(context, interleavingValue);
    bucketInfo->SetInterleaving(interleavingTest, interleavingValue);

    /* Log */
    LogInfo* logInfo = new LogInfo;
    aggContext->SetLogInfo(logInfo);
    
    /* Hook: dump context to xml */
    UserContext * dumpCtx = new TestUserContext;
    aggContext->SetSharedObject("A4_CTXDUMPER", std::tr1::shared_ptr<UserContext>(dumpCtx));
     
    /* 子类可重载DoProcess成员函数, 实现自定义逻辑功能*/
    mContext = context;
    mAggContext = aggContext;
    mBucketInfo = aggContext->GetBucketInfo();
    mDebugInfo = aggContext->GetDebugInfo();
    return DoProcess(aggContext);
}



void MedChatParseProcessor::DoRegisterMetric(AmonitorRegister *monitorRegister)
{
    return;
}

BaseProcessor* MedChatParseProcessor::DoClone()
{
    return new MedChatParseProcessor(*this);
}

void MedChatParseProcessor::DoDestroy()
{
    delete this;
}

int MedChatParseProcessor::DoProcess(AggContext * aggContext)
{
    InitResponse(aggContext);
    return PR_SUCCESS;
}

bool MedChatParseProcessor::ParseJson(std::string &msg, std::map<std::string, std::string> &params) {
    std::string status;
    MESSER_UTILS_NS(json)::rapidjson::Document doc;
    doc.Parse(msg.c_str());
    if(doc.IsObject()){
        for(auto it = doc.MemberBegin();it!=doc.MemberEnd();it++){
            if ((*it).value.IsString()) {
                string tmpkey = (*it).name.GetString();
                string tmpVal = (*it).value.GetString();
                params[tmpkey] = tmpVal;
            }
        }
        if(doc.HasMember("communication")){
            const MESSER_UTILS_NS(json)::rapidjson::Value&  communication = doc["communication"];
            if(communication.HasMember("uid")){
                params["uid"] = communication["uid"].GetString();
            }
            if(communication.HasMember("sessionid")){
                params["sessionid"] = communication["sessionid"].GetString();
            }
            if(communication.HasMember("reqid")){
                params["reqid"] = communication["reqid"].GetString();
            }
        }
        if(doc.HasMember("data")){
            const MESSER_UTILS_NS(json)::rapidjson::Value&  data = doc["data"];
            if(data.HasMember("message")){
                const MESSER_UTILS_NS(json)::rapidjson::Value& message = data["message"];
                if(message.HasMember("content")){
                    params["query"] = message["content"].GetString();
                }
                else if(message.HasMember("query")){
                    params["query"] = message["query"].GetString();
                }
                if(message.HasMember("content_rewrite")){
                    params["query_rewrite"] = message["content_rewrite"].GetString();
                }
                else if(message.HasMember("query_rewrite")){
                    params["query_rewrite"] = message["query_rewrite"].GetString();
                }
                if(message.HasMember("status")){
                    status = message["status"].GetString();
                    params["status"] = message["status"].GetString();
                }
            }
        }
        if(doc.HasMember("bucket") and doc["bucket"].IsObject()){
            const MESSER_UTILS_NS(json)::rapidjson::Value& bucket_paramVal = doc["bucket"];
            MESSER_UTILS_NS(util)::ostringstream ss;
            bool first = true;
            for(auto it = bucket_paramVal.MemberBegin();it!=bucket_paramVal.MemberEnd();it++){
                string tmpkey = (*it).name.GetString();
                string tmpVal = (*it).value.GetString();
                if (first) {
                    ss << tmpkey << "=" << tmpVal;
                    first = false;
                }
                else
                {
                    ss << "&" << tmpkey << "=" << tmpVal;
                }
            }
            if (!first) {
                params[REQUEST_BUCKET_KEY] = ss.str();
            }
        }
        if(doc.HasMember("params") and doc["params"].IsObject()){
            const MESSER_UTILS_NS(json)::rapidjson::Value& bucket_paramVal = doc["params"];
            MESSER_UTILS_NS(util)::ostringstream ss;
            bool first = true;
            for(auto it = bucket_paramVal.MemberBegin();it!=bucket_paramVal.MemberEnd();it++){
                string tmpkey = (*it).name.GetString();
                string tmpVal = (*it).value.GetString();
                params[tmpkey] = tmpVal;            
            }
        }
    }

    return true;
}

bool MedChatParseProcessor::Parse(Context* context, String2String& params)
{
    HttpPacket* requestPacket = context->GetRequestPacket();
    if (requestPacket == NULL) {
        return false;
    }

    std::string args;
    HttpMethod method = requestPacket->GetMethod();
    if (method == HM_GET) {
        if (!requestPacket->GetArgs(args)) {
            AGG_LOG(ERROR, "get args failed for http get method");
            return false;
        }
        if (!UrlUtil::EnhanceDecodeUrlKv(args, params)) {
            AGG_LOG(ERROR, "url decode args[%s] failed", args.c_str());
            return false;
        }
    } else if (method == HM_POST) {
        if (!requestPacket->GetBody(args)) {
            AGG_LOG(ERROR, "get args failed for http post method");
            return false;
        }
        if (!ParseJson(args,params)) {
            AGG_LOG(ERROR, "parse args failed for http post method");
            return false;
        }
    } else {
        AGG_LOG(ERROR, "illegal http method[%d]", method);
        return false;
    }

    return true;
}

bool MedChatParseProcessor::GetBucketGroup(Context* context, std::string& bucketGroupStr)
{
    HttpPacket* requestPacket = context->GetRequestPacket();
    if (requestPacket == NULL) {
        return false;
    }

    if (!requestPacket->GetHeader(REQUEST_BUCKET_KEY, bucketGroupStr)) {
        AGG_LOG(ERROR, "get header args[%s] failed", bucketGroupStr.c_str());
        return false;
    }

    return true;
}

AGG_END_NAMESPACE(biz);
