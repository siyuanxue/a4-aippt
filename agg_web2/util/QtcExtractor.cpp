#include <agg_web2/util/QtcExtractor.h>
// #include <agg_web2/util/bert_tokenizer.h>
#include <autil/StringUtil.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, QtcExtractor);

using namespace std;


void QtcExtractor::extractQtcStdkv( 
    const std::map<std::string, int>& bertMapData,
    const std::string& query, 
    const std::string& title,
    const std::string& contents,
    std::string& reqStr){

    reqStr.reserve(1024);

    size_t maxQLen = 35;
    size_t maxQtLen = 64;
    size_t maxQcLen = 512;
    size_t maxQtcLen = 512;

    std::vector<string> tokenQResult;
    std::vector<string> tokenQResultId;
    std::vector<string> QsegmentId;

    string origQStr="[CLS]";
    origQStr += ToLowerCase(query);
    origQStr += "[SEP]";
    bertTokenize(origQStr, tokenQResult, tokenQResultId, QsegmentId, bertMapData);
    if(tokenQResultId.size()>maxQLen){
        tokenQResultId.erase(tokenQResultId.begin()+maxQLen-1,tokenQResultId.begin()+tokenQResultId.size());
        tokenQResultId.push_back("102");
    }
    std::string tokenQResultIdStr = autil::StringUtil::toString(tokenQResultId, ",");
    if(tokenQResultIdStr.size()>0){
        reqStr = "[dat]\nquery_id=0:";
        reqStr += tokenQResultIdStr;
    }

    std::vector<string> tokenQTResult;
    std::vector<string> tokenQTResultId;
    std::vector<string> QTsegmentId;
    string origQTStr="[CLS]";
    origQTStr+=ToLowerCase(query);
    origQTStr+="[SEP]";
    origQTStr+=ToLowerCase(title);
    origQTStr+="[SEP]";
    bertTokenize(origQTStr, tokenQTResult, tokenQTResultId,
            QTsegmentId, bertMapData);
    if(tokenQTResultId.size()>maxQtLen){
        tokenQTResultId.erase(tokenQTResultId.begin()+maxQtLen-1,
                tokenQTResultId.begin()+tokenQTResultId.size());
        tokenQTResultId.push_back("102");
    }
    std::string tokenQTResultIdStr = autil::StringUtil::toString(tokenQTResultId, ",");
    std::string QTsegmentIdStr = autil::StringUtil::toString(QTsegmentId, ",");
    if(tokenQTResultIdStr.size()>0){
        reqStr += "\nqt_id=0:";
        reqStr += tokenQTResultIdStr;
        reqStr += "\nqt_segment_id=0:";
        reqStr += QTsegmentIdStr;
    }

    std::vector<string> tokenQCResult;
    std::vector<string> tokenQCResultId;
    std::vector<string> QCsegmentId;
    std::string origQCStr="[CLS]";
    origQCStr+=ToLowerCase(query);
    origQCStr+="[SEP]";
    origQCStr+=ToLowerCase(contents);
    origQCStr+="[SEP]";
    bertTokenize(origQCStr, tokenQCResult, tokenQCResultId,
            QCsegmentId, bertMapData);
    if(tokenQCResultId.size()>maxQcLen) {
        tokenQCResultId.erase(tokenQCResultId.begin()+maxQcLen-1,
                tokenQCResultId.begin()+tokenQCResultId.size());
        tokenQCResultId.push_back("102");
    }
    std::string tokenQCResultIdStr = autil::StringUtil::toString(tokenQCResultId, ",");
    std::string QCsegmentIdStr = autil::StringUtil::toString(QCsegmentId, ",");
    if(tokenQCResultIdStr.size()>0){
        reqStr += "\nqc_id=0:";
        reqStr += tokenQCResultIdStr;
        reqStr += "\nqc_segment_id=0:";
        reqStr += QCsegmentIdStr;
    }

    std::vector<string> tokenQTCResult;
    std::vector<string> tokenQTCResultId;
    std::vector<string> QTCsegmentId;
    string origQTCStr="[CLS]";
    origQTCStr+=ToLowerCase(query);
    origQTCStr+="[SEP]";
    origQTCStr+=ToLowerCase(title);
    origQTCStr+="[SEP]";
    origQTCStr+=ToLowerCase(contents);
    origQTCStr+="[SEP]";
    bertTokenize(origQTCStr, tokenQTCResult, tokenQTCResultId,
            QTCsegmentId, bertMapData);
    if(tokenQTCResultId.size()>maxQtcLen) {
        tokenQTCResultId.erase(tokenQTCResultId.begin()+maxQtcLen-1,
                tokenQTCResultId.begin()+tokenQTCResultId.size());
        tokenQTCResultId.push_back("102");
    }
    std::string tokenQTCResultIdStr = autil::StringUtil::toString(tokenQTCResultId, ",");
    std::string QTCsegmentIdStr = autil::StringUtil::toString(QTCsegmentId, ",");
    if(tokenQTCResultIdStr.size()>0){
        reqStr += "\nqtc_id=0:";
        reqStr += tokenQTCResultIdStr;
        reqStr += "\nqtc_segment_id=0:";
        reqStr += QTCsegmentIdStr;
    }

    return;
}

