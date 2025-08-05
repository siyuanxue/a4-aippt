#include <agg_web2/util/QcExtractor.h>

#include <climits>
#include <vector>
#include <fstream>
#include <map>
#include <NlpToolkit/normalizer.h>
#include <A4/util/string_tokenizer.h>
#include <agg2/common/define.h>
#include <agg2/util/string_tool.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, QcExtractor);

using namespace std;


// Contont-Extracting
std::vector<std::string> QcExtractor::supersplit(const std::string& s, const std::string& c) {
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

void QcExtractor::string_replace( std::string &strBig, const std::string &strsrc, const std::string &strdst) {
    std::string::size_type pos = 0;
    std::string::size_type srclen = strsrc.size();
    std::string::size_type dstlen = strdst.size();

    while( (pos=strBig.find(strsrc, pos)) != std::string::npos )
    {
        strBig.replace( pos, srclen, strdst );
        pos += dstlen;
    }
}

bool QcExtractor::loadBertToken(const std::string bertTokenPath, std::map<std::string, int>& word2Id) {
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

bool QcExtractor::data2BertToken(std::set<std::string> &lineSet, std::map<std::string, int>& word2Id) {
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


bool QcExtractor::loadTermWeight(const std::string terWeightPath, std::map<std::string, float>& termWeigt) {
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

bool QcExtractor::data2TermWeight(std::set<std::string> &lineSet, std::map<std::string, float>& termWeigt) {
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


bool QcExtractor::loadStopWord(const std::string stopWordPath, std::set<std::string> &stopWord) {
    std::ifstream myfile(stopWordPath.c_str());
    std::string s;
    while(getline(myfile,s)) {
        stopWord.insert(s);
    }
    myfile.close();
    return true;
}

bool QcExtractor::data2StopWord(std::set<std::string> &lineSet, std::set<std::string> &stopWord) {
    stopWord = lineSet;
    return true;
}


bool QcExtractor::filteNonImp(std::vector<std::string> &words, const std::set<std::string> &stopWord) {
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

bool QcExtractor::isSpecialChar(char c) {
    std::string sChars = "`~!@#$%^&*()_+-={}[]|:;<>?,\"./\\";
    std::string::size_type idx = sChars.find(c);
    if(idx == std::string::npos) {
        return false;
    }
    else {
        return true;
    }
}

void QcExtractor::splitToWordsWordPiece(const std::string &sentence, std::vector<std::string> &result, bool removeSpace, bool mergeAscii, const std::map<std::string, int> &word2Id, size_t maxLen,bool unkRemap, bool split_bert) {
    result.clear();
    if (sentence.empty())
    {
        return;
    }

    size_t sentenceSize = std::min(sentence.size(), maxLen);
    for (size_t i = 0; i < sentenceSize;)
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

            size_t j = i + 1;
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
            if (tokenStr.size() > 50 && split_bert) {
                i = j;
                continue;
            }
            while (tokenStart < tokenStr.size())
            {
                size_t tokenEnd = tokenStr.size();
                std::string curSubTokenStr;
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
                if(unkRemap){

                    result.push_back(tokenStr);

                }else{
                    
                    result.push_back("[UNK]");

                }
                
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

std::string QcExtractor::toLowerString(std::string str)
{
    transform(str.begin(), str.end(), str.begin(), (int (*)(int))tolower);
    return str;
}

std::vector<size_t> QcExtractor::sort_indexes_e(std::vector<float> &v, bool reverse) {
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

float QcExtractor::bm25_score(vector<string> &queryWordList, map<string, int> &queryTermCountMap, int passageLen, map<string, float> &idf) {
    float bm25Score = 0.0;
    float k1 = 2;
    float b = 0.75;
    float avgpl = 100;
    for (auto x: queryWordList) {
        bm25Score += idf[x] * queryTermCountMap[x] * (k1+1) / (queryTermCountMap[x] + k1*(1-b+b*passageLen/avgpl));
    }
    return bm25Score;
}

int QcExtractor::subStringCount(string &T, string &P) {
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

bool QcExtractor::extractQContentNew(std::vector<std::string> &queryWordList, std::string &query, std::string &body, std::string sepInBody, const std::map<std::string, int> &wordIdInBert, const std::map<std::string, float> &termWeigt, std::string &bodySelect, std::vector<std::vector<int>> &qpSelectBertIdVecD, std::vector<std::vector<int>> &qpSelectSegIdVecD, vector<vector<int>>& qpSelectPositionVecD, std::vector<int>& qpSelectFirstPositionVec, int maxQLen, int maxPassageNum, int maxBertInputLen) {

    if (body.size() == 0) {
        return true;
    }
    if (body.size() > 200000) {
        body = body.substr(0, 200000);
    }


    query = toLowerString(query);
    std::vector<std::string> queryBertToken;
    splitToWordsWordPiece(query, queryBertToken, true, true, wordIdInBert);
    if ((int)(queryBertToken.size()) > maxQLen) {
        std::vector<std::string> buf(queryBertToken.begin(), queryBertToken.begin()+maxQLen);
        queryBertToken = buf;
    }

//    int maxSeqLen = min(maxCLen, (int)(maxBertInputLen-queryBertToken.size()-4));
    int maxPassageLen = maxBertInputLen - queryBertToken.size() - 3;

    string_replace(body, " ", "");
    string_replace(body, "　", "");
    string_replace(body, sepInBody+sepInBody+sepInBody, "\n");
//    cout << body << endl;

    string synbols[] = {",", "?", "!", "，", "！", "？", "。", "\n", ";", "；"};
    vector<string> senList;
    senList.push_back(body);
    for (auto synbol: synbols) {
        vector<string> tmpInfo;
        for (auto sen: senList) {
            vector<string> field = supersplit(sen, synbol);
            tmpInfo.insert(tmpInfo.end(), field.begin(), field.end());
        }
        senList = tmpInfo;
    }

    vector<vector<string>> passageInBertTokenVec;
    vector<int> passageStartPositionVec;
    vector<string> passageInBertTokenBuf;
    int recentPosition = 0;
    for (auto sen: senList) {
        string_replace(sen, sepInBody, "");
//        cout << "Debug1: " << sen << endl;
        sen = toLowerString(sen);
        vector<string> tokenResult;
        splitToWordsWordPiece(sen, tokenResult, true, true, wordIdInBert);

        if (tokenResult.empty()) {
            continue;
        }

        if ((int)(tokenResult.size()) > maxPassageLen) {
            tokenResult.resize(maxPassageLen);
        }

        if ((int)(passageInBertTokenBuf.size() + tokenResult.size()) > maxPassageLen) {
            passageInBertTokenVec.push_back(passageInBertTokenBuf);
            passageStartPositionVec.push_back(recentPosition);
            recentPosition += passageInBertTokenBuf.size();
            passageInBertTokenBuf = tokenResult;
        }
        else {
            passageInBertTokenBuf.insert(passageInBertTokenBuf.end(), tokenResult.begin(), tokenResult.end());
        }
    }
    if (passageInBertTokenBuf.size() > 0) {
        passageInBertTokenVec.push_back(passageInBertTokenBuf);
        passageStartPositionVec.push_back(recentPosition);
    }

    if (passageInBertTokenVec.size() == 0) {
        return true;
    }

    // start of selectSentence

    int queryWordNum = queryWordList.size();
    if (queryWordNum == 0) {
        return true;
    }

    map<string, float> queryTermWeight;
    for (auto x: queryWordList) {
        auto iter = termWeigt.find(x);
        if (iter != termWeigt.end()) {
            queryTermWeight[x] = termWeigt.at(x);
        }
        else {
            queryTermWeight[x] = 10.0;
        }
    }

    vector<float> passageScoreVec;
    map<string, int> queryTermCountMap;
    int maxSenNum = 10000;
    float score = 0.0;
    for (int i = 0 ; i < (int)(passageInBertTokenVec.size()); i++) {
        if (i >= maxSenNum) {
            break;
        }
        if (passageInBertTokenVec[i].size() < 5) {
            passageScoreVec.push_back(-i);
            continue;
        }

        score = 0.0;
        string sen;
        for (auto x: passageInBertTokenVec[i]) {
            sen += x;
        }
        for (auto x: queryWordList) {
            queryTermCountMap[x] = subStringCount(sen, x);
        }
        score = bm25_score(queryWordList, queryTermCountMap, passageInBertTokenVec[i].size(), queryTermWeight) * maxSenNum;
        score += (maxSenNum - i);
        passageScoreVec.push_back(score);
    }

    vector<bool> selectFlag(passageInBertTokenVec.size(), false);
    vector<size_t> idxSorted = sort_indexes_e(passageScoreVec, true);

    int passageSelectNum = 0;
    selectFlag[0] = true;
    passageSelectNum += 1;
    for (auto x: idxSorted) {
        if (passageSelectNum >= maxPassageNum) {
            break;
        }
        if (!selectFlag[x]) {
            selectFlag[x] = true;
            passageSelectNum += 1;
        }
    }



    std::vector<int> queryBertId, querySegId, queryPosition;
    queryBertId.push_back(wordIdInBert.at("[CLS]"));
    for (auto s: queryBertToken) {
        auto tokenIter = wordIdInBert.find(s);
        if (tokenIter != wordIdInBert.end()) {
            queryBertId.push_back(wordIdInBert.at(s));
        }
        else {
            queryBertId.push_back(wordIdInBert.at("[UNK]"));
        }
    }
    queryBertId.push_back(wordIdInBert.at("[SEP]"));
    for (int i = 0; i < (int)(queryBertToken.size())+2; i++) {
        queryPosition.push_back(i);
        querySegId.push_back(0);
    }
//    vector<string> selectBertToken;
    for (size_t i = 0; i < passageInBertTokenVec.size(); i++) {
        if (selectFlag[i]) {
            vector<int> passageBertIdVec;
            vector<int> passagePositionVec;
            vector<int> passageSegIdVec;
            if (bodySelect.length() != 0) {
                bodySelect += "||||";
            }
            for (auto s: passageInBertTokenVec[i]) {
                bodySelect += s;
                auto tokenIter = wordIdInBert.find(s);
                if (tokenIter != wordIdInBert.end()) {
                    passageBertIdVec.push_back(wordIdInBert.at(s));
                }
                else {
                    passageBertIdVec.push_back(wordIdInBert.at("[UNK]"));
                }
            }
            passageBertIdVec.push_back(wordIdInBert.at("[SEP]"));
            int positionStart = passageStartPositionVec[i] + queryBertToken.size()+2;
//            for (int j = 0; j < (int)(passageInBertTokenVec[i].size())+1; j++) {
//                passagePositionVec.push_back(positionStart+j);
//                passageSegIdVec.push_back(1);
//            }
            passageBertIdVec.insert(passageBertIdVec.begin(), queryBertId.begin(), queryBertId.end());
            qpSelectBertIdVecD.push_back(passageBertIdVec);
//            passagePositionVec.insert(passagePositionVec.begin(), queryPosition.begin(), queryPosition.end());
//            qpSelectPositionVecD.push_back(passagePositionVec);
//            passageSegIdVec.insert(passageSegIdVec.begin(), querySegId.begin(), querySegId.end());
//            qpSelectSegIdVecD.push_back(passageSegIdVec);
	    qpSelectFirstPositionVec.push_back(positionStart);
        }
    }

    return true;
}

void AppendBertId(const std::map<std::string, int> &wordIdInBert,
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
void AppendQtclistMaskId(const std::map<std::string, int> &wordIdInBert,
        const std::vector<std::string>& tokens,
        const std::string& prefix,
        const std::string& postfix,
        int maskId,
        size_t idx,
        int maxLen,
        vector<int>& masks) {
    if (! prefix.empty()) {
        masks.push_back(wordIdInBert.at(prefix));
    }
    for (size_t i=idx; i < tokens.size() && i - idx < size_t(maxLen); ++i) {
        masks.push_back(maskId);
    }
    if (! postfix.empty()) {
        masks.push_back(wordIdInBert.at(postfix));
    }
}
void AppendBertIdUnk(const std::map<std::string, int> &wordIdInBert,
        const std::vector<std::string>& tokens,
        const std::string& prefix,
        const std::string& postfix,
        int segId,
        size_t idx,
        int maxLen,
        vector<int>& ids,
        vector<int>& segs,
        std::map<std::string,int> &unkMap,
        int &index) {
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
            if(unkMap.size()==0){
                unkMap.insert(std::pair<std::string,int>(tokens[i],unk));
                ids.push_back(unk);
                index=0;
            }else{
                auto tokenIterUnk = unkMap.find(tokens[i]);
                if(tokenIterUnk!=unkMap.end()){
                    ids.push_back(tokenIterUnk->second);
                }else{
                    if(index<98){
                        index+=1;
                        unkMap.insert(std::pair<std::string,int>(tokens[i],index));                   
                        ids.push_back(index);
                        
                    }else{
                        index=1;
                        unkMap.insert(std::pair<std::string,int>(tokens[i],index));  
                        ids.push_back(index);
                    }

                }              
            }
        }
        segs.push_back(segId);
    }
    if (! postfix.empty()) {
        ids.push_back(wordIdInBert.at(postfix));
        segs.push_back(segId);
    }
}
bool QcExtractor::extractQContentQTQ4P(
        const std::map<std::string, int> &wordIdInBert,
        const std::vector<std::string>& queryTokens,
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
        bool qtcStudentBugfixFlag) {

    if (body.empty() && title.empty()) {
        return true;
    }

    // query
    std::vector<int> qIds, qSegIds;
    AppendBertId(wordIdInBert, queryTokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds);
    
    // title
    qtIds = qIds;
    qtSegs = qSegIds;
    std::vector<std::string> tokens;
    string t = toLowerString(title);
    splitToWordsWordPiece(t, tokens, true, true, wordIdInBert);
    int maxLen = maxQtLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtIds, qtSegs);

    // content
    string content = body.substr(0, 50000);
    string_replace(content, " ", "");
    string_replace(content, "　", "");
    string_replace(content, sepInBody+sepInBody+sepInBody, "\n");
    tokens.clear();
    if (qtcStudentBugfixFlag) {
        splitToWordsWordPiece(content, tokens, true, true, wordIdInBert, 5000);
    } else {
        splitToWordsWordPiece(content, tokens, true, true, wordIdInBert, 1024);
    }
    qpIds.resize(maxPassageNum);
    qpSegs.resize(maxPassageNum);
    maxLen = maxQpLen - qIds.size() - 1;
    size_t idx = 0;
    for (int i=0; i<maxPassageNum; ++i) {
        qpIds[i] = qIds;
        qpSegs[i] = qSegIds;
        AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1,
                idx, maxLen, qpIds[i], qpSegs[i]);
        idx += maxLen;
    }

    return true;
}

bool QcExtractor::extractQContentQTQ4P(
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
        bool qtcStudentBugfixFlag) {

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
    int maxLen = maxQtLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtIds, qtSegs);

    // content
    string content = body.substr(0, 50000);
    string_replace(content, " ", "");
    string_replace(content, "　", "");
    string_replace(content, sepInBody+sepInBody+sepInBody, "\n");
    tokens.clear();
    if (qtcStudentBugfixFlag) {
        splitToWordsWordPiece(content, tokens, true, true, wordIdInBert, 5000);
    } else {
        splitToWordsWordPiece(content, tokens, true, true, wordIdInBert, 1024);
    }
    qpIds.resize(maxPassageNum);
    qpSegs.resize(maxPassageNum);
    maxLen = maxQpLen - qIds.size() - 1;
    size_t idx = 0;
    for (int i=0; i<maxPassageNum; ++i) {
        qpIds[i] = qIds;
        qpSegs[i] = qSegIds;
        AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1,
                idx, maxLen, qpIds[i], qpSegs[i]);
        idx += maxLen;
    }

    return true;
}

// nayu.zq for qtc_teacher_v1
bool QcExtractor::extractQContentQTQC(
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
        int maxQcLen) {

    if (body.empty() && title.empty()) {
        return true;
    }
    // process query not include CLS and SEP
    std::vector<std::string> tokens;
    string q = toLowerString(query);
    splitToWordsWordPiece(q, tokens, true, true, wordIdInBert);
    AppendBertId(wordIdInBert, tokens, "", "", 0, 0, maxQLen, queryIds, querySegIds);

    // query
    tokens.clear();
    std::vector<int> qIds, qSegIds;
    splitToWordsWordPiece(q, tokens, true, true, wordIdInBert);
    AppendBertId(wordIdInBert, tokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds);
    
    // title
    qtIds = qIds;
    qtSegs = qSegIds;
    tokens.clear();
    string t = toLowerString(title);
    splitToWordsWordPiece(t, tokens, true, true, wordIdInBert);
    int maxLen = maxQtLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtIds, qtSegs);

    // content
    qcIds = qIds;
    qcSegs = qSegIds;
    tokens.clear();
    string content = body.substr(0, 50000);
    //string_replace(content, " ", "");
    string_replace(content, "　", " ");
    string_replace(content, sepInBody+sepInBody+sepInBody, "\n");
    splitToWordsWordPiece(content, tokens, true, true, wordIdInBert, 10000);
    maxLen = maxQcLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qcIds, qcSegs);

    return true;
}

// nayu.zq for qtc_teacher_v1
bool QcExtractor::extractQContentQTQCQTC(
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
        int maxQcLen) {

    if (body.empty() && title.empty()) {
        return true;
    }
    // process query not include CLS and SEP
    std::vector<std::string> tokens;
    string q = toLowerString(query);
    splitToWordsWordPiece(q, tokens, true, true, wordIdInBert);
    AppendBertId(wordIdInBert, tokens, "", "", 0, 0, maxQLen, queryIds, querySegIds);

    // query
    tokens.clear();
    std::vector<int> qIds, qSegIds;
    splitToWordsWordPiece(q, tokens, true, true, wordIdInBert);
    AppendBertId(wordIdInBert, tokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds);
    
    // title
    qtIds = qIds;
    qtSegs = qSegIds;
    tokens.clear();
    string t = toLowerString(title);
    splitToWordsWordPiece(t, tokens, true, true, wordIdInBert);
    int maxLen = maxQtLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtIds, qtSegs);

    // content
    qcIds = qIds;
    qcSegs = qSegIds;
    tokens.clear();
    string content = body.substr(0, 50000);
    //string_replace(content, " ", "");
    string_replace(content, "　", " ");
    string_replace(content, sepInBody+sepInBody+sepInBody, "\n");
    splitToWordsWordPiece(content, tokens, true, true, wordIdInBert, 10000);
    maxLen = maxQcLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qcIds, qcSegs);

    // qtc
    qtcIds = qtIds;
    qtcSegs = qtSegs;
    maxLen = maxQcLen - qtIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtcIds, qtcSegs);
    
    return true;
}

