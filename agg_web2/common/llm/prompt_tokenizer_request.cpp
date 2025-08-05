#include <agg_web2/common/llm/prompt_tokenizer_request.h>
#include <agg2/upstream/llm_cache.h>
#include <agg2/util/string_tool.h>
#include <agg2/util/url_util.h>
#include <jsoncpp/json/json.h>
#include <agg2/common/rapidjson.h>
#include <messer_utils/json/JsonParser.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, PromptTokenizerRequest);

BEGIN_DEFINE_SERIALIZATION_FUNC(PromptTokenizerRequest) {
    ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(HttpRequest);
}
END_DEFINE_SERIALIZATION_FUNC(PromptTokenizerRequest)


PromptTokenizerRequest::PromptTokenizerRequest()
{ 
 
}

PromptTokenizerRequest::PromptTokenizerRequest(AggContext *aggContext,const std::string &model) : QaLLMRequest(aggContext,model)
{

}

PromptTokenizerRequest::~PromptTokenizerRequest() {}

bool PromptTokenizerRequest::DoEncode(std::string &uriQuery, std::string &body,
                            String2String &headers) 
{
    headers["Content-Type"] = "application/json";

        JsonDocument root(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
        SetJson(root, root, "model", mModel);
        SetJson(root, root, "proto_format", mProtoFormat);
        JsonValue messageIts(MESSER_UTILS_NS(json)::rapidjson::kArrayType);
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
        JsonValue msgIt(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
        SetJson(root, msgIt, "role", mRole);
        SetJson(root, msgIt, "content", mContent);
        messageIts.PushBack(msgIt,root.GetAllocator());
        SetJson(root, root, "message", messageIts);
#if 0
        AggDataWrapper wrapper(AggDataManager::GetInstance(), "llm_extra_args");        
        LLMExtraArgs * llmExtraArgs = wrapper.Get<LLMExtraArgs>(); 
        if (NULL != llmExtraArgs) {
            std::string content = llmExtraArgs->GetContent();
            JsonDocument extra_doc(MESSER_UTILS_NS(json)::rapidjson::kObjectType);
            extra_doc.Parse<0>(content.c_str());
            if(!extra_doc.HasParseError() && extra_doc.IsObject()) {
                if(extra_doc.HasMember("extra_args") && extra_doc["extra_args"].IsObject()){
                    SetJson(root, root, "extra_args", extra_doc["extra_args"]);
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
        SetJson(root, root, "extra_args",extra_args);
        MESSER_UTILS_NS(json)::rapidjson::StringBuffer buffer(0, 200*1024);
        MESSER_UTILS_NS(json)::rapidjson::Writer<MESSER_UTILS_NS(json)::rapidjson::StringBuffer> writer(buffer);
        root.Accept(writer);
        body = std::string(buffer.GetString());
    
    AGG_LOG(INFO, "PromptTokenizerRequest body:[%s]\n\n",body.c_str());
    return true;

}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::PromptTokenizerRequest);

