#include <agg_web2/util/hear_detect.h>
#include <ctime>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, HearDetect);
using namespace std;

HearDetect::HearDetect()
{
}

HearDetect::~HearDetect()
{
}

static double GetRatioByLevel(int level) {
    if (level == 1) {
        return 0.75;
    } else if (level == 2) {
        return 1;
    } else if (level == 3) {
        return 1.5;
    } else {
        return 2.;
    }
}

static double CalAvg(const std::vector<double> &vec, size_t maxCnt) {
    if (vec.empty() || maxCnt == 0) {
        return 0.;
    }
    double sum = 0.;
    for (size_t i = 0; i < vec.size() && i < maxCnt; i++) {
        sum += vec[i];
    }
    return sum / (double)std::min(vec.size(), maxCnt);
}

static double CalStd(const std::vector<double> &vec, double mean, size_t maxCnt) {
    if (vec.empty() || maxCnt == 0) {
        return 0.;
    }
    double sum = 0.;
    for (size_t i = 0; i < vec.size() && i < maxCnt; i++) {
        sum += (vec[i] - mean) * (vec[i] - mean);
    }
    return sqrt(sum / (double)std::min(vec.size(), maxCnt));
}


static double CalMid(const std::vector<double> &vec, size_t maxCnt) {
    std::vector<double> vecTmp(vec.begin(), vec.end());
    if (vecTmp.empty() || maxCnt == 0) {
        return 0.;
    }
    if (vecTmp.size() > maxCnt) {
        vecTmp.resize(maxCnt);
    }
    std::sort(vecTmp.begin(), vecTmp.end());
    size_t midIdx = vecTmp.size() / 2;
    double ret = vecTmp[midIdx];
    if (vecTmp.size() % 2 == 0) {
        ret = (ret + vecTmp[midIdx-1]) / 2.;
    }
    return ret;
}

bool HearDetect::step_detect_sub(const vector<double>& vec, int zero_threshold, int un_zero_threshold){
    if( vec.empty()){
        return false;
    }
    if( vec[0] < un_zero_threshold){
        return false;
    }
    vector<double> vecTmp(vec.begin(), vec.begin() + min(14, int(vec.size())));
    int vec_sz = int(vecTmp.size());
    if( vec_sz >= 14){
        int cntZero = 0;
        int cntUnZero = 0;
        for(int i = vec_sz - 1; i >= 0; --i){
            if (vecTmp[i] <= zero_threshold){
                cntZero += 1;
            }else{
                break;
            }
        }
        for(int i = 0; i < vec_sz; ++i){
            if (vecTmp[i] >= un_zero_threshold) {
                cntUnZero += 1;
            }else{
                break;
            }
        }
        if(cntZero >= 10 && cntUnZero > 0){
            if(cntZero + cntUnZero == vec_sz ){
                return true;
            }
            int cntMid = 0;
            bool hasMoreUnZero = cntUnZero > 1;
            for(int i = cntUnZero; i < vec_sz - cntZero; ++i){
                if((vecTmp[i] > zero_threshold || hasMoreUnZero) && vecTmp[i] < un_zero_threshold){
                    cntMid++;
                }else{
                    break;
                }
            }
            if(cntZero + cntUnZero + cntMid == vec_sz ){
                return true;
            }
        }
    }
    return false;
}

bool HearDetect::step_detect(const vector<double>& vecTmp, bool isGood){
    if(isGood){
        return !vecTmp.empty() && ((vecTmp[0] >= 8 && step_detect_sub(vecTmp, int(vecTmp[0] / 2), vecTmp[0])) || step_detect_sub(vecTmp, 1, 5));
    }else{
        return step_detect_sub(vecTmp, 5, 15);
    }
//    return !vecTmp.empty() && vecTmp[0] >= 8 && step_detect_sub(vecTmp, int(vecTmp[0] / 2), vecTmp[0]);
//    return step_detect_sub(vecTmp, 2, 8) ||
}