void QtcExtractor::extractPrStdkv(
    const std::map<std::string, int>& bertMapData,
    const std::string& query,
    const std::string& title,
    const std::string& paragraph,
    std::string& reqStr){

    std::vector<string> tokenResult;
    std::vector<string> tokenResultId;
    std::vector<string> segmentId;
    std::vector<string> maskId;

    reqStr.reserve(1024);

    size_t maxQLen = 35;
    size_t maxQtLen = 64;
    size_t maxQcLen = 512;
    size_t maxQtcLen = 512;

    string origStr = "[CLS]";
    origStr += ToLowerCase(query);
    origStr += "[SEP]";
    origStr += ToLowerCase(title);
    origStr += "[SEP]";
    origStr += ToLowerCase(paragraph);

    bertTokenize(origStr, tokenResult, tokenResultId, segmentId, maskId, bertMapData);
    std::string tokenResultIdStr = autil::StringUtil::toString(tokenResultId, ",");
    std::string segmentIdStr = autil::StringUtil::toString(segmentId, ",");
    std::string maskIdStr = autil::StringUtil::toString(maskId, ",");

    if(tokenResultIdStr.size() > 0) {
        reqStr = "[dat]";
        reqStr += "input_ids=";
        reqStr += autil::StringUtil::toString(tokenResultIdStr.size());
        reqStr += ":";
        reqStr += tokenResultIdStr;
        reqStr += "input_mask=";
        reqStr += autil::StringUtil::toString(maskIdStr.size());
        reqStr += ":";
        reqStr += maskIdStr;
        reqStr += "segment_ids=";
        reqStr += autil::StringUtil::toString(segmentIdStr.size());
        reqStr += ":";
        reqStr += segmentIdStr;
    }

    return;
}

void QtcExtractor::bertTokenize(
    const std::string & sentence, 
    std::vector<std::string> & tokenResult, 
    std::vector<std::string> & tokenResultId,
    std::vector<std::string> & segmentIds,
    const std::map<std::string, int>& bertWord2Id)
{
    tokenResult.clear();
    tokenResultId.clear();
    segmentIds.clear();

    splitToWordsWordPiece(sentence, tokenResult, true, true, bertWord2Id);

    bool foundSep = false;
    for (size_t tokenIndex = 0; tokenIndex < tokenResult.size(); ++tokenIndex)
    {
        int res_id = Find(bertWord2Id, tokenResult[tokenIndex]);
        if (res_id != -1) {
            tokenResultId.push_back(to_string(res_id));
            if (!foundSep) {
                segmentIds.push_back("0");
            } else {
                segmentIds.push_back("1");
            }
        }
        if (tokenResult[tokenIndex] == "[SEP]") 
            foundSep = true;
    }
    AGG_LOG(DEBUG, "Bert Token Result Id [%s]", autil::StringUtil::toString(tokenResultId, ",").c_str());
}

void QtcExtractor::bertTokenize(
    const std::string & sentence, 
    std::vector<std::string> & tokenResult, 
    std::vector<std::string> & tokenResultId,
    std::vector<std::string> & segmentIds,
    std::vector<std::string> & inputMask,
    const std::map<std::string, int>& bertWord2Id)
{
    tokenResult.clear();
    tokenResultId.clear();
    segmentIds.clear();
    inputMask.clear();
    splitToWordsWordPiece(sentence, tokenResult, true, true, bertWord2Id);

    bool foundSep = false;
    for (size_t tokenIndex = 0; tokenIndex < tokenResult.size(); ++tokenIndex)
    {
        int res_id = Find(bertWord2Id, tokenResult[tokenIndex]);
        if (res_id != -1) {
            tokenResultId.push_back(to_string(res_id));
            if (!foundSep) {
                segmentIds.push_back("0");
            } else {
                segmentIds.push_back("1");
            }
            inputMask.push_back("1");
        }

        if (tokenResult[tokenIndex] == "[SEP]") 
            foundSep = true;
    }
}


#define TO_LOWER_CHAR(ch) ((ch >= 'A' && ch <= 'Z') ? (ch - 'A' + 'a') : ch)
std::string QtcExtractor::ToLowerCase(const std::string & in)
{
    std::string res;
    res.reserve(res.size() + 1);
    for (std::string::const_iterator it = in.begin(); it != in.end(); ++it) {
        char tt = TO_LOWER_CHAR(*it);
        res.push_back(tt);
    }
    return res;
}


int QtcExtractor::Find(const std::map<std::string, int>& bertWord2Id, const std::string& subTokenStr)
{
    auto it = bertWord2Id.find(subTokenStr);
    if (it != bertWord2Id.end()) {
        // 键存在于map中
        return it->second;
    } else {
        // 键不存在于map中
        return -1;
    };
}

void QtcExtractor::splitToWordsWordPiece(const std::string &sentence, std::vector<std::string> &result, bool removeSpace, bool mergeAscii, const std::map<std::string, int> &word2Id, size_t maxLen,bool unkRemap, bool split_bert)
{
    result.clear();
    if (sentence.empty())
    {
        return;
    }

    size_t sentenceSize = std::min(sentence.size(), maxLen);
    for (size_t i = 0; i < sentenceSize;)
    {
        // 判断当前的字符串是否是[SEP]或[CLS]，如果是的话，直接将其作为一个单词添加到结果中，然后将索引i增加相应的值，继续下一次循环即可
        if ((sentence[i] == '[' && i + 5 <= sentenceSize && sentence.substr(i, 5) == "[SEP]")
                 || (sentence[i] == '[' && i + 5 <= sentenceSize && sentence.substr(i, 5) == "[CLS]"))
        {
            result.push_back(sentence.substr(i, 5));
            i += 5;
        }
        else if ((sentence[i] & 240) == 224)
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

bool QtcExtractor::isSpecialChar(char c)
{
    std::string sChars = "`~!@#$%^&*()_+-={}[]|:;<>?,\"./\\";
    std::string::size_type idx = sChars.find(c);
    if(idx == std::string::npos) {
        return false;
    }
    else {
        return true;
    }
}


// end of Contont-Extracting

AGG_END_NAMESPACE(agg_web2);
