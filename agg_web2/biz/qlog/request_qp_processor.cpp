#include <agg_web2/biz/qlog/request_qp_processor.h>
#include <agg2/upstream/llm_cache.h>
#include "agg_web2/common/common.h"
#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg2/helper/agg_context_helper.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <agg_web2/helper/medical_chat_helper.h>
#include <messer_utils/json/rapidjson/document.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, RequestQpProcessor);

const std::string QP_NODE_NAME_PARAM_KEY = "qp_node";
const std::string CHANNELS_CLUSTERS_PARAM_KEY = "channels_clusters";
const std::string QP_KV_ITEM_PARAM_KEY = "kv_item";
const std::string QP_CONFIG_ITEM_PARAM_KEY = "config_item";

const std::string KV_KEY_SPLITER = ",";
const std::string KV_KEY_VALUE_SPLITER = ":";

//const std::string CONFIG_KEY_SPLITER = ",";
//const std::string CONFIG_KEY_VALUE_SPLITER = ":";

// serialize class object
BEGIN_DEFINE_SERIALIZATION_FUNC(RequestQpProcessor)
    // Example: serialize mField
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(RequestQpProcessor)

RequestQpProcessor::RequestQpProcessor()
{
}

RequestQpProcessor::~RequestQpProcessor()
{
}

bool RequestQpProcessor::DoInit(const String2String & params)
{
    PARSE_PROCESSOR_PARAM(params, QP_NODE_NAME_PARAM_KEY, mQpNodeName);
    PARSE_PROCESSOR_PARAM_MAP(params, QP_KV_ITEM_PARAM_KEY, KV_KEY_SPLITER, KV_KEY_VALUE_SPLITER, mKvItemMap);
    PARSE_PROCESSOR_PARAM_MAP(params, QP_CONFIG_ITEM_PARAM_KEY, KV_KEY_SPLITER, KV_KEY_VALUE_SPLITER, mConfigItemMap);
    
    std::string value;
    PARSE_PROCESSOR_PARAM(params, CHANNELS_CLUSTERS_PARAM_KEY, value);
    StringTokenizer tokenizer;
    size_t count = tokenizer.Tokenize(value, ";", StringTokenizer::TOKEN_TRIM);
    std::set<std::string> allChannel;
    for (size_t i = 0; i < count; ++i) {
        std::string token = tokenizer[i];
        StringTokenizer tokenizer2;
        size_t count2 = tokenizer2.Tokenize(token, ":", StringTokenizer::TOKEN_TRIM);
        if (count2 != 3) {
            AGG_LOG(ERROR, "channel and cluster count is not equal 4");
            return false;
        }
        if (allChannel.find(tokenizer2[1])  == allChannel.end()) {
            if (mChannels != "") {
                mChannels += ",";
            }
            mChannels += tokenizer2[1];
            allChannel.insert(tokenizer2[1]);
        }
        
        std::vector<std::string> tmpVec; 
        tmpVec.push_back(tokenizer2[0]);
        tmpVec.push_back(tokenizer2[1]);
        tmpVec.push_back(tokenizer2[2]);
        mQpRuleItemVec.push_back(tmpVec);
    }
    return true;
}

void RequestQpProcessor::DoRegisterMetric(AmonitorRegister * monitorRegister)
{
}

