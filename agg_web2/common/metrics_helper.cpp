#include <agg_web2/common/metrics_helper.h>
#include <A4/util/time_util.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, MetricsHelper);

bool MetricsHelper::GetLatency(AggContext* aggContext)
{
    Context* context = aggContext->GetContext();
    uint64_t startTime, endTime;
    if (aggContext->GetValue(KEY_START_TIME_PHASE2, startTime) && aggContext->GetValue(KEY_START_TIME_PHASE3, endTime)) {
        context->MutableMetricItem()->IncreaseQps(KEY_LATENCY_PHASE2_TOTAL);
        context->MutableMetricItem()->SetValue(KEY_LATENCY_PHASE2_TOTAL, endTime - startTime);
        AGG_LOG(DEBUG, "Phase2 latency[%ld],[%ld],[%ld]", endTime - startTime, startTime, endTime);
    }

    startTime = 0; endTime = 0;
    if (aggContext->GetValue(KEY_START_TIME_PHASE3, startTime) && aggContext->GetValue(KEY_START_TIME_PHASE4, endTime)) {
        context->MutableMetricItem()->IncreaseQps(KEY_LATENCY_PHASE3_TOTAL);
        context->MutableMetricItem()->SetValue(KEY_LATENCY_PHASE3_TOTAL, endTime - startTime);
        AGG_LOG(DEBUG, "Phase3 latency[%ld],[%ld],[%ld]", endTime - startTime, startTime, endTime);
    }

    startTime = 0; endTime = 0;
    if (aggContext->GetValue(KEY_START_TIME_PHASE4, startTime) && aggContext->GetValue(KEY_START_TIME_PHASE5, endTime)) {
        context->MutableMetricItem()->IncreaseQps(KEY_LATENCY_PHASE4_TOTAL);
        context->MutableMetricItem()->SetValue(KEY_LATENCY_PHASE4_TOTAL, endTime - startTime);
        AGG_LOG(DEBUG, "Phase4 latency[%ld],[%ld],[%ld]", endTime - startTime, startTime, endTime);
    }

    startTime = 0; endTime = 0;
    if (aggContext->GetValue(KEY_START_TIME_PHASE5, startTime) && aggContext->GetValue(KEY_START_TIME_PHASE6, endTime)) {
        context->MutableMetricItem()->IncreaseQps(KEY_LATENCY_PHASE5_TOTAL);
        context->MutableMetricItem()->SetValue(KEY_LATENCY_PHASE5_TOTAL, endTime - startTime);
        AGG_LOG(DEBUG, "Phase5 latency[%ld],[%ld],[%ld]", endTime - startTime, startTime, endTime);
    }

    startTime = 0; endTime = 0;
    if (aggContext->GetValue(KEY_START_TIME_PHASE6, startTime) && aggContext->GetValue(KEY_END_TIME_PHASE6, endTime)) {
        context->MutableMetricItem()->IncreaseQps(KEY_LATENCY_PHASE6_TOTAL);
        context->MutableMetricItem()->SetValue(KEY_LATENCY_PHASE6_TOTAL, endTime - startTime);
        AGG_LOG(DEBUG, "Phase6 latency[%ld],[%ld],[%ld]", endTime - startTime, startTime, endTime);
    }

    return true;
}

bool MetricsHelper::GetPhase2StartTime(AggContext* aggContext)
{
    return GetCurTime(aggContext, KEY_START_TIME_PHASE2);
}

bool MetricsHelper::GetPhase3StartTime(AggContext* aggContext)
{
    return GetCurTime(aggContext, KEY_START_TIME_PHASE3);
}

bool MetricsHelper::GetPhase4StartTime(AggContext* aggContext)
{
    return GetCurTime(aggContext, KEY_START_TIME_PHASE4);
}

bool MetricsHelper::GetPhase5StartTime(AggContext* aggContext)
{
    return GetCurTime(aggContext, KEY_START_TIME_PHASE5);
}

bool MetricsHelper::GetPhase6StartTime(AggContext* aggContext)
{
    return GetCurTime(aggContext, KEY_START_TIME_PHASE6);
}

bool MetricsHelper::GetPhase6EndTime(AggContext* aggContext)
{
    return GetCurTime(aggContext, KEY_END_TIME_PHASE6);
}

bool MetricsHelper::GetCurTime(AggContext* aggContext, const std::string& key)
{
    uint64_t nowTime = TimeUtil::GetTimeInMs();
    aggContext->SetValue(key, nowTime);
    return true;
}

AGG_END_NAMESPACE(agg_web2);

