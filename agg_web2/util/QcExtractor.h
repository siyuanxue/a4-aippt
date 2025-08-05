#ifndef A4_AGG_WEB_QC_EXTRACTOR
#define A4_AGG_WEB_QC_EXTRACTOR

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


class QcExtractor
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
    static void splitToWordsWordPiece(const std::string &sentence, std::vector<std::string> &result, bool removeSpace, bool mergeAscii, const std::map<std::string, int> &word2Id, size_t maxLen=512 , bool unkRemap=false, bool split_bert=false);
    static std::string toLowerString(std::string str);
    static std::vector<size_t> sort_indexes_e(std::vector<float> &v, bool reverse);
    static float bm25_score(std::vector<std::string> &queryWordList, std::map<std::string, int> &queryTermCountMap, int passageLen, std::map<std::string, float> &idf);
    static int subStringCount(std::string &T, std::string &P);
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

    static bool extractQContentQTQ4P(
            const std::map<std::string, int> &wordIdInBert,
            const std::vector<std::string> & queryTokens,
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
            int maxQpLen,
            bool qtcStudentBugfixFlag);


    static bool extractQContentQTQ4P(
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
            int maxQpLen,
            bool qtcStudentBugfixFlag);

    static bool extractQueryQtcNn(
        const std::map<std::string, int> &wordIdInBert,
        const std::string &query,
        std::string &query_dat,
        int maxQLen);

	static bool extractQContentQTQC(
            const std::map<std::string, int> &wordIdInBert,
            const std::string &query,
            const std::string &body,
            const std::string &title,
            const std::string &sepInBody,
            std::vector<int> &queryIds,
            std::vector<int> &querySegIds,
            std::vector<int> &qtIds,
            std::vector<int> &qtSegs,
            std::vector<int> &qcIds,
            std::vector<int> &qcSegs,    
            int maxQLen,
            int maxQtLen,
            int maxQcLen);
    
    static bool extractQContentQTQCQTC(
            const std::map<std::string, int> &wordIdInBert,
            const std::string &query,
            const std::string &body,
            const std::string &title,
            const std::string &sepInBody,
            std::vector<int> &queryIds,
            std::vector<int> &querySegIds,
            std::vector<int> &qtIds,
            std::vector<int> &qtSegs,
            std::vector<int> &qcIds,
            std::vector<int> &qcSegs,
            std::vector<int> &qtcIds,
            std::vector<int> &qtcSegs,         
            int maxQLen,
            int maxQtLen,
            int maxQcLen);

    static bool extractQueryL2NN(
        const std::map<std::string, int> &wordIdInBert,
        const std::string &query,
        std::string &query_dat,
        int maxQLen);

    static void extractQueryToken(
        const std::map<std::string, int> &wordIdInBert,
        const std::string &query,
        std::vector<std::string>& tokens,
        bool unkRemap=false);

    static bool extractQContentQTQCQTC(
            const std::map<std::string, int> &wordIdInBert,
            const std::vector<std::string> &queryTokens,
            std::string &body,
            const std::string &title,
            const std::string &sepInBody,
            std::vector<int> &queryIds,
            std::vector<int> &querySegIds,
            std::vector<int> &qtIds,
            std::vector<int> &qtSegs,
            std::vector<int> &qcIds,
            std::vector<int> &qcSegs,
            std::vector<int> &qtcIds,
            std::vector<int> &qtcSegs,         
            int maxQLen,
            int maxQtLen,
            int maxQcLen);
    static bool extractQContentQTlistQTClist(
            const std::map<std::string, int> &wordIdInBert,
            const std::vector<std::string>& queryTokens,
            std::string &body,
            const std::string &title,
            const std::string &sepInBody,
            std::vector<int> &queryIds,
            std::vector<int> &querySegIds,
            std::vector<int> &qtlistIds,
            std::vector<int> &qtlistSegs,
            std::vector<int> &qtlistMask,
            std::vector<int> &qcIds,
            std::vector<int> &qcSegs,
            std::vector<int> &qtclistIds,
            std::vector<int> &qtclistSegs,
            std::vector<int> &qtclistMask,
            int maxQLen,
            int maxQtLen,
            int maxQcLen,
            int maxtlistLen,
            std::vector<std::string> tlist_top3,
            bool split_bert);

    static bool extractQContentQTlistQTClistUNK(
            const std::map<std::string, int> &wordIdInBert,
            const std::vector<std::string>& queryTokens,
            std::string &body,
            const std::string &title,
            const std::string &sepInBody,
            std::vector<int> &queryIds,
            std::vector<int> &querySegIds,
            std::vector<int> &qtlistIds,
            std::vector<int> &qtlistSegs,
            std::vector<int> &qtlistMask,
            std::vector<int> &qcIds,
            std::vector<int> &qcSegs,
            std::vector<int> &qtclistIds,
            std::vector<int> &qtclistSegs,
            std::vector<int> &qtclistMask,
            int maxQLen,
            int maxQtLen,
            int maxQcLen,
            int maxtlistLen,
            std::vector<std::string> tlist_top3,
            bool split_bert);

    static bool extractQContentQTQCQTCUNK(
            const std::map<std::string, int> &wordIdInBert,
            const std::vector<std::string> &queryTokens,
            std::string &body,
            const std::string &title,
            const std::string &sepInBody,
            std::vector<int> &queryIds,
            std::vector<int> &querySegIds,
            std::vector<int> &qtIds,
            std::vector<int> &qtSegs,
            std::vector<int> &qcIds,
            std::vector<int> &qcSegs,
            std::vector<int> &qtcIds,
            std::vector<int> &qtcSegs,         
            int maxQLen,
            int maxQtLen,
            int maxQcLen);

    static bool extractQContentQTQC(
            const std::map<std::string, int> &wordIdInBert,
            const std::vector<std::string> &queryTokens,
            std::string &body,
            const std::string &title,
            const std::string &sepInBody,
            std::vector<int> &queryIds,
            std::vector<int> &querySegIds,
            std::vector<int> &qtIds,
            std::vector<int> &qtSegs,
            std::vector<int> &qcIds,
            std::vector<int> &qcSegs,    
            int maxQLen,
            int maxQtLen,
            int maxQcLen);
     
    static bool IsContainAlphaString(const std::string& str);
    static bool IsAlpha(const char& ch);


private:

    AGG_LOG_DECLARE();

};


AGG_END_NAMESPACE(agg_web2);

#endif
