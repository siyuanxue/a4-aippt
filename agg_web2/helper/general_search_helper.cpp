#include <agg_web2/helper/general_search_helper.h>
#include <autil/StringUtil.h>
#include <agg2/util/string_tool.h>
#include <sstream>
#include <agg2/helper/time_helper.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, GeneralSearchHelper);

GeneralSearchHelper::GeneralSearchHelper()
{
}

GeneralSearchHelper::~GeneralSearchHelper()
{
}

bool GeneralSearchHelper::RewriteQueryNeedRecover(const std::string& query,
    std::vector<std::string>& rewriteQueryVec)
{
    // 如果改写Query为空，使用原始Query请求
    if (rewriteQueryVec.size() == 0) {
        return true;
    }

    // 当改写Query为空 或 改写提示“无需检索” 或 改写与原Query相同时，使用原始Query请求
    if ((rewriteQueryVec.size() == 1) &&
        (GeneralSearchHelper::IsEmpty(rewriteQueryVec[0]) ||
        rewriteQueryVec[0].find("无需检索") != std::string::npos ||
        rewriteQueryVec[0] == query)) {
        return true;
    }

    // 因摘要暂时无法支持多Query的摘要
    // 暂时关闭切分成多Query的查询
    if (rewriteQueryVec.size() > 1) {
        return true;
    }

    // 当原始Query为中文，但改写Query全部为英文和数字，改回原始Query
    if (GeneralSearchHelper::HasChinese(query) &&
        GeneralSearchHelper::IsAllAlNum(rewriteQueryVec[0])) {
        return true;
    }
    return false;
}

bool GeneralSearchHelper::SingleRewriteQueryNeedRecover(const std::string& query,
        std::vector<std::string>& singleRewriteQueryVec)
{
    // 如果改写Query为空, 则不需要重新发起请求
    if (singleRewriteQueryVec.size() == 0) {
        return true;
    }

    // 当改写Query为空 或 改写与原Query相同时, 则不需要重新发起请求
    if ((singleRewriteQueryVec.size() == 1) &&
        (GeneralSearchHelper::IsEmpty(singleRewriteQueryVec[0]) ||
         singleRewriteQueryVec[0] == query)) {
        return true;
    }

    // 因摘要暂时无法支持多Query的摘要
    // 暂时关闭切分成多Query的查询
    if (singleRewriteQueryVec.size() > 1) {
        return true;
    }

    // 当原始Query为中文，但改写Query全部为英文和数字，则不需要重新发起请求
    if (GeneralSearchHelper::HasChinese(query) &&
        GeneralSearchHelper::IsAllAlNum(singleRewriteQueryVec[0])) {
        return true;
    }
    return false;
}

bool GeneralSearchHelper::IsAllAlNum(const std::string& content)
{
    size_t length = 0;
    for (size_t i = 0; i < content.size(); ++i) {
        if (GeneralSearchHelper::IsAlNum(content[i])) {
            length += 1;
        }
    }
    return length == content.size();
}

bool GeneralSearchHelper::IsEmpty(const std::string& content)
{
    return content.size() == 0;
}

bool GeneralSearchHelper::HasChinese(const std::string& content)
{
    for (size_t i = 0; i < content.size(); ++i) {
        if (GeneralSearchHelper::IsChineseChar(content[i])) {
            return true;
        }
    }
    return false;
}

bool GeneralSearchHelper::IsChineseChar(const char& ch)
{
    return (ch & 0x80);
}

bool GeneralSearchHelper::IsAlNum(const char& ch)
{
    return ((ch >= '0' && ch <= '9') ||
        (ch >= 'a' && ch <= 'z') ||
        (ch >= 'A' && ch <= 'Z'));
}


std::string GeneralSearchHelper::GetFormatTime(int Currenttime, const std::string& mode)
{
    if (Currenttime < 1600000000) {
        return std::string("");
    }

    char outStr[256];
    time_t t = Currenttime;
    struct tm *tmp = localtime(&t);
    if (mode=="only_year"){
        strftime(outStr, sizeof(outStr), "%Y", tmp);
    } else if(mode=="only_chinese"){
        strftime(outStr, sizeof(outStr), "%Y年%m月%d号", tmp);
    }else {
        strftime(outStr, sizeof(outStr), "%Y-%m-%d", tmp);
    }
    return std::string(outStr);
}

