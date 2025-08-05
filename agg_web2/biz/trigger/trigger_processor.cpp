#include <agg_web2/biz/trigger/trigger_processor.h>
#include "agg_web2/common/common.h"
#include <agg_web2/upstream/common/common_request.h>
#include <agg_web2/upstream/common/common_result.h>
#include <agg2/helper/agg_context_helper.h>
#include <agg2/util/string_tool.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, TriggerProcessor);

// serialize class object
BEGIN_DEFINE_SERIALIZATION_FUNC(TriggerProcessor)
    // Example: serialize mField
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(TriggerProcessor)

TriggerProcessor::TriggerProcessor()
{
}

TriggerProcessor::~TriggerProcessor()
{
}

bool TriggerProcessor::DoInit(const String2String & params)
{
    return true;
}

void TriggerProcessor::DoRegisterMetric(AmonitorRegister * monitorRegister)
{
}

int TriggerProcessor::DoProcess(AggContext * aggContext)
{
    int64_t start_time = autil::TimeUtility::currentTimeInMicroSeconds();

    // step 1：流量分发，风险流量 / 非高考问答流量，进行过滤
    string query = AggContextHelper::GetRawQuery(aggContext);
    if (!query.empty())
        query = StringTool::ReplaceStr(query, "\n", "");

    std::vector<std::string> intents = GetQueryGaokaoIntent(query);
    if (intents.size() == 0) { // 访问失败，重新访问一次，重试2次
        intents = GetQueryGaokaoIntent(query);
        if (intents.size() == 0) {
            intents = GetQueryGaokaoIntent(query);
        }
    }
        
    aggContext->SetValue("QUERY_INTENTS", intents); 

    bool needTrigger = true;
    if (intents.size() == 0) { //TODO：空可能是获取失败
        needTrigger = false;
    }
    
    if (std::find(intents.begin(), intents.end(), "其他-非高考") != intents.end() || 
            std::find(intents.begin(), intents.end(),"其他-高考相关") != intents.end()) {
        needTrigger = false;
    }

    AGG_LOG(ERROR, "Intent:[%s][%s]", query.c_str(), autil::StringUtil::toString(intents, ",").c_str());
    
    int64_t end_time = autil::TimeUtility::currentTimeInMicroSeconds();
    AGG_LOG(ERROR, "TriggerProcessor Cost:%d", end_time - start_time);

    if (!needTrigger) {
        return PR_REQUEST_EXIT;
    }
    
    return PR_SUCCESS;
}
    
std::vector<std::string> TriggerProcessor::GetQueryGaokaoIntent(std::string& query)
{
    vector<string> intents;
    HttpRequestPtr httpRequest(new CommonRequest(query, "gaokao_intent"));
    AggHttpWrapperPtr wrapper = GetHttpWrapper(mAggContext, "gaokao_intent_service"); 
    HttpResponsePtr aggResponse = wrapper->AsyncRequest(httpRequest);
    if (aggResponse) {
        ResultPtr intentResult(new CommonResult("gaokao_intent"));
        if (aggResponse->GetResult(intentResult)) {
            CommonResultPtr intentPtr = dynamic_pointer_cast<CommonResult>(intentResult);
            intents = intentPtr->mIntents;
        }
    }
    return intents;
}

BaseProcessor * TriggerProcessor::DoClone()
{
    return new TriggerProcessor(*this);
}

void TriggerProcessor::DoDestroy()
{
    delete this;
}

bool TriggerProcessor::RegisterMethods()
{
    return true;
}

bool TriggerProcessor::RegisterActors()
{
    return true;
}

AGG_END_NAMESPACE(agg_web2);