void QcExtractor::extractQueryToken(
        const std::map<std::string, int> &wordIdInBert,
        const std::string &query,
        std::vector<std::string>& tokens,
        bool unkRemap)
{
    string q = toLowerString(query);
    splitToWordsWordPiece(q, tokens, true, true, wordIdInBert,512,unkRemap);
}

bool QcExtractor::extractQContentQTQC(
        const std::map<std::string, int> &wordIdInBert,
        const std::vector<std::string>& queryTokens,
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
        int maxQcLen) {

    if (body.empty() && title.empty()) {
        return true;
    }
    // process query not include CLS and SEP
    AppendBertId(wordIdInBert, queryTokens, "", "", 0, 0, maxQLen, queryIds, querySegIds);

    // query
    std::vector<int> qIds, qSegIds;
    AppendBertId(wordIdInBert, queryTokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds);
    
    // title
    qtIds = qIds;
    qtSegs = qSegIds;
    std::vector<std::string> tokens;
    string t = toLowerString(title);
    splitToWordsWordPiece(t, tokens, true, true, wordIdInBert);
    int maxLen = maxQtLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtIds, qtSegs);

    // content
    qcIds = qIds;
    qcSegs = qSegIds;
    tokens.clear();
    if (body.size() > 50000) body.resize(50000);
    string_replace(body, "　", " ");
    string_replace(body, sepInBody+sepInBody+sepInBody, "\n");
    splitToWordsWordPiece(body, tokens, true, true, wordIdInBert, 10000);
    maxLen = maxQcLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qcIds, qcSegs);

    return true;
}

