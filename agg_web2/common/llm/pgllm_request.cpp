#include <agg_web2/common/llm/pgllm_request.h>
#include <agg2/upstream/llm_cache.h>
#include <agg2/util/string_tool.h>
#include <agg2/util/url_util.h>
#include <jsoncpp/json/json.h>
#include <agg2/common/rapidjson.h>
#include <messer_utils/json/JsonParser.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, PgLLMRequest);

BEGIN_DEFINE_SERIALIZATION_FUNC(PgLLMRequest) {
    ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(HttpRequest);
    ar &BOOST_SERIALIZATION_NVP(mAggContext);
}
END_DEFINE_SERIALIZATION_FUNC(PgLLMRequest)


PgLLMRequest::PgLLMRequest()
{ 
 
}

PgLLMRequest::PgLLMRequest(AggContext *aggContext,const std::string &model):QaLLMRequest(aggContext,model)
{

}

PgLLMRequest::~PgLLMRequest() {}

bool PgLLMRequest::DoEncode(std::string &uriQuery, std::string &body,
                            String2String &headers) 
{
    headers["Content-Type"] = "application/json";
    headers["token"] = mApiKey;
    headers["User-Agent"] = "curl/7.29.0";
    headers["Host"] = mHost;
    headers["Accept"] = "*/*";
    std::string uid;
    mAggContext->GetRequestParam("uid", uid);

    AGG_LOG(INFO, "PgLLMRequest token:[%s] Host:[%s]", mApiKey.c_str(), mHost.c_str());
    
    JsonDocument root(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
    SetJson(root, root, "model", mModel);
    if(!mRequestId.empty()){
        SetJson(root, root, "request_id", mRequestId);
    }
    if (!mPrompt.empty()) {
        SetJson(root, root, "prompt", mPrompt);
    }
    else
    {
        JsonValue promptIts(MESSER_UTILS_NS(json)::rapidjson::kArrayType);
        JsonValue msgIt(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
        SetJson(root, msgIt, "role", "user");
        SetJson(root, msgIt, "content", mContent);
        promptIts.PushBack(msgIt,root.GetAllocator());
        SetJson(root, root, "prompt", promptIts);
    }
    SetJson(root, root, "user_id", uid);
    SetJson(root, root, "nickname", uid);
#if 0
    AggDataWrapper wrapper(AggDataManager::GetInstance(), "llm_extra_args");
    LLMExtraArgs * llmExtraArgs = wrapper.Get<LLMExtraArgs>(); 
    if (NULL != llmExtraArgs) {
        std::string content = llmExtraArgs->GetContent();
        JsonDocument extra_doc(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
        extra_doc.Parse<0>(content.c_str());
        if(!extra_doc.HasParseError() && extra_doc.IsObject()) {
            if(extra_doc.HasMember("extra_args") && extra_doc["extra_args"].IsObject()){
                SetJson(root, root, "params", extra_doc["extra_args"]);
            }
        }
    }
#endif
    JsonDocument extra_args(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
    if (mMaxNewTokens > 0) {
        SetJson(root,extra_args,"max_new_tokens",mMaxNewTokens);
    }
    if (mTemperature > 0) {
        SetJson(root,extra_args,"temperature",mTemperature);
    }
    SetJson(root, root, "params",extra_args);
    JsonDocument passparams(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
    SetJson(root, root, "passparams",passparams);
    SetJson(root, root, "tag", "web_chat_client");
    SetJson(root, root, "app", "web_chat_client");
    //SetJson(root, root, "category", "通用");
    MESSER_UTILS_NS(json)::rapidjson::StringBuffer buffer(0, 200*1024);
    MESSER_UTILS_NS(json)::rapidjson::Writer<MESSER_UTILS_NS(json)::rapidjson::StringBuffer> writer(buffer);
    root.Accept(writer);
    body = std::string(buffer.GetString());
    
    AGG_LOG(INFO, "pgLLMRequest body:[%s]\n\n",body.c_str());
    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::PgLLMRequest);

