#ifndef HEAR_DETECT_UTIL_H
#define HEAR_DETECH_UTIL_H

#include <set>
#include <A4/common/log.h>
#include <A4/common/common.h>
#include <agg2/common/agg_log.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class HearDetect 
{
public:
    HearDetect();
    ~HearDetect();

public:
    static bool step_detect_sub(const std::vector<double>& vec, int zero_threshold, int un_zero_threshold);
    static bool step_detect(const std::vector<double>& vecTmp, bool isGood = false);
    static int DaySignal(const std::vector<double> &vec, size_t begin, int level, bool isGood=false);
    static bool GetDaySignal( std::vector<double> mQueryLogDayUvAggHits, int& daySignal, int& breakDay, int& firstBreakDay, bool isGood=false);
private:
    AGG_LOG_DECLARE();
};

AGG_END_NAMESPACE(agg_web2);

#endif
