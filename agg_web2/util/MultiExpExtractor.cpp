#include <agg_web2/util/MultiExpExtractor.h>

#include <climits>
#include <vector>
#include <fstream>
#include <map>
#include <NlpToolkit/normalizer.h>
#include <A4/util/string_tokenizer.h>
#include <agg2/common/define.h>
#include <agg2/util/string_tool.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, MultiExpExtractor);

using namespace std;

template<class T>
void AppendBertId(const std::map<std::string, int> &wordIdInBert,
        const std::vector<std::string>& tokens,
        const std::string& prefix,
        const std::string& postfix,
        int segId,
        size_t idx,
        int maxLen,
        vector<T>& ids,
        vector<T>& segs) {
    if (! prefix.empty()) {
        ids.push_back(wordIdInBert.at(prefix));
        segs.push_back(segId);
    }
    int unk = wordIdInBert.at("[UNK]");
    for (size_t i=idx; i < tokens.size() && i - idx < size_t(maxLen); ++i) {
        auto tokenIter = wordIdInBert.find(tokens[i]);
        if (tokenIter != wordIdInBert.end()) {
            ids.push_back(tokenIter->second);
        } else {
            ids.push_back(unk);
        }
        segs.push_back(segId);
    }
    if (! postfix.empty()) {
        ids.push_back(wordIdInBert.at(postfix));
        segs.push_back(segId);
    }
}

// Contont-Extracting
std::vector<std::string> MultiExpExtractor::supersplit(const std::string& s, const std::string& c) {
    std::vector<std::string> v;
    std::string::size_type pos1, pos2;
    size_t len = s.length();
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2)
    {
        v.emplace_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != len)
        v.emplace_back(s.substr(pos1));

    return v;
}

