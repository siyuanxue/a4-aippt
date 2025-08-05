#include <agg_web2/util/AggWebStringTool.h>
#include <agg2/util/string_tool.h>
#include <agg2/common/agg_log.h>
#include <agg2/helper/agg_context_helper.h>
#include <agg2/helper/agg_sub_context_helper.h>
#include <messer_utils/util/num_to_string.h>
#include <A4/util/string_util.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, AggWebStringTool);
AGG_LOG_SETUP_WITH_NAME(monitorLog, monitorLog, AggWebStringTool);

#define DEBUG_SEG_LENGTH (3096)

using namespace std;

void AggWebStringTool::PrintCacheParamStr(AggContext * aggContext)
{
    if (aggContext == NULL) {
        AGG_LOG(ERROR, "aggContext is NULL");
        return;
    }

    std::string query = AggContextHelper::GetRawQuery(aggContext);
    std::string belong = AggContextHelper::GetRequestParam(aggContext, "belong");
    std::string cacheParam = belong;
    cacheParam.append(";").append(AggContextHelper::GetCacheParam(aggContext));
    size_t subContextCount = aggContext->GetSubContextCount();
    for (size_t i = 0; i < subContextCount; ++i) {
        AggSubContext * aggSubContext = aggContext->GetSubContext(i);
        cacheParam.append("`0").append(AggSubContextHelper::GetCacheParam(aggSubContext));
    }

    AGG_LOG_WITH_NAME(monitorLog, INFO, "[CACHE_PARAM] query:[%s] text:[%s]", query.c_str(), cacheParam.c_str()); 
}

int AggWebStringTool::CheckRepeatCount(const std::string& text, 
    const std::string& word) {
    if (word.length() == 0) {
        return 0;
    }
    size_t pos = 0;
    int cnt = 0;
    while((pos=text.find(word, pos)) != string::npos) {
        cnt ++;
        pos += word.length();
    }

    return cnt;
}

bool AggWebStringTool::IsAlNumStr(const std::string &str)
{
    for (size_t i = 0; i != str.size(); ++i) {
        if (!isalnum(str[i])) {
            return false;
        }
    }
    return true;
}

std::string AggWebStringTool::FilterInvalidChar(const std::string& inputStr)
{
    string destStr;
    std::string::const_iterator lastValidIter = inputStr.begin();
    size_t i = 0;
    for(std::string::const_iterator iter = inputStr.begin(); iter != inputStr.end();) {
        uint32_t unicodeValue = 0;
        std::string::const_iterator preIter = iter;
        if (StringTool::GetUnicodeValue(inputStr, iter, unicodeValue)) {
            if (StringTool::IsValidXmlUnicodeValue(unicodeValue)) {
                continue;
            }
        }

        destStr.append(lastValidIter, preIter);
        lastValidIter = iter;
    }

    if (lastValidIter == inputStr.begin()) {
        return inputStr;
    }
    
    destStr.append(lastValidIter, inputStr.end());
    return destStr;
}

bool AggWebStringTool::FilterInvalidXmlChar(const std::string& query, const std::string& xmlStr, std::string& destStr)
{
    // 检测是否已分配内存
    bool mallocDest = false;
    destStr.clear();

    std::string::const_iterator lastValidIter = xmlStr.begin();
    size_t i = 0;
    for(std::string::const_iterator iter = xmlStr.begin(); iter != xmlStr.end();) {
        uint32_t unicodeValue = 0;
        std::string::const_iterator preIter = iter;
        if (StringTool::GetUnicodeValue(xmlStr, iter, unicodeValue)) {
            if (StringTool::IsValidXmlUnicodeValue(unicodeValue)) {
                continue;
            }
        }
    
        /*
         * xmlStr一般会比较大，预先分配内存
         */
        if (!mallocDest) {
            mallocDest = true;
            destStr.reserve(xmlStr.size());
            size_t from_start = std::distance(xmlStr.begin(), iter); //当前位置距离开头X个字符
            if(from_start > DEBUG_SEG_LENGTH){
                from_start = from_start - DEBUG_SEG_LENGTH;
            } else {
                from_start = 0;
            }
            AGG_LOG_WITH_NAME(monitorLog, INFO, "[INVALID_UNICODE] query:[%s] text:[%s]", 
                    query.c_str(), xmlStr.substr(from_start, 2*DEBUG_SEG_LENGTH).c_str());   
        }

        destStr.append(lastValidIter, preIter);
        lastValidIter = iter;
    }

    if (lastValidIter == xmlStr.begin()) {
        return false;
    }
    
    destStr.append(lastValidIter, xmlStr.end());
    return true;
}

int AggWebStringTool::FindSubStr(const std::string& inputstr, const std::string& target){
   auto it = inputstr.find(target);
   if (it == inputstr.npos){
       return -1;
   }
   else{
       std::string beforestr = inputstr.substr(0, it);
       std::vector<std::string> words;
       int cnt = StringTool::SplitToWords(beforestr, words, false, false); 
       return cnt;
   }
}