bool QcExtractor::extractQContentQTQCQTC(
        const std::map<std::string, int> &wordIdInBert,
        const std::vector<std::string>& queryTokens,
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
        int maxQcLen) {

    if (body.empty() && title.empty()) {
        return true;
    }
    // process query not include CLS and SEP
    AppendBertId(wordIdInBert, queryTokens, "", "", 0, 0, maxQLen, queryIds, querySegIds);

    // query
    std::vector<int> qIds, qSegIds;
    AppendBertId(wordIdInBert, queryTokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds);
    
    // title
    qtIds = qIds;
    qtSegs = qSegIds;
    std::vector<std::string> tokens;
    string t = toLowerString(title);
    splitToWordsWordPiece(t, tokens, true, true, wordIdInBert);
    int maxLen = maxQtLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtIds, qtSegs);

    // content
    qcIds = qIds;
    qcSegs = qSegIds;
    tokens.clear();
    if (body.size() > 50000) body.resize(50000);
    string_replace(body, "　", " ");
    string_replace(body, sepInBody+sepInBody+sepInBody, "\n");
    splitToWordsWordPiece(body, tokens, true, true, wordIdInBert, 10000);
    maxLen = maxQcLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qcIds, qcSegs);

    // qtc
    qtcIds = qtIds;
    qtcSegs = qtSegs;
    maxLen = maxQcLen - qtIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtcIds, qtcSegs);
    
    return true;
}