bool MultiExpExtractor::isSpecialChar(char c) {
    static std::string sChars = "`~!@#$%^&*()_+-={}[]|:;<>?,\"./\\";
    std::string::size_type idx = sChars.find(c);
    if(idx == std::string::npos) {
        return false;
    }
    else {
        return true;
    }
}
string vec2String(const std::vector<std::string> &vec, const std::string &sep)
{
    std::string res;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0)
            res += sep;
        res += vec[i];
    }
    return res;
}
string idVec2String(const std::vector<uint32_t> &vec, const std::string &sep)
{
    std::string res;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0)
            res += sep;
        res += std::to_string(vec[i]);
    }
    return res;
}
// seg words to bert str
void MultiExpExtractor::splitToWordsWordPiece2(const std::vector<std::string> &words, std::vector<std::string> &result, 
        bool removeSpace, bool mergeAscii,const std::map<std::string, int> &word2Id, size_t maxLen)
{
    result.clear();
    std::string sentence = vec2String(words, "");
    if (sentence.empty())
        return;
    // tag each words start pos
    std::vector<bool> segTagVec;
    for (size_t i = 0, lenth = 0; i < words.size(); i++){
        segTagVec.push_back(true);
        for (size_t j = lenth+1; j < lenth+words.at(i).size(); j++){
            segTagVec.push_back(false);
        }
        lenth += words.at(i).size();
    }
    if (segTagVec.size() != sentence.size()){
        return;
    }

    int sentenceSize = sentence.size();
    for (int i = 0; i < sentenceSize;)
    {
        if ((sentence[i] & 240) == 224)
        {
            if (i + 3 <= sentenceSize)
            {
                result.push_back(string(&sentence[i], 3));
            }
            i += 3;
        }
        else if ((sentence[i] & 224) == 192)
        {
            if (i + 2 <= sentenceSize)
            {
                result.push_back(string(&sentence[i], 2));
            }
            i += 2;
        }
        else if ((sentence[i] & 128) == 0)
        {
            if (removeSpace && isspace(sentence[i]))
            {
                ++i;
                continue;
            }

            int j = i + 1;
            if (mergeAscii && !isspace(sentence[i]) && !isSpecialChar(sentence[i]))
            {
                while ((j < sentenceSize)
                       && ((sentence[j] & 128) == 0)
                       && !isspace(sentence[j])
                       && !isSpecialChar(sentence[j]))
                {
                    ++j;
                }
            }
            std::string tokenStr = string(&sentence[i], j - i);
            bool isBad = false;
            size_t tokenStart = 0;
            while (tokenStart < tokenStr.size())
            {
                //size_t tokenEnd = tokenStr.size();
                // incase of conflict with seg
                size_t tokenEnd = tokenStart+1;
                while (tokenEnd < tokenStr.size() && segTagVec[i+tokenEnd]==false)
                {
                    tokenEnd++;
                }
                std::string curSubTokenStr = "";
                while (tokenStart < tokenEnd)
                {
                    std::string subTokenStr = string(&tokenStr[tokenStart], tokenEnd - tokenStart);
                    if (tokenStart > 0)
                    {
                        subTokenStr = "##" + subTokenStr;
                    }
                    if (word2Id.find(subTokenStr) != word2Id.end())
                    {
                        curSubTokenStr = subTokenStr;
                        break;
                    }
                    --tokenEnd;
                }
                if (curSubTokenStr.empty())
                {
                    isBad = true;
                    break;
                }
                result.push_back(curSubTokenStr);
                tokenStart = tokenEnd;
            }
            if (isBad)
            {
                result.push_back("[UNK]");
            }
//            result.push_back(tokenStr);
            i = j;
        }
        else
        {
            ++i;
            continue;
        }
    }
    // max
}
// seg words to bert str, add bme tag
void MultiExpExtractor::splitToWordsWordPiece3(const std::vector<std::string> &words, std::vector<std::string> &result, 
        std::vector<uint32_t> &seg_tag, bool removeSpace, bool mergeAscii,const std::map<std::string, int> &word2Id, size_t maxLen)
{
    result.clear();
    std::string sentence = vec2String(words, "");
    if (sentence.empty())
        return;
    // tag each words start pos
    std::vector<bool> segTagVec;
    for (size_t i = 0, lenth = 0; i < words.size(); i++){
        segTagVec.push_back(true);
        for (size_t j = lenth+1; j < lenth+words.at(i).size(); j++){
            segTagVec.push_back(false);
        }
        lenth += words.at(i).size();
    }
    if (segTagVec.size() != sentence.size()){
        return;
    }

    int sentenceSize = sentence.size();
    for (int i = 0; i < sentenceSize;)
    {
        if ((sentence[i] & 240) == 224)
        {
            if (i + 3 <= sentenceSize)
            {
                result.push_back(string(&sentence[i], 3));
            }
            i += 3;
        }
        else if ((sentence[i] & 224) == 192)
        {
            if (i + 2 <= sentenceSize)
            {
                result.push_back(string(&sentence[i], 2));
            }
            i += 2;
        }
        else if ((sentence[i] & 128) == 0)
        {
            if (removeSpace && isspace(sentence[i]))
            {
                ++i;
                continue;
            }

            int j = i + 1;
            if (mergeAscii && !isspace(sentence[i]) && !isSpecialChar(sentence[i]))
            {
                while ((j < sentenceSize)
                       && ((sentence[j] & 128) == 0)
                       && !isspace(sentence[j])
                       && !isSpecialChar(sentence[j]))
                {
                    ++j;
                }
            }
            std::string tokenStr = string(&sentence[i], j - i);
            bool isBad = false;
            size_t tokenStart = 0;
            while (tokenStart < tokenStr.size())
            {
                size_t tokenEnd = tokenStart+1;
                while (tokenEnd < tokenStr.size() && segTagVec[i+tokenEnd]==false)
                {
                    tokenEnd++;
                }
                std::string curSubTokenStr = "";
                while (tokenStart < tokenEnd)
                {
                    std::string subTokenStr = string(&tokenStr[tokenStart], tokenEnd - tokenStart);
                    if (tokenStart > 0)
                    {
                        subTokenStr = "##" + subTokenStr;
                    }
                    if (word2Id.find(subTokenStr) != word2Id.end())
                    {
                        curSubTokenStr = subTokenStr;
                        break;
                    }
                    --tokenEnd;
                }
                if (curSubTokenStr.empty())
                {
                    isBad = true;
                    break;
                }
                result.push_back(curSubTokenStr);
                tokenStart = tokenEnd;
            }
            if (isBad)
            {
                result.push_back("[UNK]");
            }
            i = j;
        }
        else
        {
            ++i;
            continue;
        }
    }
    // segTagVec, sentence, result, seg_tag
    for (size_t idx = 0, start = 0; idx < result.size(); idx++) {
        std::string token = result[idx];
        if (token.size() > 2 && token[0]=='#' && token[1]=='#')
            token = token.substr(2);
        size_t pos = sentence.find(token, start);
        if (pos == std::string::npos || pos != start) {
            break;
        }
        size_t end = pos+token.size();
        if (segTagVec[start]) {
            if (end >= segTagVec.size())
                seg_tag.push_back(0);
            else if (segTagVec[end])
                seg_tag.push_back(0);
            else
                seg_tag.push_back(1);
        }
        else {
            if (end >= segTagVec.size())
                seg_tag.push_back(3);
            else if (segTagVec[end])
                seg_tag.push_back(3);
            else
                seg_tag.push_back(2);
        }
        start = end;
    }
}
void MultiExpExtractor::get_words(const std::string &exp, std::vector<std::string> & terms, std::string phrase, size_t start_pos, size_t end_pos)
{
    size_t start = start_pos;
    size_t phrase_size = phrase.size();
    while (start != std::string::npos && start < end_pos){
        size_t left = exp.find(phrase, start);
        if (left == std::string::npos || left >= end_pos)
            break;
        size_t right = exp.find("'", left+phrase_size);
        if (right == std::string::npos || right >= end_pos)
            break;
        std::string word = exp.substr(left+phrase_size, right-left-phrase_size);
        terms.push_back(word);
        start = right;
    }
}
void MultiExpExtractor::parse_exp(const std::string &exp, std::vector<std::string> & and_terms, std::vector<std::string> & rank_terms)
{
    size_t sep_idx = exp.find(" RANK phrase");
    if (sep_idx == std::string::npos)
        sep_idx = exp.size();
    std::string phrase = "phrase:'";
    if (exp.find("phrase_at:'") != std::string::npos)
        phrase = "phrase_at:'";
    get_words(exp, and_terms, phrase, 0, sep_idx);
    get_words(exp, rank_terms, phrase, sep_idx, exp.size());
}
void MultiExpExtractor::splitToWordsWordPiece_exp(const std::string &exp, std::vector<std::string> &expBertToken, 
       std::vector<uint32_t> & expBertTokenId, std::vector<uint32_t> & expSegId, std::vector<uint32_t> & expExpId, 
       const std::map<std::string, int> &word2Id, size_t maxLen)
{
    //get and terms, rank terms
    std::vector<std::string> and_terms, rank_terms;
    parse_exp(exp, and_terms, rank_terms);

    std::vector<std::string> andBertToken, rankBertToken;
    std::vector<uint32_t> andBertTokenId, rankBertTokenId;
    splitToWordsWordPiece2(and_terms, andBertToken, true, true, word2Id, maxLen);
    splitToWordsWordPiece2(rank_terms, rankBertToken, true, true, word2Id, maxLen);

    expBertToken.insert(expBertToken.end(), andBertToken.begin(), andBertToken.end());
    expBertToken.insert(expBertToken.end(), rankBertToken.begin(), rankBertToken.end());
    AppendBertId(word2Id, expBertToken, "", "", 1, 0, (int)maxLen, expBertTokenId, expSegId);
    
    for (size_t i=0; i<andBertToken.size() && i<maxLen; i++)
        expExpId.push_back(2);
    for (size_t i=0; i<rankBertToken.size() && i+andBertToken.size()<maxLen; i++)
        expExpId.push_back(3);
}
void MultiExpExtractor::packTwoVector(std::vector<uint32_t> &merge_vec, const std::vector<uint32_t> &vec1, const std::vector<uint32_t> &vec2)
{
    merge_vec.clear();
    merge_vec.insert(merge_vec.end(), vec1.begin(), vec1.end());
    merge_vec.insert(merge_vec.end(), vec2.begin(), vec2.end());
}
void MultiExpExtractor::getExpDat(std::string &exp_dat, const std::string &query, const std::string &exp, 
        const std::vector<uint32_t> &input_ids_new, const std::vector<uint32_t> &segment_ids_new, const std::vector<uint32_t> &exp_type_ids_new)
{
    exp_dat.append("[dat]\n");
    exp_dat.append("query=0:"+query+"\n");
    exp_dat.append("exp0=0:"+exp+"\n");
    std::string input_ids_new0 = idVec2String(input_ids_new, ",");
    std::string segment_ids_new0 = idVec2String(segment_ids_new, ",");
    std::string exp_type_ids_new0 = idVec2String(exp_type_ids_new, ",");
    exp_dat.append("input_ids_new0=0:"+input_ids_new0+"\n");
    exp_dat.append("segment_ids_new0=0:"+segment_ids_new0+"\n");
    exp_dat.append("exp_type_ids_new0=0:"+exp_type_ids_new0+"\n");
}