bool RequestQpProcessor::InitQpRequest(AggContext *aggContext, QpRequest & qpRequest,const string &query,const String2String &kvItemMap,const String2String &configItemMap,const std::string &channels)
{
    String2String kvMap, configMap, clauseMap;
    clauseMap["cluster"] = "xxx";
    clauseMap["query"] = "phrase:'" + query + "'";
    
    //QpCollectorPtr qpCollector = AggContextHelper::GetQpCollectorPtr(aggContext); 

    for (String2StringConstIter iter = kvItemMap.begin(); iter != kvItemMap.end(); ++iter) {
        std::string value;
        if (!aggContext->GetRequestParam(iter->first, value) || value.empty()) {
            if (iter->second.empty()) {
                AGG_LOG(WARN, "Cant't Find Necessory Kv:%s in RequestParam!", iter->first.c_str());
                return false;
            }
            value = iter->second;
        }
        kvMap[iter->first] = value;
    }

    kvMap["disable_repeat_score"] = "1";

    for (String2StringConstIter iter = configItemMap.begin(); iter != configItemMap.end(); ++iter) {
        std::string value;
        if (!aggContext->GetRequestParam(iter->first, value) || value.empty()) {
            if (iter->second.empty()) {
                AGG_LOG(WARN, "Cant't Find Necessory Config:%s in RequestParam!", iter->first.c_str());
                return false;
            }
            value = iter->second;
        }
        configMap[iter->first] = value;
    }
    
    qpRequest.SetKvMap(kvMap);
    qpRequest.SetConfigMap(configMap);
    qpRequest.SetClauseMap(clauseMap);
    qpRequest.SetRawQuery(query);
    qpRequest.SetType(channels);
    
    //std::string ip = aggContext->GetAggRequest()->_ip;
    //if (ip == "") {
    //    ip = "127.0.0.1";
    //}
    qpRequest.SetIp("127.0.0.1");
    qpRequest.SetFilterId("0");

    /* qpRequest.SetFilterId(AggWebContextHelper::GetFilterId(aggContext));
    
    String2String generalParamMap = qpRequest.GetGeneralParamMap();
    generalParamMap["bucket"] = AggContextHelper::GetBucketInfoStr(aggContext);
    generalParamMap["city"] = AggWebContextHelper::GetCity(mAggContext);
    generalParamMap["province"] = AggWebContextHelper::GetProvince(aggContext);
    generalParamMap["district"] = AggWebContextHelper::GetDistrict(aggContext);
    generalParamMap["hid"] = AggContextHelper::GetRequestParam(aggContext, REQUEST_HID_KEY, "");

    if (interleavingTest != "" && interleavingValue != "") {
        generalParamMap["interleaving"] = interleavingTest + "=" + interleavingValue;
    }

    MockHelper::MockSpellCheck(aggContext, generalParamMap);
    qpRequest.SetGeneralParamMap(generalParamMap);

    DEBUG_VAR(channels);
    DEBUG_VAR(query);
    */

    std::string qpRequestQuery, debugPlain;
    qpRequest.EncodeAndDebug(qpRequestQuery, debugPlain);

    AGG_LOG(DEBUG, "qp str:[%s][%s]", qpRequestQuery.c_str(), debugPlain.c_str());
    
    return true;
}

QpResultPtr RequestQpProcessor::GetQpResult(QpRequestPtr qpRequest, bool setQpResult)
{
    if (!qpRequest) {
        AGG_LOG(ERROR, "qpRequest is null");
        return {};
    }
    qpRequest->SetHttpMethod(HM_POST);
    HttpRequestPtr httpRequest = qpRequest;
    QpResultPtr qpResult(new QpResult);
    ResultPtr result = qpResult;
    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, mQpNodeName);
    if (!wrapper->SyncRequest(httpRequest, result)) {
        AggWebContextHelper::AddSharedInfo(mAggContext, KEY_CONTEXT_FAILED_NODE, mQpNodeName);
        AGG_LOG(ERROR, "GetQpResult failed");
        return {};
    }

    //if (setQpResult) {
    mAggContext->SetQpResult(qpResult);
        //QpCollectorPtr qpCollector = AggContextHelper::GetQpCollectorPtr(mAggContext);
        //qpCollector->SetValue<QpResult>(MODULE_QP,KEY_QP_RESULT,qpResult);
        //qpCollector->SetValue(MODULE_QP,KEY_QP_RAW_RESULT_STR,qpResult->GetBody());
    //}
    AGG_LOG(INFO, "GetQpResult Succ.");
    return qpResult;
}

int RequestQpProcessor::DoProcess(AggContext * aggContext)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();
    string rawQuery = AggContextHelper::GetRawQuery(aggContext);
    if (!rawQuery.empty()) {
        rawQuery = StringTool::ReplaceStr(rawQuery, "\n", "");
    }
    
    QpRequestPtr qpRequest(new QpRequest());
    // 初始化QpRequest，用来访问QP
    if (!InitQpRequest(mAggContext, *qpRequest, rawQuery, mKvItemMap, mConfigItemMap, mChannels)) {
        SET_CONTEXT_ERROR_INFO(mContext, "InitQpResultError!");        
        return PR_REQUEST_EXIT;
    }
    
    // prepare params for multi exp and dct imp
    //mAggContext->SetValue(CONTEXT_QP_REQUEST_KEY, qpRequest);
    //std::string qpRequestQuery, debugPlain;
    //qpRequest->EncodeAndDebug(qpRequestQuery, debugPlain);
    //mAggContext->SetGeneralInfo("QpQuery", debugPlain);
    //
    AGG_LOG(ERROR, "debug, channels:[%s]", mChannels.c_str());

    QpResultPtr qpResult = GetQpResult(qpRequest, true);

    if (qpResult) {
        if (!GenerateSubContextWithQpResult(qpResult, aggContext)) {
            AGG_LOG(ERROR, "Gen Sub Content Failed!");
            return PR_REQUEST_EXIT;
        }
    } else {
        AGG_LOG(ERROR, "GetQpResult Failed!"); 
        return PR_REQUEST_EXIT;
    }
    
    AGG_LOG(INFO, "GetQpResult And InitSubContent Succ."); 
    
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "RequestQpProcessor Cost:%d", end_time - start_time);
    
    //std::string debuginfo = "RequestQpProcessor Cost:" + autil::StringUtil::toString(end_time - start_time);
    //AggWebContextHelper::AppendProcessorDebugInfo(aggContext, debuginfo);
    
    return PR_SUCCESS;
}

