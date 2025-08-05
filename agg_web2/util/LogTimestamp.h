#ifndef A4_AGG_WEB_UTIL_LOG_TIMESTAMP
#define A4_AGG_WEB_UTIL_LOG_TIMESTAMP

#include <string>
#include <vector>
#include <A4/common/log.h>
#include <A4/common/common.h>
#include <agg2/common/agg_log.h>
#include <agg2/common/agg_context.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class LogTimestamp
{
public:
    LogTimestamp();
    ~LogTimestamp();
    static bool IsLogTimestampBucket(AggContext * aggContext);
    static void RegisterLogTimestamp(AggContext * aggContext);
    static void PrintTimestamp(AggContext * aggContext, const std::string & mark, const std::string & info);

private:
    size_t mCounter;
    uint64_t mId;

    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(LogTimestamp);

AGG_END_NAMESPACE(agg_web2);
#endif
