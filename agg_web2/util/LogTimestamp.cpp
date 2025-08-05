#include <A4/util/time_util.h>
#include <agg_web2/util/LogTimestamp.h>
#include <agg2/common/bucket_info.h>
#include <unistd.h>
#include <agg_web2/helper/agg_web_context_helper.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, LogTimestamp);

const static std::string KEY_LOG_TIMESTAMP = "key_log_timestamp";

LogTimestamp::LogTimestamp()
{
    mCounter = 0; 
    mId = TimeUtil::GetTimeInUs();
}


LogTimestamp::~LogTimestamp()
{
}

bool LogTimestamp::IsLogTimestampBucket(AggContext * aggContext)
{
    auto* bucketInfo = aggContext->GetBucketInfo();
    std::string bucketName;
    GET_BUCKET_VALUE(bucketInfo, "log_timestamp", bucketName);
    if ("1" == bucketName) {
        return true;
    }
    return false;
}

void LogTimestamp::RegisterLogTimestamp(AggContext * aggContext)
{
    if (false == LogTimestamp::IsLogTimestampBucket(aggContext)) {
        AGG_LOG(DEBUG, "IsLogTimestampBucket false!");
        return;
    }
    AGG_LOG(ERROR, "IsLogTimestampBucket true!");
    LogTimestampPtr logTimestampPtr = std::make_shared<LogTimestamp>();
    aggContext->SetValue(KEY_LOG_TIMESTAMP, logTimestampPtr);

    LogTimestamp::PrintTimestamp(aggContext, "AGG_BEG", "MSG");
}

void LogTimestamp::PrintTimestamp(AggContext * aggContext, const std::string & mark, const std::string & info)
{
    LogTimestampPtr logTimestampPtr = aggContext->GetValue<LogTimestamp>(KEY_LOG_TIMESTAMP);
    if (!logTimestampPtr) {
        AGG_LOG(DEBUG, "logTimestampPtr is null!");
        return;
    }

    uint64_t currTime = TimeUtil::GetTimeInUs();
    size_t counter = logTimestampPtr->mCounter;
    const std::string & hid = AggContextHelper::GetRequestParam(aggContext, REQUEST_HID_KEY);
    //size_t pid = getpid();
    const std::string & query = aggContext->GetAggRequest()->_query;
    bool cacheSwitcher = true;
    cacheSwitcher = (aggContext->GetValue(CACHE_SWITCHER, cacheSwitcher) && !cacheSwitcher); 
    AGG_LOG(ERROR, "[PrintTimestamp]\t%s\t%s\t%lu\t%lu\t%d\t%s\t%s", hid.c_str(), query.c_str(), counter, currTime, int(cacheSwitcher), mark.c_str(), info.c_str());
    logTimestampPtr->mCounter = counter + 1;
}

AGG_END_NAMESPACE(agg_web2);
