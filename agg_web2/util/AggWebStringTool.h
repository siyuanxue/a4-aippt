#ifndef AGG_WEB2_QA_AGG_STRING_TOOL_H
#define AGG_WEB2_QA_AGG_STRING_TOOL_H

#include <agg2/util/string_tool.h>
#include <string>
#include <regex>

AGG_BEGIN_NAMESPACE(agg_web2);

class AggContext;

class AggWebStringTool : public StringTool
{
public:
    static void PrintCacheParamStr(AggContext * aggContext);

    // 查找字符串出现的次数
    static int CheckRepeatCount(const std::string& text, const std::string& word);

    static bool FilterInvalidXmlChar(const std::string& query, const std::string& xmlStr, std::string& destStr);
    static std::string FilterInvalidChar(const std::string& inputStr);
    static bool IsAlNumStr(const std::string &str);
    // 判断str 是否和后面所有字符串中的某一个相等
    template<typename... T>
    static bool IsAny(const std::string& str, T... all)
    {
        std::string strarray[] = {all...};
        for(const std::string& s : strarray){
            if(str == s){
                return true;
            }
        }

        return false;
    }

    static std::vector<std::string> SplitBySpliter(const std::string& str, const std::string& regexStr);
    static int FindSubStr(const std::string& inputstr, const std::string& target);

    static std::string Double2str(double num, int precise = 3);

    static std::string GetShortValue(const std::string & str);
    static void RemoveRedTag(std::string &s);
    static int ExtraNum(std::string & str, std::vector<std::string> &res);
    static bool IsNum(const std::string str);
    static bool FindNearStr(std::string& mainStr, std::string& targetStr, std::string& nearStr, int leftOrRight, size_t& targetPos);
    static double GetApproximateSimilarityWithMaxLen(std::string &source, std::string &dest, size_t maxLen);
    static std::string SubStrByUtf8Size(std::string& str, size_t utfSize);
private:
    AGG_LOG_DECLARE();
    AGG_LOG_DECLARE_WITH_NAME(monitorLog);

};

AGG_END_NAMESPACE(agg_web2);
#endif 
