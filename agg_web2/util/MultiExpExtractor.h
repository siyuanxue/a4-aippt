#ifndef A4_AGG_WEB_MULTI_EXP_EXTRACTOR
#define A4_AGG_WEB_MULTI_EXP_EXTRACTOR

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


class MultiExpExtractor
{
public:
    static std::vector<std::string> supersplit(const std::string& s, const std::string& c);
    static bool isSpecialChar(char c);
    static void splitToWordsWordPiece(std::string &sentence, std::vector<std::string> &result, bool removeSpace, bool mergeAscii, const std::map<std::string, int> &word2Id, size_t maxLen=512);
    static std::string toLowerString(std::string str);

    static void splitToWordsWordPiece2(const std::vector<std::string> &words, std::vector<std::string> &result, 
            bool removeSpace, bool mergeAscii, const std::map<std::string, int> &word2Id, size_t maxLen=512);
    static void splitToWordsWordPiece3(const std::vector<std::string> &words, std::vector<std::string> &result, std::vector<uint32_t> &seg_tag, 
            bool removeSpace, bool mergeAscii, const std::map<std::string, int> &word2Id, size_t maxLen=512);

    static void splitToWordsWordPiece_exp(const std::string &exp, std::vector<std::string> &expBertToken, 
       std::vector<uint32_t> & expBertTokenId, std::vector<uint32_t> & expSegId, std::vector<uint32_t> & expExpId, 
       const std::map<std::string, int> &word2Id, size_t maxLen);

    static void parse_exp(const std::string &exp, std::vector<std::string> & and_terms, std::vector<std::string> & rank_terms);

    static void get_words(const std::string &exp, std::vector<std::string> & terms, std::string phrase, size_t start_pos, size_t end_pos);

    static void packTwoVector(std::vector<uint32_t> &merge_vec, const std::vector<uint32_t> &vec1, const std::vector<uint32_t> &vec2);

    static void getExpDat(std::string &exp_dat, const std::string &query, const std::string &exp, 
        const std::vector<uint32_t> &input_ids_new, const std::vector<uint32_t> &segment_ids_new, const std::vector<uint32_t> &exp_type_ids_new);
    static void getDctDat(std::string &dct_dat, const std::string &query, const std::vector<std::string> &seg_vec, 
        const std::vector<uint32_t> &input_ids_vec, const std::vector<uint32_t> &seg_tag_vec);

private:

    AGG_LOG_DECLARE();

};


AGG_END_NAMESPACE(agg_web2);

#endif