int HearDetect::DaySignal(const std::vector<double> &vec, size_t begin, int level, bool isGood) {
    double ratio = GetRatioByLevel(level);
    if (vec.size() <= begin) {
        return 0;
    }

    std::vector<double> vecTmp(vec.begin() + begin, vec.begin() + begin + min(int(vec.size()) - int(begin), 30));
    int threshold = 20;
    if (vecTmp[0] < threshold) {
        if(step_detect(vecTmp, isGood)){
            return 1;
        }
        return 0;
    }
    double mean = CalAvg(vecTmp, (size_t)30);
    double std = CalStd(vecTmp, mean, (size_t)30);
    AGG_LOG(DEBUG, "day mean:%f std:%f", mean, std);
    AGG_LOG(DEBUG, "vecTmp[0] :%f", vecTmp[0]);
    if (vecTmp[0] / (mean + 1e-5) > 1.75 * ratio) {
        AGG_LOG(DEBUG, "--1--");
        return 1;
    }
    if (vecTmp.size() >= 2 && (vecTmp[0] + vecTmp[1]) / (mean + 1e-5) > 4. * ratio) {
        AGG_LOG(DEBUG, "--2--");
        return 1;
    }
    if (vecTmp.size() >= 3 && (vecTmp[0] + vecTmp[1] + vecTmp[2]) / (mean + 1e-5) > 7. * ratio) {
        AGG_LOG(DEBUG, "--3--");
        return 1;
    }
    if (vecTmp.size() >= 4 && (vecTmp[0] + vecTmp[1] + vecTmp[2] + vecTmp[3]) / (mean + 1e-5) > 10. * ratio) {
        AGG_LOG(DEBUG, "--4--");
        return 1;
    }
    if (vecTmp.size() >= 5 && (vecTmp[0] + vecTmp[1] + vecTmp[2] + vecTmp[3] + vecTmp[4]) / (mean + 1e-5) > 13. * ratio) {
        AGG_LOG(DEBUG, "--5--");
        return 1;
    }
    if (vecTmp[0] / (vecTmp[vecTmp.size() - 1] + 1e-5) > 7.5 * ratio) {
        AGG_LOG(DEBUG, "--6--");
        return 1;
    }
    if (vecTmp[0] > mean + 3. * std * ratio) {
        AGG_LOG(DEBUG, "--7--");
        return 1;
    }
    if (vecTmp.size() >= 2 && (vecTmp[0] + vecTmp[1]) / 2. > mean + 3. * std * ratio) {
        AGG_LOG(DEBUG, "--8--");
        return 1;
    }
    if (vecTmp.size() >= 7) {
        int cntSmall = 0;
        for (size_t i = 1; i < 7; i++) {
            if (vecTmp[i] <= 5) {
                cntSmall++;
            }
        }
        AGG_LOG(DEBUG, "cnt small:%d vecTmp[0]:%f", cntSmall, vecTmp[0]);
        if (vecTmp[0] >= 25. * ratio && cntSmall >= std::min(6, int(6 * ratio))) {
            AGG_LOG(DEBUG, "--9--");
            return 1;
        }
    }
    if (vecTmp.size() >= 7) {
        int cntZero = 0;
        int cntUnZero = 0;
        for (size_t i = 6; i >= 0; i--) {
            if (vecTmp[i] < 1e-5) {
                cntZero++;
            } else {
                break;
            }
        }
        for (size_t i = 0; i <= 6; i++) {
            if (vecTmp[i] > 15) {
                cntUnZero++;
            } else {
                break;
            }
        }
//        由0阶跃
        if (cntZero + cntUnZero == 7 && cntZero >= 3) {
            AGG_LOG(DEBUG, "--10--");
            return 1;
        }
    }
    return 0;
}

bool HearDetect::GetDaySignal( vector<double> mQueryLogDayUvAggHits, int& daySignal, int& breakDay, int& firstBreakDay, bool isGood){
    int daySignal1 = 0, daySignal2 = 0, daySignal3 = 0;
    breakDay = -1;
    firstBreakDay = -1;
    for (size_t i = 0; i < mQueryLogDayUvAggHits.size(); i++) {
        if (i < (size_t)14) {
            daySignal1 = std::max(DaySignal(mQueryLogDayUvAggHits, i, 1, isGood), daySignal1);
        }
        if (i < (size_t)7) {
            daySignal2 = std::max(DaySignal(mQueryLogDayUvAggHits, i, 2, isGood), daySignal3);
        }
//        近3天
        if (i < (size_t)3) {
            daySignal3 = std::max(DaySignal(mQueryLogDayUvAggHits, i, 3, isGood), daySignal3);
            if(breakDay == -1 && daySignal3){
                breakDay = int(i);
            }
        }else{
            if(breakDay == -1 && DaySignal(mQueryLogDayUvAggHits, i, 3, isGood)){
                breakDay = int(i);
            }
        }
    }
    daySignal = 0;
    if (daySignal3) {
        daySignal = 3;
    } else if (daySignal2) {
        daySignal = 2;
    } else if (daySignal1) {
        daySignal = 1;
    }
    int threshold = 20;
    if (daySignal == 0 && CalMid(mQueryLogDayUvAggHits, 7) < threshold) {
        daySignal = -1;
    }
    if(breakDay >= 0){
        firstBreakDay = breakDay;
        for (size_t i = breakDay + 1; i < mQueryLogDayUvAggHits.size() && firstBreakDay + 4 >= int(i); i++){
            if(DaySignal(mQueryLogDayUvAggHits, i, 3, isGood)){
                firstBreakDay = int(i);
            }
        }
    }
    return breakDay >= 0;
}


AGG_END_NAMESPACE(agg_web2);
