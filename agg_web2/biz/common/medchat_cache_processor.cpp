#include <agg_web2/biz/common/medchat_cache_processor.h>
#include <agg2/upstream/llm_cache.h>
#include "agg_web2/common/common.h"
#include <agg2/wrapper/agg_http_wrapper.h>
#include <agg2/helper/agg_context_helper.h>
#include <agg_web2/helper/agg_web_context_helper.h>
#include <agg_web2/helper/medical_chat_helper.h>

AGG_BEGIN_NAMESPACE(processor);
AGG_LOG_SETUP(agg_web2, MedChatCacheProcessor);

const static string TAIR_NODE_NAME_KEY = "session_tair_node";
const static string PARAM_MAX_HISTORY_SIZE = "llm_max_history_size";

const std::string METHOD_READ_CACHE = "read_llm_cache";
const std::string METHOD_UPDATE_CACHE = "update_llm_cache";
// serialize class object
BEGIN_DEFINE_SERIALIZATION_FUNC(MedChatCacheProcessor)
    // Example: serialize mField
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(MedChatCacheProcessor)

MedChatCacheProcessor::MedChatCacheProcessor()
{
}

MedChatCacheProcessor::~MedChatCacheProcessor()
{
}

bool MedChatCacheProcessor::DoInit(const String2String & params)
{
    PARSE_PROCESSOR_PARAM(params, TAIR_NODE_NAME_KEY, mTairNodeName);
    PARSE_PROCESSOR_PARAM_UNRET(params, PARAM_MAX_HISTORY_SIZE, mMaxHistorySize);
    return true;
}

void MedChatCacheProcessor::DoRegisterMetric(AmonitorRegister * monitorRegister)
{
}

int MedChatCacheProcessor::ReadCache(AggContext * aggContext)
{
    //create
    LLMCachePtr llmCachePtr(new LLMCache());
    aggContext->SetValue<LLMCache>(LLM_CACHE_AGG_KEY,llmCachePtr);
    llmCachePtr->SetMaxHistorySize(mMaxHistorySize);
    //get
    AggCacheWrapperPtr wrapper = GetCacheWrapper(aggContext, mTairNodeName);
    int status = 0; 
    std::string sessionid;
    mAggContext->GetRequestParam("sessionid", sessionid);
    if (sessionid.empty()) {
        AGG_LOG(ERROR,"get session empty");
        return PR_SUCCESS;
    }
    std::string cacheKey = LLM_CACHE_KEY_PREFIX+sessionid;
    std::string cacheVal;
    if (!wrapper->Get(cacheKey, cacheVal, status)) {
        AGG_LOG(ERROR, "Get [%s] Tair Failed.", sessionid.c_str());    
        return PR_SUCCESS;
    }
    if (llmCachePtr->Deserialize(cacheVal)){
        AGG_LOG(ERROR,"llm cache deserialize failed");
    }
    return PR_SUCCESS;
}

int MedChatCacheProcessor::UpdateCache(AggContext * aggContext)
{
    AggCacheWrapperPtr wrapper = GetCacheWrapper(aggContext, mTairNodeName);
    MedicalChatHelper::UpdateLLMCache(aggContext,wrapper);
    return PR_SUCCESS;
}

BaseProcessor * MedChatCacheProcessor::DoClone()
{
    return new MedChatCacheProcessor(*this);
}

void MedChatCacheProcessor::DoDestroy()
{
    delete this;
}

bool MedChatCacheProcessor::RegisterMethods()
{
    return RegisterMethod(METHOD_READ_CACHE, &MedChatCacheProcessor::ReadCache)
           && RegisterMethod(METHOD_UPDATE_CACHE, &MedChatCacheProcessor::UpdateCache);
}

bool MedChatCacheProcessor::RegisterActors()
{
    return true;
}

AGG_END_NAMESPACE(agg_web2);