bool QcExtractor::extractQContentQTlistQTClist(
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
        bool split_bert) {

    if (body.empty() && title.empty()) {
        return true;
    }
    // process query not include CLS and SEP
    AppendBertId(wordIdInBert, queryTokens, "", "", 0, 0, maxQLen, queryIds, querySegIds);

    // // tlist top3
    // std::vector<std::string> tlist_top3;
    // for (size_t n = 0; n < tlist_top4.size(); ++n) {
    //     if ( strcmp(tlist_top4[n].c_str(),title.c_str()) == 0 ){
    //         continue;
    //     }
    //     tlist_top3.push_back(tlist_top4[n]);
    //     if (tlist_top3.size() == 3){
    //         break;
    //     }
    // }
    // AGG_LOG(DEBUG, "[csc]qtlist tlist: title[%s], tlist1[%s], tlist2[%s], tlist3[%s], tlist_size[%u]", 
    //                 title.c_str(), tlist_top3[0].c_str(), tlist_top3[1].c_str(), tlist_top3[2].c_str(), tlist_top3.size());

    // query
    std::vector<int> qIds, qSegIds;
    AppendBertId(wordIdInBert, queryTokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds);
    
    // qt-list
    qtlistIds = qIds;
    qtlistSegs = qSegIds;
    qtlistMask = qSegIds;
    // tlist1
    string tlist1 = toLowerString(tlist_top3[0]);
    std::vector<std::string> tlist_token1;
    splitToWordsWordPiece(tlist1, tlist_token1, true, true, wordIdInBert);
    int maxLen1 = qIds.size() + maxtlistLen - 1;
    AppendBertId(wordIdInBert, tlist_token1, "", "[unused99]", 0, 0, maxLen1, qtlistIds, qtlistSegs);
    AppendQtclistMaskId(wordIdInBert, tlist_token1, "", "[unused99]", 1, 0, maxLen1, qtlistMask);
    // tlist2
    string tlist2 = toLowerString(tlist_top3[1]);
    std::vector<std::string> tlist_token2;
    splitToWordsWordPiece(tlist2, tlist_token2, true, true, wordIdInBert);
    int maxLen2 = maxLen1 + maxtlistLen - 1;
    AppendBertId(wordIdInBert, tlist_token2, "", "[unused99]", 0, 0, maxLen2, qtlistIds, qtlistSegs);
    AppendQtclistMaskId(wordIdInBert, tlist_token2, "", "[unused99]", 2, 0, maxLen2, qtlistMask);
    // tlist3
    string tlist3 = toLowerString(tlist_top3[2]);
    std::vector<std::string> tlist_token3;
    splitToWordsWordPiece(tlist3, tlist_token3, true, true, wordIdInBert);
    int maxLen3 = maxLen2 + maxtlistLen - 1;
    AppendBertId(wordIdInBert, tlist_token3, "", "[SEP]", 0, 0, maxLen3, qtlistIds, qtlistSegs);
    AppendQtclistMaskId(wordIdInBert, tlist_token3, "", "[SEP]", 3, 0, maxLen3, qtlistMask);
    // title
    std::vector<std::string> tokens;
    string t = toLowerString(title);
    splitToWordsWordPiece(t, tokens, true, true, wordIdInBert);
    int maxLen = maxQtLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtlistIds, qtlistSegs);
    AppendQtclistMaskId(wordIdInBert, tokens, "", "[SEP]", 4, 0, maxLen, qtlistMask);

    // qc
    qcIds = qIds;
    qcSegs = qSegIds;
    tokens.clear();
    if (body.size() > 50000) body.resize(50000);
    string_replace(body, "　", " ");
    string_replace(body, sepInBody+sepInBody+sepInBody, "\n");
    splitToWordsWordPiece(body, tokens, true, true, wordIdInBert, 10000, false, split_bert);
    maxLen = maxQcLen - qIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qcIds, qcSegs);

    // qtc-list
    qtclistIds = qtlistIds;
    qtclistSegs = qtlistSegs;
    qtclistMask = qtlistMask;
    maxLen = maxQcLen - qtlistIds.size() - 1;
    AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtclistIds, qtclistSegs);
    AppendQtclistMaskId(wordIdInBert, tokens, "", "[SEP]", 5, 0, maxLen, qtclistMask);
    
    return true;
}


