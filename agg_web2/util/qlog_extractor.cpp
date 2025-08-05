#include <agg_web2/util/qlog_extractor.h>

#include <climits>
#include <vector>
#include <fstream>
#include <map>
#include <NlpToolkit/normalizer.h>
#include <A4/util/string_tokenizer.h>
#include <agg2/common/define.h>
#include <agg2/util/string_tool.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, QlogExtractor);

using namespace std;

// Contont-Extracting
std::vector<std::string> QlogExtractor::supersplit(const std::string& s, const std::string& c) {
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

void QlogExtractor::string_replace( std::string &strBig, const std::string &strsrc, const std::string &strdst) {
    std::string::size_type pos = 0;
    std::string::size_type srclen = strsrc.size();
    std::string::size_type dstlen = strdst.size();

    while( (pos=strBig.find(strsrc, pos)) != std::string::npos )
    {
        strBig.replace( pos, srclen, strdst );
        pos += dstlen;
    }
}

bool QlogExtractor::loadBertToken(const std::string bertTokenPath, std::map<std::string, int>& word2Id) {
    std::ifstream myfile(bertTokenPath.c_str());
    std::string s;
    while(getline(myfile,s)) {
        std::vector<std::string> terms = supersplit(s, "\t");
        if (terms.size() == 2) {
            word2Id[terms[0]] = stoi(terms[1]);
        } else {
            printf("[%s]", s.c_str());
            break;
        }
    }
    myfile.close();
    return true;
}

bool QlogExtractor::data2BertToken(std::set<std::string> &lineSet, std::map<std::string, int>& word2Id) {
    for (auto line: lineSet) {
	std::vector<std::string> terms = supersplit(line, "\t");
        if (terms.size() == 2) {
            word2Id[terms[0]] = stoi(terms[1]);
        } else {
            printf("[%s]", line.c_str());
            break;
        }
    }
    return true;
}


bool QlogExtractor::loadTermWeight(const std::string terWeightPath, std::map<std::string, float>& termWeigt) {
    std::ifstream myfile(terWeightPath.c_str());
    std::string s;
    while(getline(myfile,s)) {
        std::vector<std::string> terms = supersplit(s, "\t");
        if (terms.size() == 3) {
            termWeigt[terms[0]] = stof(terms[2]);
        } else {
            printf("[%s]", s.c_str());
            break;
        }
    }
    myfile.close();
    return true;
}

bool QlogExtractor::data2TermWeight(std::set<std::string> &lineSet, std::map<std::string, float>& termWeigt) {
    for (auto line: lineSet) {
	std::vector<std::string> terms = supersplit(line, "\t");
        if (terms.size() == 3) {
            termWeigt[terms[0]] = stof(terms[2]);
        } else {
            printf("[%s]", line.c_str());
            break;
        }
    }
    return true;
}


bool QlogExtractor::loadStopWord(const std::string stopWordPath, std::set<std::string> &stopWord) {
    std::ifstream myfile(stopWordPath.c_str());
    std::string s;
    while(getline(myfile,s)) {
        stopWord.insert(s);
    }
    myfile.close();
    return true;
}

bool QlogExtractor::data2StopWord(std::set<std::string> &lineSet, std::set<std::string> &stopWord) {
    stopWord = lineSet;
    return true;
}


bool QlogExtractor::filteNonImp(std::vector<std::string> &words, const std::set<std::string> &stopWord) {
    std::vector<std::string> buf;
    buf.reserve(words.size());
    for (auto x: words) {
        if (stopWord.find(x) == stopWord.end()) {
            buf.push_back(x);
        }
    }
    words = buf;
    return true;
}

bool QlogExtractor::isSpecialChar(char c) {
    std::string sChars = "`~!@#$%^&*()_+-={}[]|:;<>?,\"./\\";
    std::string::size_type idx = sChars.find(c);
    if(idx == std::string::npos) {
        return false;
    }
    else {
        return true;
    }
}

void QlogExtractor::splitToWordsWordPiece(std::string &sentence, std::vector<std::string> &result, bool removeSpace, bool mergeAscii, const std::map<std::string, int> &word2Id, size_t maxLen) {
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

std::string QlogExtractor::toLowerString(std::string str)
{
    transform(str.begin(), str.end(), str.begin(), (int (*)(int))tolower);
    return str;
}

std::vector<size_t> QlogExtractor::sort_indexes_e(std::vector<float> &v, bool reverse) {
    std::vector<size_t> idx(v.size());
    iota(idx.begin(), idx.end(), 0);
    if (reverse) {
        sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {return v[i1] > v[i2]; });
    }
    else {
        sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {return v[i1] < v[i2]; });
    }
    return idx;
}

float QlogExtractor::bm25_score(vector<string> &queryWordList, map<string, int> &queryTermCountMap, int passageLen, map<string, float> &idf) {
    float bm25Score = 0.0;
    float k1 = 2;
    float b = 0.75;
    float avgpl = 100;
    for (auto x: queryWordList) {
        bm25Score += idf[x] * queryTermCountMap[x] * (k1+1) / (queryTermCountMap[x] + k1*(1-b+b*passageLen/avgpl));
    }
    return bm25Score;
}

int QlogExtractor::subStringCount(string &T, string &P) {
    int count=0;
    int begin=0;
    while((begin=T.find(P,begin))!=(int)(string::npos))
    {
        count++;
//        cout<<begin<<"  "<<count<<endl;
        begin=begin+P.length();
    }
    return count;
}

