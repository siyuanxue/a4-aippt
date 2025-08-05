#include <A4/util/time_util.h>
#include <agg_web2/util/LatencyCounter.h>
AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, LatencyCounter);


LatencyCounter::LatencyCounter(const std::string& info) : mInfo(info)
{
    mStartTime = TimeUtil::GetTimeInUs();
}
LatencyCounter::LatencyCounter(const char* info) : mInfo(info)
{
    mStartTime = TimeUtil::GetTimeInUs();
}
LatencyCounter::~LatencyCounter()
{
    uint64_t spent = TimeUtil::GetTimeInUs() - mStartTime;
    AGG_LOG(DEBUG, "LatencyCounter [%s] spent [%f]ms.", mInfo.c_str(), double(spent)/1000.0f);
}

AGG_END_NAMESPACE(agg_web2);