void GeneralSearchHelper::WeatherFormat(std::string& query, std::string& extra_refer){
    // 1.Query信息追加具体日期
    std::string query_with_date;
    std::vector<std::string> specific_list;
    if (query.find("昨天") != std::string::npos) {
        specific_list = {GeneralSearchHelper::GetFormatTime(TimeHelper::GetSystemCurrentTimeSec() - 24 * 3600, "default")};
        query_with_date = StringTool::ReplaceStr(query, "昨天", specific_list[0]);
    }
    if (query.find("今天") != std::string::npos) {
        specific_list = {GeneralSearchHelper::GetFormatTime(TimeHelper::GetSystemCurrentTimeSec(), "default")};
        query_with_date = StringTool::ReplaceStr(query, "今天", specific_list[0]);
    }
    if (query.find("明天") != std::string::npos) {
        specific_list = {GeneralSearchHelper::GetFormatTime(TimeHelper::GetSystemCurrentTimeSec() + 24 * 3600, "default")};
        query_with_date = StringTool::ReplaceStr(query, "明天", specific_list[0]);
    }
    if (query.find("大后天") == std::string::npos && query.find("后天") != std::string::npos) {
        specific_list = {GeneralSearchHelper::GetFormatTime(TimeHelper::GetSystemCurrentTimeSec() + 48 * 3600, "default")};
        query_with_date = StringTool::ReplaceStr(query, "后天", specific_list[0]);
    }
    if (query.find("大后天") != std::string::npos) {
        specific_list = {GeneralSearchHelper::GetFormatTime(TimeHelper::GetSystemCurrentTimeSec() + 72 * 3600, "default")};
        query_with_date = StringTool::ReplaceStr(query, "大后天", specific_list[0]);
    }
    if (query.find("下周") != std::string::npos) {
        specific_list = {};
        // [zisen add] tmp_diff用于保存当前跟下周一相差的天数
        time_t tmp_now_time = TimeHelper::GetSystemCurrentTimeSec();
        struct tm *tmp_now_day = localtime(&tmp_now_time);
        int tmp_diff = 7 - (tmp_now_day->tm_wday + 6) % 7;
        for (int idx : {0,1,2,3,4,5,6}){
            specific_list.push_back(GeneralSearchHelper::GetFormatTime(
                    TimeHelper::GetSystemCurrentTimeSec()+ ((idx+tmp_diff)*24*3600), "default"));
        }
        if (!specific_list.empty())
            query_with_date = StringTool::ReplaceStr(query, "下周",
                specific_list[0] + "到" + specific_list[specific_list.size()-1]);
    }

    //[zisen add] 若日期列表未命中上述规则，则补充从今天开始的7天
    if (specific_list.empty()){
        specific_list = {};
        for (int idx : {0,1,2,3,4,5,6}){
            specific_list.push_back(GeneralSearchHelper::GetFormatTime(
                    TimeHelper::GetSystemCurrentTimeSec()+ (idx*24*3600), "default"));
        }
    }

    if (! specific_list.empty() && !query_with_date.empty() && !specific_list[0].empty()) {
        query = query_with_date;
    }

    // 2.补充一个时间表作为参考材料
    // stringstream time;
    // time << "前天是" <<  the_day_before_yesterday << "，昨天是" << yesterday
    //     << "，今天是" << today << "，明天是" << tomorrow  << "，后天是" << the_day_after_tomorrow << "。\n";
    // extra_refer = time.str();
    WeatherReferFormat(extra_refer, specific_list);
    // AGG_LOG(ERROR, "GetAggWebResult extra_refer=%s", extra_refer.c_str());
}

void GeneralSearchHelper::WeatherReferFormat(std::string& originRefer,
                                            const std::vector<std::string>& specificList) {
    std::vector<std::string> newReferVec;
    std::string newRefer;
    std::vector<std::string> referVec = autil::StringUtil::split(originRefer, "\n");
    std::string curYear = GeneralSearchHelper::GetFormatTime(TimeHelper::GetSystemCurrentTimeSec(), "only_year");
    for (std::string line : referVec){
        if (autil::StringUtil::startsWith(line, curYear)){
            // "日期"开头，仅保留指定日期的天气数据
            for (std::string specific_date : specificList) {
                // AGG_LOG(ERROR, "WeatherReferFormat=%d, mReferInfo=%d, mReferInfo=%s, mReferInfo=%s",
                //         newReferVec.size(), specificList.size(), curYear.c_str(), specific_date.c_str());
                if(autil::StringUtil::startsWith(line, specific_date))
                    newReferVec.push_back(line);
            }
        }
    }
    for (const std::string& line : newReferVec){
        newRefer += line + "\n";
    }
    if (!newRefer.empty()) originRefer = newRefer;
    return;
}

