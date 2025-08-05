#include "agg_web2/biz/common/latency_calculate_processor.h"
#include "agg_web2/common/metrics_helper.h"
#include <A4/util/time_util.h>

AGG_BEGIN_NAMESPACE(common)
AGG_LOG_SETUP(agg_web2, LatencyCalculateProcessor);

// serialize class object
BEGIN_DEFINE_SERIALIZATION_FUNC(LatencyCalculateProcessor)
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseProcessor);
END_DEFINE_SERIALIZATION_FUNC(LatencyCalculateProcessor)


bool LatencyCalculateProcessor::DoInit(const String2String& params)
{
    PARSE_PROCESSOR_PARAM_UNRET(params, PARAM_CACHE_SUCCESSED_SESSION_LATENCY_KEY, mCacheSuccessedSessionLatencyKey)
    PARSE_PROCESSOR_PARAM_UNRET(params, PARAM_CACHE_FAILED_SESSION_LATENCY_KEY, mCacheFailedSessionLatencyKey)
    PARSE_PROCESSOR_PARAM_UNRET(params, PARAM_SESSION_LATENCY_KEY, mSessionLatencyKey)
    return true;
}

void LatencyCalculateProcessor::DoRegisterMetric(AmonitorRegister* monitorRegister)
{
    if (!mCacheSuccessedSessionLatencyKey.empty() && !mCacheFailedSessionLatencyKey.empty()) {
        monitorRegister->RegisterValueMetric(mCacheSuccessedSessionLatencyKey, "ms");
        monitorRegister->RegisterValueMetric(mCacheFailedSessionLatencyKey, "ms");
    }

    if (!mSessionLatencyKey.empty()) {
        monitorRegister->RegisterValueMetric(mSessionLatencyKey, "ms");
    }

    
    // latency
    monitorRegister->RegisterValueMetric(KEY_LATENCY_PHASE2_TOTAL, "ms");
    monitorRegister->RegisterValueMetric(KEY_LATENCY_PHASE3_TOTAL, "ms");
    monitorRegister->RegisterValueMetric(KEY_LATENCY_PHASE4_TOTAL, "ms");
    monitorRegister->RegisterValueMetric(KEY_LATENCY_PHASE5_TOTAL, "ms");
    monitorRegister->RegisterValueMetric(KEY_LATENCY_PHASE6_TOTAL, "ms");
}

int LatencyCalculateProcessor::DoProcess(AggContext* aggContext)
{
    uint64_t sessionStartTime = mContext->GetStartProcessTime();
    if (sessionStartTime != 0) {
        uint64_t nowTime = TimeUtil::GetTimeInMs();
        uint64_t sessionLatency = nowTime - sessionStartTime;
        // bool cacheReadStatus = false;
        auto* item = mContext->MutableMetricItem();

        if (!mCacheSuccessedSessionLatencyKey.empty() && !mCacheFailedSessionLatencyKey.empty()) {
            bool mainCacheReadStatus = false;
            bool rtCacheReadStatus = false;
            mAggContext->GetValue(MAIN_CACHE_READ_STATUS, mainCacheReadStatus);
            mAggContext->GetValue(RT_CACHE_READ_STATUS, rtCacheReadStatus);

            if (mainCacheReadStatus && rtCacheReadStatus) {
                item->SetValue(mCacheSuccessedSessionLatencyKey, sessionLatency);
            } else {
                item->SetValue(mCacheFailedSessionLatencyKey, sessionLatency);
            }
        }

        if (!mSessionLatencyKey.empty()) {
            item->SetValue(mSessionLatencyKey, sessionLatency);
        }
    }
    
    // latency
    MetricsHelper::GetPhase6EndTime(mAggContext);
    MetricsHelper::GetLatency(mAggContext);

    return PR_REQUEST_EXIT;
}

BaseProcessor* LatencyCalculateProcessor::DoClone()
{
    return new LatencyCalculateProcessor(*this);
}

void LatencyCalculateProcessor::DoDestroy()
{
    delete this;
}


AGG_END_NAMESPACE(common)
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(common)::LatencyCalculateProcessor)
