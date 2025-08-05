#include <agg_web2/common/llm/qa_llm_request.h>
#include <agg2/upstream/llm_cache.h>
#include <agg2/util/string_tool.h>
#include <agg2/util/url_util.h>
#include <jsoncpp/json/json.h>
#include <agg2/common/rapidjson.h>
#include <messer_utils/json/JsonParser.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, QaLLMRequest);

BEGIN_DEFINE_SERIALIZATION_FUNC(QaLLMRequest) {
    ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(HttpRequest);
    ar &BOOST_SERIALIZATION_NVP(mAggContext);
}
END_DEFINE_SERIALIZATION_FUNC(QaLLMRequest)


QaLLMRequest::QaLLMRequest() : mAggContext(nullptr)
{ 
    SetHttpMethod(HM_POST);
    mMaxNewTokens = 0;
    mTemperature = 0;
    mProtoFormat = "shenma_ai";
    mRole = "user"; 
}

QaLLMRequest::QaLLMRequest(AggContext *aggContext,const std::string &model) : mAggContext(aggContext)
                                                                                         ,mModel(model)
{
    SetHttpMethod(HM_POST);
    mMaxNewTokens = 0;
    mTemperature = 0;
    mProtoFormat = "shenma_ai";
    mRole = "user";
}

QaLLMRequest::~QaLLMRequest() {}

bool QaLLMRequest::DoEncode(std::string &uriQuery, std::string &body,
                            String2String &headers) 
{
    headers["Content-Type"] = "application/json";
    if(!mApiKey.empty()){
        headers["Authorization"] = mApiKey;
    }
    
    if(!mRequestId.empty()){
        JsonDocument root(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
        SetJson(root, root, "request_id", mRequestId);
        MESSER_UTILS_NS(json)::rapidjson::StringBuffer buffer(0, 200*1024);
        MESSER_UTILS_NS(json)::rapidjson::Writer<MESSER_UTILS_NS(json)::rapidjson::StringBuffer> writer(buffer);
        root.Accept(writer);
        body = std::string(buffer.GetString());
    }else{
        JsonDocument root(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
        SetJson(root, root, "model", mModel);

        JsonValue messageIts(MESSER_UTILS_NS(json)::rapidjson::kArrayType);

        if (mNeedHistory) { // 要加历史
            LLMCachePtr llmCachePtr = mAggContext->GetValue<LLMCache>(LLM_CACHE_AGG_KEY);
            if (llmCachePtr) {
                LLMHistoryVec & historyVec = llmCachePtr->GetHistory();
                for (auto history : historyVec) {
                    JsonValue msgIt(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
                    SetJson(root, msgIt, "role", history->role);
                    SetJson(root, msgIt, "content", history->content);
                    messageIts.PushBack(msgIt,root.GetAllocator());
                }
            }
        }

        JsonValue msgIt(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
        SetJson(root, msgIt, "role", mRole);
        SetJson(root, msgIt, "content", mContent);
        messageIts.PushBack(msgIt,root.GetAllocator());
        SetJson(root, root, "message", messageIts);
        JsonDocument extra_args(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
        if (mMaxNewTokens > 0) {
            SetJson(root,extra_args,"max_new_tokens",mMaxNewTokens);
        } 
        if (mTemperature > 0) {
            SetJson(root,extra_args,"temperature",mTemperature);
        }
        SetJson(root, root, "extra_args",extra_args);
        MESSER_UTILS_NS(json)::rapidjson::StringBuffer buffer(0, 200*1024);
        MESSER_UTILS_NS(json)::rapidjson::Writer<MESSER_UTILS_NS(json)::rapidjson::StringBuffer> writer(buffer);
        root.Accept(writer);
        body = std::string(buffer.GetString());
    }
    AGG_LOG(INFO, "genralQaLLMRequest body:[%s]\n\n",body.c_str());
    return true;

}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::QaLLMRequest);