bool GeneralSearchHelper::GetTicketsMoney(const std::string& origin_number, std::string& film_tickets_money) {
    // 1.有效的科学计数法
    double film_ticket = 0;
    SAFE_TYPE_CHANGE(origin_number, film_ticket);
    if (film_ticket < 100) {
        return false;
    }

    film_ticket = film_ticket / static_cast<double>(100);   // 换算成"元"
    if (film_ticket < 0.1) return false;

    // 2.将数字换算成金钱
    double prefix_number = 0.0;
    std::string postfix_str = "元";
    if (film_ticket > 100000000){ // 1亿
        prefix_number = static_cast<double>(film_ticket) / static_cast<double>(100000000);
        postfix_str = "亿元";
    }else if(film_ticket > 10000000) { // 1千万
        prefix_number = static_cast<double>(film_ticket) / static_cast<double>(10000000);
        postfix_str = "千万元";
    }else if(film_ticket > 10000){ // 1万
        prefix_number = static_cast<double>(film_ticket) / static_cast<double>(10000);
        postfix_str = "万元";
    }else{  // 小于1W
        prefix_number = static_cast<double>(film_ticket);
        postfix_str = "元";
    }
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << prefix_number;
    film_tickets_money = stream.str() + postfix_str;
    return true;
}

void GeneralSearchHelper::FilmFormat(std::string& query, std::string& extra_refer) {
    // 1.修改Prompt: Query信息

    // 2.修改Prompt: Doc信息
    std::string newRefer;
    std::vector<std::string> referVec = autil::StringUtil::split(extra_refer, "\n");
    for (std::string& line : referVec){
        if (autil::StringUtil::startsWith(line, "票房累计金额")){
            // 科学计数法转换
            std::vector<std::string> tickets = autil::StringUtil::split(line, ": ");
            if(tickets.size() == 2){
                std::string film_tickets_money;
                GeneralSearchHelper::GetTicketsMoney(tickets[1], film_tickets_money);
                line = tickets[0] + ": " + film_tickets_money;
                // AGG_LOG(ERROR, "FilmFormat, 票房=%s", line.c_str());
            }
        }
    }
    for (std::string line : referVec) {
        newRefer += line + "\n";
    }
    extra_refer = newRefer;
    // AGG_LOG(ERROR, "FilmFormat=%d, FilmFormat=%d, FilmFormat=%s", referVec.size(), newRefer.size(), newRefer.c_str());
    return;
}

bool GeneralSearchHelper::CheckSimilarity(const std::string& query, const std::string& latestQuery)
{
    if (query == latestQuery) {
        return true;
    }
    std::vector<std::string> vec1 = StringTool::ToUtf8(query);
    std::vector<std::string> vec2 = StringTool::ToUtf8(latestQuery);
    int seqlen = 3 * 5; // 相差应该在5个汉字内
    if (vec1.size() + seqlen < vec2.size()
            || vec2.size() + seqlen < vec1.size()
            || vec1.size() > 50
            || vec2.size() > 50
            || vec1.size() < 3
            || vec2.size() < 3) {
        return false;
    }

    std::vector<std::vector<int>> dp(vec1.size() + 1, std::vector<int>(vec2.size() + 1, 0));
    for (size_t i = 1; i <= vec1.size(); ++i) {
        for (size_t j = 1; j <= vec2.size(); ++j) {
            if (vec1[i - 1] == vec2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    size_t maxlen = dp[vec1.size()][vec2.size()];
    bool sim = maxlen + 3 > vec1.size() && maxlen + 3 > vec2.size(); // 两个字
    AGG_LOG(ERROR, "query=%s, latestQuery=%s, maxlen=%lu, sim=%d", query.c_str(), latestQuery.c_str(), maxlen, sim);
    return sim;
}

void GeneralSearchHelper::LStripStrInPlace(std::string& in, const std::vector<std::string>& syms)
{
    bool found = true;
    while (found && ! in.empty()) {
        found = false;
        for (const auto& x: syms) {
            if (autil::StringUtil::startsWith(in, x)) {
                in = in.erase(0, x.size());
                found = true;
            }
        }
    }
}

void GeneralSearchHelper::RStripStrInPlace(std::string& in, const std::vector<std::string>& syms)
{
    bool found = true;
    while (found && ! in.empty()) {
        found = false;
        for (const auto& x: syms) {
            if (autil::StringUtil::endsWith(in, x)) {
                in.resize(in.size() - x.size());
                found = true;
            }
        }
    }
}

AGG_END_NAMESPACE(agg_web2);

