#ifndef A4_AGG_WEB_UTIL_LATENCY_COUNTER
#define A4_AGG_WEB_UTIL_LATENCY_COUNTER

#include <string>
#include <vector>
#include <A4/common/log.h>
#include <A4/common/common.h>
#include <agg2/common/agg_log.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class LatencyCounter
{
public:
    LatencyCounter(const std::string& info);
    LatencyCounter(const char* info);
    ~LatencyCounter();
private:
    std::string mInfo;
    uint64_t mStartTime;
    AGG_LOG_DECLARE();
};

AGG_END_NAMESPACE(agg_web2);
#endif