bool QcExtractor::extractQContentQTlistQTClistUNK(
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
        bool split_bert) {

    if (body.empty() && title.empty()) {
        return true;
    }

    bool unkRemap=true;
    std::map<std::string,int> unkUniqMap;
    int index=0;

    // process query not include CLS and SEP
    // AppendBertId(wordIdInBert, queryTokens, "", "", 0, 0, maxQLen, queryIds, querySegIds);

    AppendBertIdUnk(wordIdInBert, queryTokens, "", "", 0, 0, maxQLen, queryIds, querySegIds,unkUniqMap,index);

    // query
    std::vector<int> qIds, qSegIds;
    // AppendBertId(wordIdInBert, queryTokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds);
    AppendBertIdUnk(wordIdInBert, queryTokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds,unkUniqMap,index);
    
    // qt-list
    qtlistIds = qIds;
    qtlistSegs = qSegIds;
    qtlistMask = qSegIds;
    // tlist1
    string tlist1 = toLowerString(tlist_top3[0]);
    std::vector<std::string> tlist_token1;
    splitToWordsWordPiece(tlist1, tlist_token1, true, true, wordIdInBert,512,unkRemap);
    int maxLen1 = qIds.size() + maxtlistLen - 1;
    // AppendBertId(wordIdInBert, tlist_token1, "", "[unused99]", 0, 0, maxLen1, qtlistIds, qtlistSegs);
    AppendBertIdUnk(wordIdInBert, tlist_token1, "", "[unused99]", 0, 0, maxLen1, qtlistIds, qtlistSegs,unkUniqMap,index);

    AppendQtclistMaskId(wordIdInBert, tlist_token1, "", "[unused99]", 1, 0, maxLen1, qtlistMask);
    // tlist2
    string tlist2 = toLowerString(tlist_top3[1]);
    std::vector<std::string> tlist_token2;
    splitToWordsWordPiece(tlist2, tlist_token2, true, true, wordIdInBert,512, unkRemap);
    int maxLen2 = maxLen1 + maxtlistLen - 1;
    // AppendBertId(wordIdInBert, tlist_token2, "", "[unused99]", 0, 0, maxLen2, qtlistIds, qtlistSegs);

    AppendBertIdUnk(wordIdInBert, tlist_token2, "", "[unused99]", 0, 0, maxLen2, qtlistIds, qtlistSegs, unkUniqMap,index);

    AppendQtclistMaskId(wordIdInBert, tlist_token2, "", "[unused99]", 2, 0, maxLen2, qtlistMask);
    // tlist3
    string tlist3 = toLowerString(tlist_top3[2]);
    std::vector<std::string> tlist_token3;
    splitToWordsWordPiece(tlist3, tlist_token3, true, true, wordIdInBert,512, unkRemap);
    int maxLen3 = maxLen2 + maxtlistLen - 1;
    // AppendBertId(wordIdInBert, tlist_token3, "", "[SEP]", 0, 0, maxLen3, qtlistIds, qtlistSegs);

    AppendBertIdUnk(wordIdInBert, tlist_token3, "", "[SEP]", 0, 0, maxLen3, qtlistIds, qtlistSegs, unkUniqMap,index);

    AppendQtclistMaskId(wordIdInBert, tlist_token3, "", "[SEP]", 3, 0, maxLen3, qtlistMask);
    // title
    std::vector<std::string> tokens;
    string t = toLowerString(title);
    splitToWordsWordPiece(t, tokens, true, true, wordIdInBert,512, unkRemap);
    int maxLen = maxQtLen - qIds.size() - 1;
    // AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtlistIds, qtlistSegs);
    AppendBertIdUnk(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtlistIds, qtlistSegs,unkUniqMap,index);
    AppendQtclistMaskId(wordIdInBert, tokens, "", "[SEP]", 4, 0, maxLen, qtlistMask);

    // qc
    qcIds = qIds;
    qcSegs = qSegIds;
    tokens.clear();
    if (body.size() > 50000) body.resize(50000);
    string_replace(body, "　", " ");
    string_replace(body, sepInBody+sepInBody+sepInBody, "\n");
    splitToWordsWordPiece(body, tokens, true, true, wordIdInBert, 10000, unkRemap, split_bert);
    maxLen = maxQcLen - qIds.size() - 1;
    // AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qcIds, qcSegs);
    AppendBertIdUnk(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qcIds, qcSegs,unkUniqMap,index);

    // qtc-list
    qtclistIds = qtlistIds;
    qtclistSegs = qtlistSegs;
    qtclistMask = qtlistMask;
    maxLen = maxQcLen - qtlistIds.size() - 1;
    // AppendBertId(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtclistIds, qtclistSegs);
    AppendBertIdUnk(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtclistIds, qtclistSegs, unkUniqMap,index);
    AppendQtclistMaskId(wordIdInBert, tokens, "", "[SEP]", 5, 0, maxLen, qtclistMask);

    for (auto iter = unkUniqMap.begin(); iter != unkUniqMap.end(); ++iter) {
        AGG_LOG(DEBUG,"lujunfeng unk token:%s,unk index:%d",iter->first.c_str(),iter->second);
    }
    
    return true;
}

