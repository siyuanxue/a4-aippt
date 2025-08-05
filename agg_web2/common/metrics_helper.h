#ifndef AGG_WEB_COMMON_METRICS_HELPER_H
#define AGG_WEB_COMMON_METRICS_HELPER_H

#include <agg_web2/common/common.h>
#include <agg2/processor/base_processor.h>

AGG_BEGIN_NAMESPACE(agg_web2);

// latency
static const std::string KEY_LATENCY_PHASE2_TOTAL = "latency_phase2_total";
static const std::string KEY_LATENCY_PHASE3_TOTAL = "latency_phase3_total";
static const std::string KEY_LATENCY_PHASE4_TOTAL = "latency_phase4_total";
static const std::string KEY_LATENCY_PHASE5_TOTAL = "latency_phase5_total";
static const std::string KEY_LATENCY_PHASE6_TOTAL = "latency_phase6_total";

static const std::string KEY_START_TIME_PHASE2 = "start_time_phase2";
static const std::string KEY_START_TIME_PHASE3 = "start_time_phase3";
static const std::string KEY_START_TIME_PHASE4 = "start_time_phase4";
static const std::string KEY_START_TIME_PHASE5 = "start_time_phase5";
static const std::string KEY_START_TIME_PHASE6 = "start_time_phase6";
static const std::string KEY_END_TIME_PHASE6 = "end_time_phase6";

class MetricsHelper
{
public:
    MetricsHelper(){};
    ~MetricsHelper(){};

public:
    // latency
    static bool GetPhase2StartTime(AggContext* aggContext);
    static bool GetPhase3StartTime(AggContext* aggContext);
    static bool GetPhase4StartTime(AggContext* aggContext);
    static bool GetPhase5StartTime(AggContext* aggContext);
    static bool GetPhase6StartTime(AggContext* aggContext);
    static bool GetPhase6EndTime(AggContext* aggContext);
    
    static bool GetLatency(AggContext* aggContext);
    
private:
    static bool GetCurTime(AggContext* aggContext, const std::string& key);

private:
    AGG_LOG_DECLARE();
};

AGG_END_NAMESPACE(agg_web2);
#endif