std::vector<std::string> AggWebStringTool::SplitBySpliter(const std::string& str, const std::string& regexStr){
    // regex reg(regexStr);
    // vector<string> tokens;
    // sregex_token_iterator it(str.begin(), str.end(), reg, -1);
    // sregex_token_iterator end;
    // while (it != end) {
    //     tokens.push_back(*it);
    //     ++it;
    // }
    // return tokens;
    std::string inputstr = str;
    std::string newSpliter = "`1<seg>`1";
    std::vector<std::string> spliter;
    StringTool::SplitToWords(regexStr, spliter, false, true);
    for(const auto& i : spliter){
        // 保留原始标点符号
        inputstr = StringTool::ReplaceStr(inputstr, i, i + newSpliter);
        AGG_LOG(DEBUG, "replace %s -> %s", i.c_str(), (i + newSpliter).c_str());
    } 
    std::vector<string> sentences =  autil::StringUtil::split(inputstr, newSpliter, false);
    return sentences;
}

std::string AggWebStringTool::Double2str(double num, int precise)
{
    if (precise > 31) precise = 31;
    char iToaBuffer[32] = {'\0'};
    MESSER_UTILS_NS(util)::NumUtil::dtoa(num, iToaBuffer, precise);
    string output(iToaBuffer);
    return std::move(output);
}

std::string AggWebStringTool::GetShortValue(const std::string& str)
{
    auto pos = str.find(".");
    if (pos == str.npos) {
        return str;
    }
    else {
        // 保留小数点后两位
        return str.substr(0, pos + 3);
    }
}

void AggWebStringTool::RemoveRedTag(std::string &s){
    // 去除title或者body中的飘红样式
    s = StringTool::RemoveStr(s, "<font color=red>");
    s = StringTool::RemoveStr(s, "</font>");
    s = StringTool::RemoveStr(s, "<em>");
    s = StringTool::RemoveStr(s, "</em>");
}

int AggWebStringTool::ExtraNum(std::string & str, std::vector<std::string> &res){
    // 将str中所有数字提取出来
    std::string s = str + "a";
    int count = 0;
    while(!s.empty()){
        std::size_t n = s.find_first_of("0123456789");
        if (n != std::string::npos)
        {
            std::size_t m = s.find_first_not_of("0123456789", n);
            size_t len = m-n;
            res.push_back(s.substr(n, len));
            count ++;
            s = s.substr(n+len);
        } else {
            break;
        }
    }
    return count;
}

bool AggWebStringTool::IsNum(const std::string str)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        int tmp = (int)str[i];
        if (tmp >= 48 && tmp <= 57)
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool AggWebStringTool::FindNearStr(std::string& mainStr, std::string& targetStr, std::string& nearStr, int leftOrRight, size_t& targetPos){
    // 在mainStr中寻找最靠近nearStr的targetStr，并返回位置targetPos， leftOrRight=0：寻找nearStr的左边，1：右边
    size_t nearStrPos = mainStr.find(nearStr);
    if (nearStrPos == string::npos){
        AGG_LOG(DEBUG, "failed to find %s in mainStr", nearStr.c_str());
        return false;
    }
    if (leftOrRight == 0){ // 左边
        targetPos = mainStr.rfind(targetStr, nearStrPos);
        if (targetPos == string::npos){
            AGG_LOG(DEBUG, "failed to find %s before %s", targetStr.c_str(), nearStr.c_str());
            return false;
        }
    } else { // 右边
        targetPos = mainStr.find(targetStr, nearStrPos + nearStr.size());
        if (targetPos == string::npos){
            AGG_LOG(DEBUG, "failed to find %s after %s", targetStr.c_str(), nearStr.c_str());
            return false;
        }
    }
    return true;
}

double AggWebStringTool::GetApproximateSimilarityWithMaxLen(std::string &source, std::string &dest, size_t maxLen) {
    // 根据字频判断两个字符串是否相似
    if (source.empty() || dest.empty()) {
        return 0.0;
    }

    size_t len = source.size() > dest.size() ? source.size() : dest.size();
    if (len > maxLen) { len = maxLen;}

    // 统计字频
    int sourceStat[256];
    memset(sourceStat, 0, 256*sizeof(int));
    size_t cnt = 0;
    for (unsigned char c : source) {
        if (cnt > maxLen) {
            break;
        }
        sourceStat[c] += 1;
        ++cnt;
    }

    cnt = 0;
    for (unsigned char c : dest) {
        if (cnt > maxLen) {
            break;
        }
        sourceStat[c] -= 1;
        ++cnt;
    }

    // 统计字频的 diff
    int32_t diff = 0;
    for (int diffPerChar : sourceStat) {
        diff += abs(diffPerChar);
    }

    size_t sourceLen = (len > source.size() ? source.size() : len);
    size_t destLen = (len > dest.size() ? dest.size() : len);
    double sim = 1.0 - (diff / (double(sourceLen + destLen)));

    return sim;
}

std::string AggWebStringTool::SubStrByUtf8Size(std::string& str, size_t utfSize) {
    // 根据utf8的长度，从头开始截断字符串
    size_t idx = 0;
    size_t curSize = 0;
    while (idx < str.size() && curSize < utfSize) {
        std::string utf8_str = StringUtil::GetNextCharUTF8(str, idx);
        if(utf8_str.empty()) break;
        curSize ++;
        idx += utf8_str.size();
    }
    return str.substr(0, idx);
}

AGG_END_NAMESPACE(agg_web2);