bool QcExtractor::extractQContentQTQCQTCUNK(
        const std::map<std::string, int> &wordIdInBert,
        const std::vector<std::string>& queryTokens,
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
        int maxQcLen) {

    if (body.empty() && title.empty()) {
        return true;
    }
    bool unkRemap=true;
    std::map<std::string,int> unkUniqMap;
    int index=0;
    // process query not include CLS and SEP
    AppendBertIdUnk(wordIdInBert, queryTokens, "", "", 0, 0, maxQLen, queryIds, querySegIds,unkUniqMap,index);
    // query
    std::vector<int> qIds, qSegIds;
    AppendBertIdUnk(wordIdInBert, queryTokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds,unkUniqMap,index);
    // title
    qtIds = qIds;
    qtSegs = qSegIds;
    std::vector<std::string> tokens;
    string t = toLowerString(title);
    splitToWordsWordPiece(t, tokens, true, true, wordIdInBert,512,unkRemap);
    int maxLen = maxQtLen - qIds.size() - 1;
    AppendBertIdUnk(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtIds, qtSegs,unkUniqMap,index);
    // content
    qcIds = qIds;
    qcSegs = qSegIds;
    tokens.clear();
    if (body.size() > 50000) body.resize(50000);
    string_replace(body, "　", " ");
    string_replace(body, sepInBody+sepInBody+sepInBody, "\n");
    splitToWordsWordPiece(body, tokens, true, true, wordIdInBert, 10000,unkRemap);
    maxLen = maxQcLen - qIds.size() - 1;
    AppendBertIdUnk(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qcIds, qcSegs,unkUniqMap,index);
    // qtc
    qtcIds = qtIds;
    qtcSegs = qtSegs;
    maxLen = maxQcLen - qtIds.size() - 1;
    AppendBertIdUnk(wordIdInBert, tokens, "", "[SEP]", 1, 0, maxLen, qtcIds, qtcSegs,unkUniqMap,index);
    for (auto iter = unkUniqMap.begin(); iter != unkUniqMap.end(); ++iter) {
        AGG_LOG(DEBUG,"unk token:%s,unk index:%d",iter->first.c_str(),iter->second);
    }
    return true;
}