void MultiExpExtractor::getDctDat(std::string &dct_dat, const std::string &query, const std::vector<std::string> &seg_vec, 
        const std::vector<uint32_t> &input_ids_vec, const std::vector<uint32_t> &seg_tag_vec)
{
    dct_dat.append("[dat]\n");
    dct_dat.append("query=0:"+query+"\n");
    std::string seg_query;
    if (seg_vec.size() > 0)
        seg_query = seg_vec[0];
    for (size_t idx=1; idx < seg_vec.size(); idx++)
        seg_query += ","+seg_vec[idx];
    dct_dat.append("seg_query=0:"+seg_query+"\n");
    std::string wordnum;
    SAFE_TYPE_CHANGE(seg_vec.size(), wordnum);
    dct_dat.append("wordnum=0:"+wordnum+"\n");
    std::string charlen;
    SAFE_TYPE_CHANGE(input_ids_vec.size()-1, charlen);
    dct_dat.append("charlen=0:"+charlen+"\n");
    std::string seg_seq = idVec2String(seg_tag_vec, ",");
    std::string input_ids = idVec2String(input_ids_vec, ",");
    dct_dat.append("seg_seq=0:"+seg_seq+"\n");
    dct_dat.append("input_ids=0:"+input_ids+"\n");
}

void MultiExpExtractor::splitToWordsWordPiece(std::string &sentence, std::vector<std::string> &result, bool removeSpace, bool mergeAscii, const std::map<std::string, int> &word2Id, size_t maxLen) {
    result.clear();
    if (sentence.empty())
    {
        return;
    }

    if (sentence.size() > maxLen) {
        sentence = sentence.substr(0, maxLen);
    }

    int sentenceSize = sentence.size();
    for (int i = 0; i < sentenceSize;)
    {
        if ((sentence[i] & 240) == 224)
        {
            if (i + 3 <= sentenceSize)
            {
                result.push_back(string(&sentence[i], 3));
            }
            i += 3;
        }
        else if ((sentence[i] & 224) == 192)
        {
            if (i + 2 <= sentenceSize)
            {
                result.push_back(string(&sentence[i], 2));
            }
            i += 2;
        }
        else if ((sentence[i] & 128) == 0)
        {
            if (removeSpace && isspace(sentence[i]))
            {
                ++i;
                continue;
            }

            int j = i + 1;
            if (mergeAscii && !isspace(sentence[i]) && !isSpecialChar(sentence[i]))
            {
                while ((j < sentenceSize)
                       && ((sentence[j] & 128) == 0)
                       && !isspace(sentence[j])
                       && !isSpecialChar(sentence[j]))
                {
                    ++j;
                }
            }
            std::string tokenStr = string(&sentence[i], j - i);
            bool isBad = false;
            size_t tokenStart = 0;
            while (tokenStart < tokenStr.size())
            {
                size_t tokenEnd = tokenStr.size();
                std::string curSubTokenStr = "";
                while (tokenStart < tokenEnd)
                {
                    std::string subTokenStr = string(&tokenStr[tokenStart], tokenEnd - tokenStart);
                    if (tokenStart > 0)
                    {
                        subTokenStr = "##" + subTokenStr;
                    }
                    if (word2Id.find(subTokenStr) != word2Id.end())
                    {
                        curSubTokenStr = subTokenStr;
                        break;
                    }
                    --tokenEnd;
                }
                if (curSubTokenStr.empty())
                {
                    isBad = true;
                    break;
                }
                result.push_back(curSubTokenStr);
                tokenStart = tokenEnd;
            }
            if (isBad)
            {
                result.push_back("[UNK]");
            }
//            result.push_back(tokenStr);
            i = j;
        }
        else
        {
            ++i;
            continue;
        }
    }
}

std::string MultiExpExtractor::toLowerString(std::string str)
{
    transform(str.begin(), str.end(), str.begin(), (int (*)(int))tolower);
    return str;
}

// end of Contont-Extracting

AGG_END_NAMESPACE(agg_web2);
