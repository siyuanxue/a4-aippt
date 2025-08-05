#ifndef AGG2_HELPER_GENERAL_SEARCH_HELPER_H
#define AGG2_HELPER_GENERAL_SEARCH_HELPER_H

#include <agg_web2/common/common.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class GeneralSearchHelper
{
public:
    GeneralSearchHelper();
    ~GeneralSearchHelper();

public:
    static bool RewriteQueryNeedRecover(const std::string& query,
                                        std::vector<std::string>& rewriteQueryVec);
    static bool SingleRewriteQueryNeedRecover(const std::string& query, std::vector<std::string>& singleRewriteQueryVec);
    static bool IsAllAlNum(const std::string& content);
    static bool IsEmpty(const std::string& content);
    static bool HasChinese(const std::string& content);
    static bool IsChineseChar(const char& ch);
    static bool IsAlNum(const char& ch);

    static void WeatherFormat(std::string& query, std::string& extra_refer);
    static void WeatherReferFormat(std::string& origin_refer, const std::vector<std::string>& date);
    static bool GetTicketsMoney(const std::string& origin_number, std::string& film_tickets_money);
    static void FilmFormat(std::string& query, std::string& extra_refer);
    static std::string GetFormatTime(int Currenttime, const std::string& mode);

    static bool CheckSimilarity(const std::string& query, const std::string& latestQuery);
    static void LStripStrInPlace(std::string& in, const std::vector<std::string>& syms);
    static void RStripStrInPlace(std::string& in, const std::vector<std::string>& syms);

private:
    AGG_LOG_DECLARE();
};

AGG_END_NAMESPACE(agg_web2);

#endif
