#ifndef AGG2_PROCESSOR_LATENCY_CALCULATE_PROCESSOR_H
#define AGG2_PROCESSOR_LATENCY_CALCULATE_PROCESSOR_H

#include <agg2/processor/base_processor.h>
#include <agg2/common/agg_context.h>

#include "agg_web2/common/common.h"

AGG_BEGIN_NAMESPACE(common)

/**
 * 处理链的最后一个processor
 */
class LatencyCalculateProcessor : public BaseProcessor
{
public:
    LatencyCalculateProcessor() = default;
    ~LatencyCalculateProcessor() override = default;

public:
    bool DoInit(const String2String& params) override;
    void DoRegisterMetric(AmonitorRegister * monitorRegister) override;
    BaseProcessor* DoClone() override;
    void DoDestroy() override;

public:
    int DoProcess(AggContext* aggContext) override;

private:
    std::string mCacheSuccessedSessionLatencyKey;
    std::string mCacheFailedSessionLatencyKey;
    std::string mSessionLatencyKey;

private:
    static constexpr const char* PARAM_CACHE_SUCCESSED_SESSION_LATENCY_KEY = "session_latency_cache_hit_key";
    static constexpr const char* PARAM_CACHE_FAILED_SESSION_LATENCY_KEY = "session_latency_cache_miss_key";
    static constexpr const char* PARAM_SESSION_LATENCY_KEY = "session_latency_key";

private:
    AGG_LOG_DECLARE();

private:
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(LatencyCalculateProcessor);

AGG_END_NAMESPACE(common)
BOOST_CLASS_EXPORT_KEY(AGG_NS(common)::LatencyCalculateProcessor)

#endif