bool QcExtractor::extractQueryQtcNn(const std::map<std::string, int> &wordIdInBert,
                                    const std::string &query,
                                    std::string & query_dat,
                                    int maxQLen)
{
    MESSER_UTILS_NS(util)::ostringstream oss;
    // query
    std::vector<std::string> tokens;
    std::vector<int> qIds, qSegIds;
    std::string q = toLowerString(query);
    splitToWordsWordPiece(q, tokens, true, true, wordIdInBert);
    AppendBertId(wordIdInBert, tokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds);

    query_dat += "[dat]\n";
    query_dat +="query_bert_id=0:";
    for (size_t i = 0; i < qIds.size(); ++i) {
        if (i > 0) {
            query_dat += ",";
        }
        query_dat += std::to_string(qIds[i]);
    }

    query_dat += "\nquery_segment_id=0:";
    for (size_t j = 0; j < qSegIds.size(); ++j) {
        if (j > 0) {
            query_dat += ",";
        }
        query_dat += std::to_string(qSegIds[j]);
    }
    // query_dat += qSegIds;
    return true;
}

bool QcExtractor::extractQueryL2NN(const std::map<std::string, int> &wordIdInBert,
                                    const std::string &query,
                                    std::string & query_dat,
                                    int maxQLen)
{
    MESSER_UTILS_NS(util)::ostringstream oss;
    // query
    std::vector<std::string> tokens;
    std::vector<int> qIds, qSegIds;
    std::string q = toLowerString(query);
    splitToWordsWordPiece(q, tokens, true, true, wordIdInBert);
    AppendBertId(wordIdInBert, tokens, "[CLS]", "[SEP]", 0, 0, maxQLen, qIds, qSegIds);

    oss << "[dat]\n" << "query_id=0:";
    for (size_t i = 0; i < qIds.size(); ++i) {
        if (i > 0) {
            oss << ",";
        }
        oss << qIds[i];
    }
    query_dat = oss.str();
    return true;
}

bool QcExtractor::IsAlpha(const char& ch)
{
    return ((ch >= 'a' && ch <= 'z') ||
        (ch >= 'A' && ch <= 'Z'));
}


bool QcExtractor::IsContainAlphaString(const std::string& str)
{
    for (size_t i = 0; i < str.size(); ++i) {
        if (QcExtractor::IsAlpha(str[i]) == true) {
            return true;
        }
    }
    return false;
}

// end of Contont-Extracting

AGG_END_NAMESPACE(agg_web2);