void QlogExtractor::AppendBertId(const std::map<std::string, int> &wordIdInBert,
        const std::vector<std::string>& tokens,
        const std::string& prefix,
        const std::string& postfix,
        int segId,
        size_t idx,
        int maxLen,
        vector<int>& ids,
        vector<int>& segs) {
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

bool QlogExtractor::extractQlogScoreBertId(
        const std::map<std::string, int> &wordIdInBert,
        const std::string &query,
        const std::string &title,
        const std::string &body,
        const std::string &sepInBody,
        std::vector<int> &qtIds,
        std::vector<int> &qtSegs,
        std::vector<std::vector<int>> &qpIds,
        std::vector<std::vector<int>> &qpSegs,
        int maxPassageNum,
        int maxQLen,
        int maxQtLen,
        int maxQpLen) {

    if (body.empty() && title.empty()) {
        return true;
    }

    // query
    std::vector<std::string> tokens;
    std::vector<int> qIds, qSegIds;
    string q = toLowerString(query);
    splitToWordsWordPiece(q, tokens, true, true, wordIdInBert);
    AppendBertId(wordIdInBert, tokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds);
    
    // title
    qtIds = qIds;
    qtSegs = qSegIds;
    tokens.clear();
    string t = toLowerString(title);
    splitToWordsWordPiece(t, tokens, true, true, wordIdInBert);
    size_t maxLen = maxQtLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtIds, qtSegs);

    // content
    string content = body.substr(0, 50000);
    string_replace(content, " ", "");
    string_replace(content, "　", "");
    string_replace(content, sepInBody+sepInBody+sepInBody, "\n");
    tokens.clear();
    splitToWordsWordPiece(content, tokens, true, true, wordIdInBert, 1024);
    qpIds.resize(maxPassageNum);
    qpSegs.resize(maxPassageNum);
    maxLen = maxQpLen - qIds.size() + 1;
    size_t idx = 0;
    for (int i=0; i<maxPassageNum; ++i) {
        qpIds[i] = qIds;
        qpSegs[i] = qSegIds;
        if (idx + maxLen > tokens.size()) {
            AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1,
                idx, maxLen, qpIds[i], qpSegs[i]);
        } else {
            AppendBertId(wordIdInBert, tokens, "", "", 1,
                idx, maxLen, qpIds[i], qpSegs[i]);
        }
        idx += maxLen;
    }

    return true;
}

bool QlogExtractor::extractClickGScoreBertId(
        const std::map<std::string, int> &wordIdInBert,
        const std::string &query,
        const std::string &title,
        const std::string &body,
        const std::string &sepInBody,
        std::vector<int> &qtIds,
        std::vector<int> &qtSegs,
        std::vector<std::vector<int>> &qpIds,
        std::vector<std::vector<int>> &qpSegs,
        int maxPassageNum,
        int maxQLen,
        int maxQtLen,
        int maxQpLen) {

    if (body.empty() && title.empty()) {
        return true;
    }

    // query
    std::vector<std::string> tokens;
    std::vector<int> qIds, qSegIds;
    string q = toLowerString(query);
    splitToWordsWordPiece(q, tokens, true, true, wordIdInBert);
    AppendBertId(wordIdInBert, tokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds);
    
    // title
    qtIds = qIds;
    qtSegs = qSegIds;
    tokens.clear();
    string t = toLowerString(title);
    splitToWordsWordPiece(t, tokens, true, true, wordIdInBert);
    size_t maxLen = maxQtLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtIds, qtSegs);

    // content
    string content = body.substr(0, 50000);
    string_replace(content, " ", "");
    string_replace(content, "　", "");
    string_replace(content, sepInBody+sepInBody+sepInBody, "\n");
    tokens.clear();
    splitToWordsWordPiece(content, tokens, true, true, wordIdInBert, 1024);
    qpIds.resize(maxPassageNum);
    qpSegs.resize(maxPassageNum);
    maxLen = maxQpLen - qIds.size() + 1;
    size_t idx = 0;
    bool endFlag = false;
    for (int i=0; i<maxPassageNum; ++i) {
        qpIds[i] = qIds;
        qpSegs[i] = qSegIds;
        if  (idx + maxLen > tokens.size()) {
            if (!endFlag) {
                AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1,
                    idx, maxLen, qpIds[i], qpSegs[i]);
            } else {
                AppendBertId(wordIdInBert, tokens, "", "", 1,
                    idx, maxLen, qpIds[i], qpSegs[i]);
            }
            endFlag = true;
        } else {
            AppendBertId(wordIdInBert, tokens, "", "", 1,
                idx, maxLen, qpIds[i], qpSegs[i]);
        }
        idx += maxLen;
    }

    return true;
}

bool QlogExtractor::extractQlogRewriteBertId(
        const std::map<std::string, int> &wordIdInBert,
        const std::string &query,
        std::vector<int> &queryIds,
        std::vector<int> &querySegs,
        int maxQLen) {
    if (query.empty()) {
        return true;
    }

    // query
    std::vector<std::string> tokens;
    string q = toLowerString(query);
    splitToWordsWordPiece(q, tokens, true, true, wordIdInBert);
    AppendBertId(wordIdInBert, tokens, "[CLS]", "[SEP]", 0, 0, maxQLen, queryIds, querySegs);
    
    return true;
}


// end of Contont-Extracting

AGG_END_NAMESPACE(agg_web2);
