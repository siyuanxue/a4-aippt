#ifndef AGG_WEB2_UTIL_STRING_DEDUPER_H
#define AGG_WEB2_UTIL_STRING_DEDUPER_H

#include <string>
#include <cstdio>
#include <vector>
#include <set>
#include <A4/common/log.h>
#include <A4/common/common.h>
#include <agg2/common/agg_log.h>

AGG_BEGIN_NAMESPACE(agg_web2);
using namespace std;

class StringDeduper
{
public:
    static int SplitToWords(const string &word, vector<string> &result, bool removeSpace, bool mergeAscii);
    static std::vector<string> getNgrams(const vector<string>& tokens, size_t n);
    static set<string> transVec2Set(const vector<string>& tokens);
    static float jaccard(const set<string>& seta, const set<string>& setb);

private:
    AGG_LOG_DECLARE();
};

AGG_END_NAMESPACE(agg_web2);

#endif
