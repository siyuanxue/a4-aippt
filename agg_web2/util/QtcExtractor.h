#ifndef A4_AGG_WEB_QTC_EXTRACTOR
#define A4_AGG_WEB_QTC_EXTRACTOR

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


class QtcExtractor
{
public:

    static void extractQtcStdkv( 
        const std::map<std::string, int>& bertMapData,
        const std::string& query, 
        const std::string& title,
        const std::string& contents,
        std::string& reqStr);

    static void extractPrStdkv(
        const std::map<std::string, int>& bertMapData,
        const std::string& query,
        const std::string& title,
        const std::string& paragraph,
        std::string& reqStr);

    static void bertTokenize(
        const std::string & sentence, 
        std::vector<std::string> & tokenResult, 
        std::vector<std::string> & tokenResultId,
        std::vector<std::string> & segmentIds,
        const std::map<std::string, int>& bertWord2Id);

    static void bertTokenize(
        const std::string & sentence, 
        std::vector<std::string> & tokenResult, 
        std::vector<std::string> & tokenResultId,
        std::vector<std::string> & segmentIds,
        std::vector<std::string> & inputMask,
        const std::map<std::string, int>& bertWord2Id);

    static std::string ToLowerCase(const std::string & in);
    static int Find(const std::map<std::string, int>& bertWord2Id, const std::string& subTokenStr);
    static void splitToWordsWordPiece(const std::string &sentence, std::vector<std::string> &result, bool removeSpace, bool mergeAscii, const std::map<std::string, int> &word2Id, size_t maxLen=512 , bool unkRemap=false, bool split_bert=false);
    static bool isSpecialChar(char c);

private:

    AGG_LOG_DECLARE();

};


AGG_END_NAMESPACE(agg_web2);

#endif
