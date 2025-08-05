#ifndef A4_AGG_WEB_QLOG_EXTRACTOR
#define A4_AGG_WEB_QLOG_EXTRACTOR

#include <string>
#include <cstdio>
#include <vector>
#include <map>

#include <A4/common/log.h>
#include <A4/common/common.h>
#include <agg2/common/agg_log.h>
#include <agg2/util/NumToString.h>
#include <messer_utils/util/ostringstream.h>

AGG_BEGIN_NAMESPACE(agg_web2);


class QlogExtractor
{
public:
    static std::vector<std::string> supersplit(const std::string& s, const std::string& c);
    static void string_replace(std::string &strBig, const std::string &strsrc, const std::string &strdst);
    static bool loadBertToken(const std::string bertTokenPath, std::map<std::string, int>& word2Id);
    static bool data2BertToken(std::set<std::string> &lineSet, std::map<std::string, int>& word2Id);
    static bool loadTermWeight(const std::string path, std::map<std::string, float>& termWeigt);
    static bool data2TermWeight(std::set<std::string> &lineSet, std::map<std::string, float>& termWeigt);
    static bool loadStopWord(const std::string path, std::set<std::string> &stopWord);
    static bool data2StopWord(std::set<std::string> &lineSet, std::set<std::string> &stopWord);
    static bool filteNonImp(std::vector<std::string> &words, const std::set<std::string> &stopWord);
    static bool isSpecialChar(char c);
    static void splitToWordsWordPiece(std::string &sentence, std::vector<std::string> &result, bool removeSpace, bool mergeAscii, const std::map<std::string, int> &word2Id, size_t maxLen=512);
    static std::string toLowerString(std::string str);
    static std::vector<size_t> sort_indexes_e(std::vector<float> &v, bool reverse);
    static float bm25_score(std::vector<std::string> &queryWordList, std::map<std::string, int> &queryTermCountMap, int passageLen, std::map<std::string, float> &idf);
    static int subStringCount(std::string &T, std::string &P);
    static void AppendBertId(const std::map<std::string, int> &wordIdInBert,
        const std::vector<std::string>& tokens,
        const std::string& prefix,
        const std::string& postfix,
        int segId,
        size_t idx,
        int maxLen,
        std::vector<int>& ids,
        std::vector<int>& segs); 

    static bool extractQContentNew(std::vector<std::string> &queryWordList,
                       std::string &query,
                       std::string &body,
                       std::string sepInBody,
                       const std::map<std::string, int> &wordIdInBert,
                       const std::map<std::string, float> &termWeigt,
                       std::string &bodySelect,
                       std::vector<std::vector<int>> &qpSelectBertIdVecD,
                       std::vector<std::vector<int>> &qpSelectSegIdVecD,
                       std::vector<std::vector<int>>& qpSelectPositionVecD,
		               std::vector<int>& qpSelectFirstPositionVec,
                       int maxQLen,
                       int maxPassageNum,
                       int maxBertInputLen);
    static bool extractQlogScoreBertId(
            const std::map<std::string, int> &wordIdInBert,
            const std::string &query,
            const std::string &body,
            const std::string &title,
            const std::string &sepInBody,
            std::vector<int> &qtIds,
            std::vector<int> &qtSegs,
            std::vector<std::vector<int>> &qpIds,
            std::vector<std::vector<int>> &qpSegs,
            int maxPassageNum,
            int maxQLen,
            int maxQtLen,
            int maxQpLen);
    static bool extractClickGScoreBertId(
            const std::map<std::string, int> &wordIdInBert,
            const std::string &query,
            const std::string &body,
            const std::string &title,
            const std::string &sepInBody,
            std::vector<int> &qtIds,
            std::vector<int> &qtSegs,
            std::vector<std::vector<int>> &qpIds,
            std::vector<std::vector<int>> &qpSegs,
            int maxPassageNum,
            int maxQLen,
            int maxQtLen,
            int maxQtcLen);
    static bool extractQlogRewriteBertId(
        const std::map<std::string, int> &wordIdInBert,
        const std::string &query,
        std::vector<int> &queryIds,
        std::vector<int> &querySegs,
        int maxQLen);


private:

    AGG_LOG_DECLARE();

};


AGG_END_NAMESPACE(agg_web2);

#endif