bool RequestQpProcessor::GenerateSubContextWithQpResult(QpResultPtr qpResult,
        AggContext* aggContext)
{
    if(qpResult==NULL)
        return false;

    std::vector<QrsClauseMap> qrsClauseMapVec;
    size_t qpHitCount = qpResult->GetQpHitCount();
    if (qpHitCount > 2) {
        qpHitCount = 2;
    }
    
    for (size_t i = 0; i < qpHitCount; i++) {
        QpHit* qpHit = qpResult->GetQpHit(i);
        QrsClauseMap qrsClauseMap;
        qrsClauseMapVec.push_back(qrsClauseMap);
        QrsClauseMap &qrsClauseMapRef = qrsClauseMapVec[qrsClauseMapVec.size() - 1];
        AGG_LOG(INFO, "qp hit count:%d", (int32_t)i);
        if (!GenerateQrsClauseMapByQpHit(qpHit, qrsClauseMapRef)) {
            ReleaseQrsClauseMapVec(qrsClauseMapVec);
            return false;
        }
        
        /*if (i == 1) {
            if (!GenerateSubContext(aggContext, qrsClauseMapVec)) {
                ReleaseQrsClauseMapVec(qrsClauseMapVec);
                return false;
            }
            qrsClauseMapVec.clear();
        }*/
    }

    if (!GenerateSubContext(aggContext, qrsClauseMapVec)) {
        ReleaseQrsClauseMapVec(qrsClauseMapVec);
        return false;
    }

    /*if (qpHitCount != 2 &&
        !GenerateSubContext(aggContext, qrsClauseMapVec)) {
        ReleaseQrsClauseMapVec(qrsClauseMapVec);
        return false;
    }*/

    return true;
}

void RequestQpProcessor::ReleaseQrsClauseMapVec(std::vector<QrsClauseMap>& qrsClauseMapVec)
{
    size_t count = qrsClauseMapVec.size();
    for (size_t i = 0; i < count; i++) {
        QrsClauseMap& qrsClauseMap = qrsClauseMapVec[i];
        for (QrsClauseMapIter iter = qrsClauseMap.begin();
             iter != qrsClauseMap.end(); ++iter) {
            A4_DELETE_AND_SET_NULL(iter->second);
        }
    }
}

bool RequestQpProcessor::GenerateQrsClauseMapByQpHit(QpHit* qpHit, QrsClauseMap& qrsClauseMap)
{
    size_t qpRuleItemCount = mQpRuleItemVec.size();
    for (size_t j = 0; j < qpRuleItemCount; j++) {
        std::vector<std::string>& qpRuleItem = mQpRuleItemVec[j];
        std::string bytes;
        if (!qpHit->GetContent("HA2" + qpRuleItem[1], bytes)) {
            AGG_LOG(ERROR, "get qrs request from qp hit failed");
            return false;
        }
        AGG_LOG(INFO, "qp bytes:[%s][%s][%s][%s]", qpRuleItem[0].c_str(),qpRuleItem[1].c_str(),qpRuleItem[2].c_str(), bytes.c_str());

        QrsClause* qrsClause = new QrsClause;
        if (!qrsClause->Decode(bytes)) {
            AGG_LOG(ERROR, "decode qrs clause[%s] failed", bytes.c_str());
            A4_DELETE_AND_SET_NULL(qrsClause);
            return false;
        }
        qrsClause->SetClusterName(qpRuleItem[2]);
        qrsClauseMap[qpRuleItem[0]] = qrsClause;
    }

    return true;
}

bool RequestQpProcessor::GenerateSubContext(AggContext* aggContext,
                                     std::vector<QrsClauseMap>& qrsClauseMapVec)
{
    QueryCollector* queryCollector = new QueryCollector;
    if (!queryCollector->Init(qrsClauseMapVec)) {
        AGG_LOG(ERROR, "init query collector failed");
        A4_DELETE_AND_SET_NULL(queryCollector);
        return false;
    }

    AggSubContext* subContext = new AggSubContext;
    subContext->SetQueryCollector(queryCollector);
    subContext->SetContext(aggContext);
    aggContext->AddSubContext(subContext);
    return true;
}


BaseProcessor * RequestQpProcessor::DoClone()
{
    return new RequestQpProcessor(*this);
}

void RequestQpProcessor::DoDestroy()
{
    delete this;
}

bool RequestQpProcessor::RegisterMethods()
{
    return true;
}

bool RequestQpProcessor::RegisterActors()
{
    return true;
}

AGG_END_NAMESPACE(agg_web2);


