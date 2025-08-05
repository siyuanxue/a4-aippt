#include <agg_web2/upstream/web_agg/aggweb_result.h>
#include <messer_utils/util/ostringstream.h>
#include <autil/TimeUtility.h>
#include <autil/StringUtil.h>
#include <algorithm>
#include <agg2/util/string_tool.h>
#include <iomanip>
#include <cctype>
#include <agg_web2/data/sc_template_data.h>
#include <agg2/wrapper/agg_data_wrapper.h>
#include <agg2/helper/time_helper.h>
#include <agg_web2/helper/general_search_helper.h>
#include <agg_web2/data/key_value_data.h>
#include <agg_web2/data/value_data.h>
#include <re2/re2.h>

AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2, AggWebResult);

using namespace std;

BEGIN_DEFINE_SERIALIZATION_FUNC(AggWebResult){
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Result);
} END_DEFINE_SERIALIZATION_FUNC(AggWebResult)

const double MAX_REL_SCORE = 2;
const double MAX_PR_SCORE = 1;

#define CHECK_FINDFIRSTNODE_UNRET_IMPL(rootElement, nodeName, nodeElement)     \
    if (!TiXmlHelper::FindFirstChildNode(rootElement, nodeName, nodeElement)) \
    {                                                                   \
        std::string nodeNameStr = nodeName;                             \
        AGG_LOG(DEBUG, "Can not find node <%s>", nodeNameStr.c_str());  \
    }                                                                   \

AggWebResult::AggWebResult() : mDebugInfoPtr(new LLMDebugInfo)
{
}

AggWebResult::AggWebResult(LLMDebugInfoPtr debugInfoPtr, bool isOffline, bool isAnquan)
    : mDebugInfoPtr(debugInfoPtr), mIsOffline(isOffline), mIsAnquanMode(isAnquan)
{
}

AggWebResult::~AggWebResult()
{
}

bool AggWebResult::Serialize(std::string & bytes)
{
    return true;
}

std::string GetTextFromXML(TiXmlElement * node)
{
    if (node && node->GetText()) {
        return node->GetText();
    }
    return std::string("");
}

void GetStableValue(const std::string& line, const std::string& key, std::string& val)
{
    std::string tmp = key + ": \"";
    size_t pos = line.find(tmp);
    if (pos != std::string::npos) {
        int start = pos + tmp.size();
        val = line.substr(start, line.size() - start);
        val = StringTool::ReplaceStr(val, "\\t", "", false);
        val = StringTool::ReplaceStr(val, "\\n", "\n", false);
        if (! val.empty() && val.back() == '"') {
            val.pop_back();
        }
    }
}

void RemoveEM(std::string& content)
{
    content = StringTool::ReplaceStr(content, "<em>", "", false);
    content = StringTool::ReplaceStr(content, "</em>", "", false);
    content = StringTool::ReplaceStr(content, "\t", "", false);
}

std::string GetQAValue(const std::string& line, const std::string& key)
{
    std::string tmp = "] " + key + "=[";
    size_t st = line.find(tmp);
    size_t ed = line.find("]", st + tmp.size());
    if (st != std::string::npos && ed != std::string::npos){
        return line.substr(st + tmp.size(), ed - st - tmp.size());
    }
    return std::string("");
}

bool CheckValidDoc(const std::string& RawBody)
{
    // 1.过滤新闻、小说; 长度小于10，占比高于70%
    std::vector<std::string> lines = autil::StringUtil::split(RawBody, "\n");
    int count_short_line = 0;
    for (std::string& line : lines) {
        if (line.size() <= 10) count_short_line+=1;
    }
    if (float(count_short_line) / float(lines.size()) >= 0.7) return false;

    // 2.过滤汉字占比低于70%
    int char_num = 0;
    int char_chinese_num = 0;

    size_t i = 0;
    while (i < RawBody.size()) {
        char c = RawBody[i];
        if ((c & 240) == 224) {
            char_chinese_num += 1;
            i += 3;
        } else if ((c & 224) == 192) {
            i += 2;
        } else {
            i += 1;
        }
        char_num += 1;
    }
    if(float(char_chinese_num) / float(char_num) <= 0.6) return false;
    return true;
}

std::string GetFormatTime(int Currenttime)
{
    if (Currenttime < 1600000000) {
        return std::string("");
    }

    char outStr[256];
    time_t t = Currenttime;
    struct tm *tmp = localtime(&t);
    strftime(outStr, sizeof(outStr), "%Y-%m-%d %H:%M:%S", tmp);
    return std::string(outStr);
}
std::string GetFormatTime(const std::string& CurrenttimeStr)
{
    int Currenttime = 0;
    SAFE_TYPE_CHANGE(CurrenttimeStr, Currenttime);
    return GetFormatTime(Currenttime);
}

bool AggWebResult::GetPathValue(TiXmlElement *element, const std::vector<std::string> & path, std::string& res)
{
    for (const auto& key: path) {
        TiXmlElement * ele;
        CHECK_FINDFIRSTNODE_RET(element, key, ele);
        if (ele == NULL) {
            AGG_LOG(ERROR, "GetAggWebResult error, key=%s", key.c_str());
            return false;
        }
        element = ele;
    }
    res = GetTextFromXML(element);
    return ! res.empty();
}

TiXmlElement * AggWebResult::GetPathNode(TiXmlElement *element, const std::vector<std::string> & path)
{
    for (const auto& key: path) {
        TiXmlElement * ele;
        CHECK_FINDFIRSTNODE_RET(element, key, ele);
        if (ele == NULL) {
            AGG_LOG(ERROR, "GetAggWebResult error, key=%s", key.c_str());
            return NULL;
        }
        element = ele;
    }
    return element;
}


bool AggWebResult::ParseScByConfig(TiXmlElement * root, const std::string& name,
        const std::string& query, int idx)
{
    if (name.empty()) {
        AGG_LOG(DEBUG, "Can not find sc name. name=%s", name.c_str());
        return false;
    }
    AGG_LOG(DEBUG, "try to call ParseScByConfig. name=%s", name.c_str());

    // 1.触发SC标识
    if (name == "yingshi_piaofang") isFilmSC = true;

    // 2.解析绝影配置
    AggDataManager * aggDataManager = AggDataManager::GetInstance();
    AggDataWrapper aggDataWrapper(aggDataManager, "sc_template_data");
    ScTemplateData * templateData = aggDataWrapper.Get<ScTemplateData>();

    std::string scStype;
    GetPathValue(root, {"hits", "hit", "sc_stype"}, scStype);

    std::string key = name + '`' + scStype;
    bool status = templateData->ParseScXml(key, root, mScAnswer);
    RemoveEM(mScAnswer);

    AGG_LOG(DEBUG, "find sc name. name=%s, sc_stype=%s, status=%d, mScAnswer=%s",
            name.c_str(), scStype.c_str(), status, mScAnswer.c_str());

    if (status) {
        std::string tmp = templateData->GetQuery(key);
        auto tmpVec = autil::StringUtil::split(tmp, "/");
        std::string title;
        GetPathValue(root, tmpVec, title);
        RemoveEM(title);
        AGG_LOG(DEBUG, "find sc name. tmp=%s", title.c_str(), tmp.c_str());
        if (! title.empty() && title != query) {
            mScAnswer.clear();
        }
    }

    if (mScAnswer.find("{hit/") != std::string::npos) {
        mScAnswer.clear();
    }

    if (name == "yingshi_piaofang") {
        // 票房SC, 单个电影票房, 转化科学计数法
        GeneralSearchHelper::FilmFormat(mQuery, mScAnswer);
    }
    bool doc_add_success_flag = false; //[zisen add] 用于判断SC结果是否添加成功
    if (! mScAnswer.empty()) {
        std::string current = GetFormatTime(TimeHelper::GetSystemCurrentTimeSec());
        LLMDocPtr docPtr(new LLMDoc(mQuery, mScAnswer, name, idx, MAX_REL_SCORE,
                    MAX_REL_SCORE, MAX_PR_SCORE, current, true));
        // 特殊Case: 股票类型SC, hushen类型, 只保留SC内容  (case: A股多少点)
        if(name=="finance_stock_new" && scStype=="hushen") mDocPtrVec.clear();
        mDocPtrVec.emplace_back(docPtr);
        doc_add_success_flag = true; //[zisen add] 若mDocPtrVec中成功添加返回结果，则flag赋值为true
    }

    return templateData->CheckName(key) && doc_add_success_flag; //[zisen add] mDocPtrVec需要成功添加返回结果才判断SC绝影模板解析成功
}

bool AggWebResult::ParseWendaSelected(TiXmlElement * root, int idx)
{
    std::string scStype;
    GetPathValue(root, {"hits", "hit", "sc_stype"}, scStype);
    if (mIsOffline) {
        if (scStype == "qa_baike" || scStype == "abstract_qa") { // 精准问答
            AGG_LOG(ERROR, "wenda_selected scStype=%s filter; doc_idx=%d", scStype.c_str(), idx);
            return false;
        }
        std::string qa_step;
        GetPathValue(root, {"hits", "hit", "item", "qa_step"}, qa_step);
        std::string qa_high_authority;
        GetPathValue(root, {"hits", "hit", "item", "qa_high_authority"}, qa_high_authority);
        // if (qa_step == "1" || qa_high_authority=="1") { // 步骤类\权威站点，不出生成式
        if (qa_step == "1") {
            AGG_LOG(ERROR, "wenda_selected qa_step=%s filter; doc_idx=%d", qa_step.c_str(), idx);
            return false;
        }
    }

    // TODO 离线刷库：步骤、绝对权威等，不出生成式问答
    std::string title;
    std::string raw_body;
    std::string url;
    if (GetPathValue(root, {"hits", "hit", "item", "title"}, title)
            && GetPathValue(root, {"hits", "hit", "item", "raw_body"}, raw_body)
            && GetPathValue(root, {"hits", "hit", "item", "url"}, url)) {
        RemoveEM(title);
        BodyClean(raw_body);
        if (scStype == "qa_baike") {
            std::string short_answer;
            if (GetPathValue(root, {"hits", "hit", "item", "short_answer"}, short_answer)) {
                RemoveEM(short_answer);
                raw_body = short_answer + "。" + raw_body;
            }
        }
        std::string SS_PUBLISHDATE;
        if (GetPathValue(root, {"SS_PUBLISHDATE"}, SS_PUBLISHDATE)) {
            SS_PUBLISHDATE = GetFormatTime(SS_PUBLISHDATE);
        } else {
            SS_PUBLISHDATE = "";
        }
        LLMDocPtr docPtr(new LLMDoc(title, raw_body, "wenda_selected "+ url, idx, MAX_REL_SCORE,
                    MAX_REL_SCORE, MAX_PR_SCORE, SS_PUBLISHDATE, false));
        mDocPtrVec.emplace_back(docPtr);
        return true;
    }
    return false;
}

bool AggWebResult::ParseNewsSc(TiXmlElement * root, const std::string& name, int idx) {

    TiXmlElement * hitsElement;
    CHECK_FINDFIRSTNODE_RET(root, "hits", hitsElement);
    if (hitsElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult hitsElement error");
        return false;
    }

    TiXmlElement * hitElement;
    CHECK_FINDFIRSTNODE_RET(hitsElement, "hit", hitElement);
    if (hitElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult hitElement error");
        return false;
    }

    TiXmlElement * nodeElement;
    CHECK_FINDFIRSTNODE_RET(hitElement, "news_node", nodeElement);
    if (nodeElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult nodeElement error");
        return false;
    }

    if (nodeElement) {
        for(; nodeElement != nullptr; nodeElement = nodeElement->NextSiblingElement()) {
            //AGG_LOG(ERROR, "news[%s]", TiXmlHelper::Print(nodeElement).c_str());
            std::string title, mScAnswer, publish_time;
            if (GetPathValue(nodeElement, {"fields", "ORI_MAIN_BODY"}, mScAnswer)
                    && GetPathValue(nodeElement, {"fields", "title"}, title)
                    && GetPathValue(nodeElement, {"fields", "publish_time"}, publish_time)) {

                RemoveEM(title);
                RemoveEM(mScAnswer);
                BodyClean(mScAnswer);

                LLMDocPtr docPtr(new LLMDoc(title, mScAnswer, name, idx, MAX_REL_SCORE,
                    MAX_REL_SCORE, MAX_PR_SCORE, publish_time, true));
                mDocPtrVec.emplace_back(docPtr);

                if(mDocPtrVec.size()==3) break;

                AGG_LOG(DEBUG, "mScAnswer=%s", mScAnswer.c_str());
            }
        }
    }

    return mDocPtrVec.size()>0;
}

bool AggWebResult::ParseWeather(TiXmlElement * root, const std::string& name, int idx)
{
    TiXmlElement * hitsElement;
    CHECK_FINDFIRSTNODE_RET(root, "hits", hitsElement);
    if (hitsElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult hitsElement error");
        return false;
    }

    TiXmlElement * hitElement;
    CHECK_FINDFIRSTNODE_RET(hitsElement, "hit", hitElement);
    if (hitElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult hitElement error");
        return false;
    }

    std::string country;
    std::string province;
    std::string city;
    std::string temp;
    std::string weather;
    std::string tips;
    std::string wind_dir;
    std::string wind_level;
    std::string highest_temp;
    std::string lowest_temp;
    std::string aqi;
    std::string aqi_value;

    if (GetPathValue(hitElement, {"module", "country"}, country)
            && GetPathValue(hitElement, {"module", "province"}, province)
            && GetPathValue(hitElement, {"module", "city"}, city)
            && GetPathValue(hitElement, {"module", "realtime_data", "current", "temp"}, temp)
            && GetPathValue(hitElement, {"module", "realtime_data", "current", "weather"}, weather)
            && GetPathValue(hitElement, {"module", "realtime_data", "current", "tips"}, tips)
            && GetPathValue(hitElement, {"module", "realtime_data", "current", "wind_dir"}, wind_dir)
            && GetPathValue(hitElement, {"module", "realtime_data", "current", "wind_level"}, wind_level)
            && GetPathValue(hitElement, {"module", "airquality_data", "aqi"}, aqi)
            && GetPathValue(hitElement, {"module", "airquality_data", "aqi_value"}, aqi_value)
            && GetPathValue(hitElement, {"module", "weather_forecast_data", "highest_temp"}, highest_temp)
            && GetPathValue(hitElement, {"module", "weather_forecast_data", "lowest_temp"}, lowest_temp)) {
    } else {
        AGG_LOG(ERROR, "GetAggWebResult Name error");
        return false;
    }

    MESSER_UTILS_NS(util)::ostringstream oss;
    oss << " " << country << province << city << "现在气温" << temp << "℃，" << weather
        << "，" << wind_dir << wind_level << "级，今天气温" << lowest_temp << "~" << highest_temp
        << "℃，空气质量" << aqi << "，空气质量指数" << aqi_value << "。\n近几日天气信息如下：";

    auto daily = GetPathNode(hitElement, {"module", "weather_forecast_data", "daily"});
    if (daily == NULL) {
        AGG_LOG(ERROR, "weather_forecast_data daily error");
        return false;
    }

    for (TiXmlElement* itemElement = daily->FirstChildElement("item");
            itemElement != NULL; itemElement = itemElement->NextSiblingElement("item")) {

        std::string predict_date;
        std::string weather_day;
        std::string temp_low;
        std::string temp_high;
        std::string wind_dir_day;
        std::string wind_level_day;
        std::string wind_dir_night;
        std::string wind_level_night;
        std::string sun_rise, sun_down;

        if (GetPathValue(itemElement, {"predict_date"}, predict_date)
                && GetPathValue(itemElement, {"weather_day"}, weather_day)
                && GetPathValue(itemElement, {"temp_low"}, temp_low)
                && GetPathValue(itemElement, {"temp_high"}, temp_high)
                && GetPathValue(itemElement, {"wind_dir_day"}, wind_dir_day)
                && GetPathValue(itemElement, {"wind_level_day"}, wind_level_day)
                && GetPathValue(itemElement, {"wind_dir_night"}, wind_dir_night)
                && GetPathValue(itemElement, {"wind_level_night"}, wind_level_night)
                && GetPathValue(itemElement, {"sun_rise"}, sun_rise)
                && GetPathValue(itemElement, {"sun_down"}, sun_down)) {
            oss << "\n"<< predict_date << "：" << weather_day << "，" << temp_low << "~" << temp_high
                << "℃，白天" << wind_dir_day << wind_level_day << "级，夜晚" << wind_dir_night
                << wind_level_night << "级，" << "日出时间为" << sun_rise << "日落时间为" << sun_down << "。";
        }
    }

    mScAnswer = oss.str();

    std::string title = country + province + city + "天气";
    std::string current = GetFormatTime(TimeHelper::GetSystemCurrentTimeSec());
    // 天气SC, 通过"今天" "明天"关键词裁剪Prompt
    GeneralSearchHelper::WeatherFormat(mQuery, mScAnswer);
    // AGG_LOG(ERROR, "GetAggWebResult WeatherFormat=%s", mScAnswer);

    LLMDocPtr docPtr(new LLMDoc(title, mScAnswer, name, idx, MAX_REL_SCORE,
                    MAX_REL_SCORE, MAX_PR_SCORE, current, true));
    mDocPtrVec.emplace_back(docPtr);

    AGG_LOG(ERROR, "mScAnswer=%s", mScAnswer.c_str());

    return true;
}

bool AggWebResult::ParseFilmList(TiXmlElement * root, const std::string& name, int idx)
{
    TiXmlElement * hitsElement;
    CHECK_FINDFIRSTNODE_RET(root, "hits", hitsElement);
    if (hitsElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult hitsElement error");
        return false;
    }

    TiXmlElement * hitElement;
    CHECK_FINDFIRSTNODE_RET(hitsElement, "hit", hitElement);
    if (hitElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult hitElement error");
        return false;
    }

    MESSER_UTILS_NS(util)::ostringstream oss;
    oss << "电影票房榜单如下：\n";

    AGG_LOG(DEBUG, "ParseFilmList=%s", oss.str().c_str());

    int index = 1;
    for (TiXmlElement* itemElement = hitElement->FirstChildElement("item");
            itemElement != NULL; itemElement = itemElement->NextSiblingElement("item")) {

        std::string show_name;  // 片名
        std::string end_date;   // 票房统计截止日期
        std::string start_date; // 上映日期
        std::string film_tickets; // 票房累计金额, 科学计数法
        std::string film_tickets_trans; // 票房累计金额, 33.44亿
        std::string film_type;  // 类型
        std::string film_area;  // 制片国家/地区

        if (GetPathValue(itemElement, {"show_name"}, show_name)
                && GetPathValue(itemElement, {"bxo_td"}, film_tickets)
                && GetPathValue(itemElement, {"show_date"}, end_date)
                && GetPathValue(itemElement, {"open_day"}, start_date)
                && GetPathValue(itemElement, {"show_type"}, film_type)
                && GetPathValue(itemElement, {"produce_country"}, film_area)) {

            if(!GeneralSearchHelper::GetTicketsMoney(film_tickets, film_tickets_trans)) film_tickets_trans = "统计中...";
            oss << "第" << index << "名： " << "电影名称：《" << show_name << "》，票房累计金额：" << film_tickets_trans
                << "，上映日期：" << start_date << "，票房统计截止日期："<< end_date << "，电影类型：" << film_type
                << "，制片国家/地区：" << film_area << "。\n";
            index += 1;

        }
        AGG_LOG(DEBUG, "ParseFilmList=%s", oss.str().c_str());
    }

    mScAnswer = oss.str();

    std::string title = "电影票房的榜单";
    std::string current = GetFormatTime(TimeHelper::GetSystemCurrentTimeSec());
    LLMDocPtr docPtr(new LLMDoc(title, mScAnswer, name, idx, MAX_REL_SCORE,
                    MAX_REL_SCORE, MAX_PR_SCORE, current, true));
    mDocPtrVec.emplace_back(docPtr);

    AGG_LOG(ERROR, "mScAnswer=%s", mScAnswer.c_str());

    return true;
}

bool AggWebResult::ParseCustomerPhone(TiXmlElement * root, const std::string& name, int idx)
{
    TiXmlElement * hitsElement;
    CHECK_FINDFIRSTNODE_RET(root, "hits", hitsElement);
    if (hitsElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult hitsElement error");
        return false;
    }

    TiXmlElement * hitElement;
    CHECK_FINDFIRSTNODE_RET(hitsElement, "hit", hitElement);
    if (hitElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult hitElement error");
        return false;
    }

    TiXmlElement * itemElement;
    CHECK_FINDFIRSTNODE_RET(hitElement, "item", itemElement);
    if (itemElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult itemElement error");
        return false;
    }

    MESSER_UTILS_NS(util)::ostringstream oss;
    oss << "尊敬的客户，您好！\n";

    AGG_LOG(ERROR, "ParseCustomerPhone=%s", oss.str().c_str());

    TiXmlElement * dataElement;
    CHECK_FINDFIRSTNODE_RET(itemElement, "data", dataElement);
    if (dataElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult dataElement error");
        return false;
    }

    for (TiXmlElement* subItemElement = dataElement->FirstChildElement("sub_item");
            subItemElement != NULL; subItemElement = subItemElement->NextSiblingElement("sub_item")) {
        std::string phone_title;  // 服务类型
        std::string phone_number;   // 电话
        if (GetPathValue(subItemElement, {"phone_title"}, phone_title)
                && GetPathValue(subItemElement, {"phone_number"}, phone_number)) {
            oss << phone_title << "：" << phone_number << "\n";
        }
        AGG_LOG(ERROR, "ParseCustomerPhone=%s", oss.str().c_str());
    }
    std::string province;
    std::string city;
    if (GetPathValue(itemElement, {"province"}, province) && GetPathValue(itemElement, {"city"}, city)){
        if (province.find("全国") != std::string::npos || city.find("全国") != std::string::npos) {
            oss << "以上电话热线适用于全国地区。";
        } else {
            oss << "以上电话热线适用于" << province << "省" << city << "市。";
        }
    }
    oss << "欢迎致电，我们将竭诚为您服务！";
    AGG_LOG(ERROR, "ParseCustomerPhone=%s", oss.str().c_str());
    mScAnswer = oss.str();

    std::string title;
    GetPathValue(itemElement, {"title"}, title);

    std::string current = GetFormatTime(TimeHelper::GetSystemCurrentTimeSec());
    LLMDocPtr docPtr(new LLMDoc(title, mScAnswer, name, idx, MAX_REL_SCORE,
                    MAX_REL_SCORE, MAX_PR_SCORE, current, true));
    mDocPtrVec.emplace_back(docPtr);

    AGG_LOG(ERROR, "mScAnswer=%s", mScAnswer.c_str());

    return true;
}

void AggWebResult::GetQuery(TiXmlElement * rootElement)
{
    GetPathValue(rootElement, {"agg_request_param", "query"}, mQuery);
    AGG_LOG(ERROR,  "hit mQuery=%s", mQuery.c_str());
    if (!mQuery.empty()) return;
    TiXmlElement * qpElement;
    CHECK_FINDFIRSTNODE_UNRET_IMPL(rootElement, "qp", qpElement);
    if (qpElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult qpElement error");
        return;
    }
    for (TiXmlElement * searchElement = qpElement->FirstChildElement("search");
            searchElement != NULL; searchElement = searchElement->NextSiblingElement("search")) {
        GetPathValue(searchElement, {"origin_query"}, mQuery);
        AGG_LOG(ERROR,  "hit mQuery=%s", mQuery.c_str());
        return;
    }
}

int AggWebResult::GetTimeliness(TiXmlElement * rootElement)
{
    std::string timeliness;
    GetPathValue(rootElement, {"qp","CUSTOM_GLOBAL_freshNew"}, timeliness);
    if (timeliness.size() > 0) {
        timeliness = timeliness.substr(0,1);
        int fresh = 0;
        SAFE_TYPE_CHANGE(timeliness, fresh);
        return fresh;
    }
    return 0;
}

void AggWebResult::SetOnlyScResult()
{
    for (size_t i = mDocPtrVec.size(); i > 0; --i) {
        auto docPtr = mDocPtrVec[i - 1];
        AGG_LOG(ERROR, "SetOnlyScResult [%lu] [%s]", i, docPtr->GetDebugString().c_str());
        if (! docPtr->mIsSc) {
            AGG_LOG(ERROR, "SetOnlyScResult erase [%lu] [%s]", i, docPtr->GetDebugString().c_str());
            mDocPtrVec.erase(mDocPtrVec.begin() + i - 1);
        }
    }
}

void AggWebResult::BodyClean(std::string& RawBody) {

    AggDataManager * aggDataManager = AggDataManager::GetInstance(); // 正则过滤
    AggDataWrapper aggDataWrapper(aggDataManager, "source_bodyclean_data");
    const auto& kvData = aggDataWrapper.Get<KeyValueData>()->GetkvData();

    MESSER_UTILS_NS(util)::ostringstream oss;
    std::vector<std::string> lines = autil::StringUtil::split(RawBody, "\n");
    for (auto& line: lines) {
        line = StringTool::ReplaceStr(line, "\u200b", " ");
        GeneralSearchHelper::LStripStrInPlace(line, {" ", "\t"});
        GeneralSearchHelper::RStripStrInPlace(line, {" ", "\t"});
        int choose = 1;
        AGG_LOG(DEBUG, "BodyClean line=%s", line.c_str());
        for(const auto& kv:kvData) {
            const std::string& pattern = kv.first;
            if (RE2::FullMatch(line, pattern)) {
                if(kv.second=="break") {
                    choose = -1;
                } else if(kv.second=="continue") {
                    choose = 0;
                }
                break;
            }
        }
        if (choose == -1) {
            break;
        }
        if (choose == 1) {
            oss << line << "\n";
        }
    }
    RawBody = oss.str();

    // 截断过长body
    int cutLen = 800 * 3;
    if (mIsOffline) {
        cutLen = 700 * 3;
    }
    if (mIsWenChuang) {
        cutLen = 5000 * 3;
    }
    std::string::size_type pos = RawBody.find('\n', cutLen);
    if (pos != std::string::npos) {
        RawBody = RawBody.substr(0, std::min(pos+1, (std::string::size_type) cutLen * 2));
    }
}

int AggWebResult::GetFirstScFilterStatus(const std::string& name)
{
    AggDataManager * aggDataManager = AggDataManager::GetInstance();
    AggDataWrapper aggDataWrapper(aggDataManager, "offline_sc_filter_data");
    auto kvData = aggDataWrapper.Get<KeyValueData>();
    if (kvData) {
        std::string value = kvData->GetValue(name);
        AGG_LOG(DEBUG, "GetFirstScFilterStatus sc=%s, value=%s", name.c_str(), value.c_str());
        if (value == "0") {
            return 0;
        } else if (value == "1") {
            return 1;
        }
    }
    return 2;
}

std::string AggWebResult::GetQueryIntent(TiXmlElement *rootElement)
{
    string MED_LABEL;
    if (GetPathValue(rootElement, {"general_info", "MED_LABEL"}, MED_LABEL)) {
        int value = 0;
        SAFE_TYPE_CHANGE(MED_LABEL, value);
        if (value >= 4) {
            return "medical";
        }
    }

    string intent;
    if (GetPathValue(rootElement, {"general_info", "quark_qu_domain_info_str"}, intent)) {
        AGG_LOG(DEBUG, "GetQueryIntent=%s", intent.c_str());
        Json::Value value;
        Json::Reader reader;
        try {
            if (!reader.parse(intent, value)) {
                AGG_LOG(ERROR, "parse json string failed [%s]", intent.c_str());
                return "";
            }
        } catch (...) {
            AGG_LOG(ERROR, "parse json string failed [%s]", intent.c_str());
            return "";
        }
        if (value.isArray() && value.size() > 0) {
            auto& meta = value[0];
            if (meta.isObject() && meta.isMember("domain") && meta["domain"].isArray()
                    && meta["domain"].size() > 0 && meta["domain"][0].isString()) {
                return meta["domain"][0].asString();
            }
        }
    }
    return "";
}

bool AggWebResult::ParseMakeupdata()
{
    AggDataManager * aggDataManager = AggDataManager::GetInstance();
    AggDataWrapper aggDataWrapper(aggDataManager, "query_makeup_data");
    auto kvData = aggDataWrapper.Get<KeyValueData>();
    if (kvData && mQuery.size() >0) {
        std::string value = kvData->GetValue(mQuery);
        if (value.size() > 0) {
            vector<string> kvs = autil::StringUtil::split(value.c_str(), "[SPLIT]", false);
            if (kvs.size() == 2) {
                LLMDocPtr docPtr(new LLMDoc(kvs[0], kvs[1], "makeup_data", 0, MAX_REL_SCORE,
                            MAX_REL_SCORE, MAX_PR_SCORE, "", true));
                mDocPtrVec.emplace_back(docPtr);
                return true;
            }
        }
    }
    return false;
}

//[zisen add]处理官网sc
bool AggWebResult::ParseOw(TiXmlElement * hitElement, int idx)
{
    //获取官网url和标题
    TiXmlElement * fieldElement;
    CHECK_FINDFIRSTNODE_UNRET_IMPL(hitElement, "fields", fieldElement);
    if (fieldElement == NULL) {
        AGG_LOG(ERROR, "fieldElement null, hitElement=%s", hitElement->GetText());
        return false;
    }
    TiXmlElement * stableElement;
    CHECK_FINDFIRSTNODE_UNRET_IMPL(fieldElement, "StableTable", stableElement);
    if (stableElement == NULL) {
        AGG_LOG(ERROR, "StableTable null, fieldElement=%s", fieldElement->GetText());
        return false;
    }
    std::string StableTable = GetTextFromXML(stableElement);
    std::string RawUrl;
    std::string Title;
    std::vector<std::string> lines = autil::StringUtil::split(StableTable, "\n");
    for (auto& line: lines) {
        GetStableValue(line, "RawUrl", RawUrl);
        GetStableValue(line, "Title", Title);
    }
    RemoveEM(Title);
    AGG_LOG(ERROR,"[zisen] url=%s,title=%s",RawUrl.c_str(),Title.c_str())
    //输出
    MESSER_UTILS_NS(util)::ostringstream oss;
    oss << "官网:" << RawUrl;
    mScAnswer = oss.str();
    AGG_LOG(ERROR, "[zisen]mScAnswer=%s", mScAnswer.c_str());
    std::string current = GetFormatTime(TimeHelper::GetSystemCurrentTimeSec());
    LLMDocPtr docPtr(new LLMDoc("官网", mScAnswer, "ow_flag", idx, MAX_REL_SCORE,
                MAX_REL_SCORE, MAX_PR_SCORE, current, true));
    mDocPtrVec.emplace_back(docPtr);
    return true;
}


//[zisen add]处理假期sc
bool AggWebResult::ParseHoliday(TiXmlElement * hitElement, const std::string& name, int idx)
{
    //获取假期安排
    std::string notice_group;
    std::string notice_info;
    GetPathValue(hitElement, {"hits", "hit", "data", "display","data","item","component","templateData","notice_group"}, notice_group);
    if (!notice_group.empty()) {
        Json::Value value;
        Json::Reader reader;
        try {
            if (!reader.parse(notice_group, value)) {
                AGG_LOG(ERROR, "parse json string failed [%s]", notice_group.c_str());
                return false;
            }
        } catch (...) {
            AGG_LOG(ERROR, "parse json string failed [%s]", notice_group.c_str());
            return false;
        }
        if (value.isArray() && value.size() > 0) {
            auto& meta = value[0];
            if (meta.isObject() && meta.isMember("notice_word") && meta["notice_word"].isString()) {
                notice_info = meta["notice_word"].asString();
            }
        }
    }
    if (notice_info.empty()){
        return false;
    }
    AGG_LOG(ERROR, "[zisen] notice info value:%s",notice_info.c_str())
    //输出
    MESSER_UTILS_NS(util)::ostringstream oss;
    oss << "为您找到最新的相关通知如下：\n" << notice_info;
    mScAnswer = oss.str();
    AGG_LOG(ERROR, "[zisen]mScAnswer=%s", mScAnswer.c_str());
    std::string current = GetFormatTime(TimeHelper::GetSystemCurrentTimeSec());
    LLMDocPtr docPtr(new LLMDoc("假期安排", mScAnswer, name, idx, MAX_REL_SCORE,
                MAX_REL_SCORE, MAX_PR_SCORE, current, true));
    mDocPtrVec.emplace_back(docPtr);
    return true;
}


bool AggWebResult::Deserialize(const std::string & bytes)
{
    // for debug
    //{
    //    std::vector<std::string> lines = autil::StringUtil::split(bytes, "\n");
    //    for (const auto& line: lines) {
    //        AGG_LOG(DEBUG, "XXX %s", line.c_str());
    //    }
    //}
    mBytes = bytes;
    AGG_LOG(DEBUG, "AGG_RESULT:[%s]", mBytes.c_str());

    TiXmlDocument doc;
    TiXmlElement *rootElement = NULL;
    doc.Parse(bytes.c_str(), 0);
    AGG_LOG(DEBUG, "bytes:[%s]", bytes.c_str());
    if (doc.Error() || (rootElement = doc.RootElement()) == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult root error");
        return false;
    }

    TiXmlElement * engineElement;
    CHECK_FINDFIRSTNODE_RET(rootElement, "engine", engineElement);
    if (engineElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult engine error");
        return false;
    }

    TiXmlElement * hitsElement;
    CHECK_FINDFIRSTNODE_RET(engineElement, "hits", hitsElement);
    if (hitsElement == NULL) {
        AGG_LOG(ERROR, "GetAggWebResult hits error");
        return false;
    }

    GetQuery(rootElement);

    if (mIsOffline) { // 刷库模式
        // filter_id过滤，敏感词不走生成式
        std::string filter_id;
        GetPathValue(rootElement, {"filter", "query_fi"}, filter_id);
        if (filter_id != "0") {
            mDebugInfoPtr->AddDebug("fi=" + filter_id + " filter");
            AGG_LOG(ERROR, "mIsOffline return, filter_id=%s", filter_id.c_str());
            return false;
        }
        // 固定组合，不走生成式
        std::vector<std::string> patterns = {"加盟", "图解", "带图", "chatgpt", "openai", "多少钱", "排行榜", "怎么"};
        for(std::string& item : patterns){
            if (mQuery.find(item) != std::string::npos) {
                AGG_LOG(ERROR, "mIsOffline return, patterns=%s", item.c_str());
                return false;
            }
        }

        string adult_level;
        if (GetPathValue(rootElement, {"general_info", "adult_level"}, adult_level)) {
            int level = 0;
            SAFE_TYPE_CHANGE(adult_level, level);
            AGG_LOG(ERROR, "mIsOffline adult_level=%s, level=%d", adult_level.c_str(), level);
            if (level > 0) {
                mDebugInfoPtr->AddDebug("adult_level=" + adult_level + " filter");
                return false;
            }
        }
    }

    AGG_LOG(ERROR, "check_process_start");
    
    // 获取qp请求串，保持和大搜一致
    GetPathValue(rootElement, {"searchqa_qrs_clause_mainindex"}, mQrsClause);

    mFresh = GetTimeliness(rootElement);
    AGG_LOG(ERROR, "fresh=%d", mFresh);
    mDomain = GetQueryIntent(rootElement);
    AGG_LOG(ERROR, "domain=%s", mDomain.c_str());

    bool needNoQaFilter = false;
    {
        std::string filter_id;
        GetPathValue(rootElement, {"filter", "query_fi"}, filter_id);
        if (filter_id != "0") { // 敏感词，过滤非QA
            needNoQaFilter = true;
        }

        if (mDomain == "novel" || mDomain == "film" || mDomain == "tv" || mDomain == "show") { //资源类行业
            needNoQaFilter = true;
        }

        string adult_level;
        if (GetPathValue(rootElement, {"general_info", "adult_level"}, adult_level)) {
            int level = 0;
            SAFE_TYPE_CHANGE(adult_level, level);
            if (level > 0) { // 色情
                needNoQaFilter = true;
            }
        }
    }

    // 问答意图判断
    if (bytes.find("<zg_qa_trigger><![CDATA[1]]></zg_qa_trigger>") != std::string::npos) {
        mIsQaTrigger = true;
    }

    if (mIsWenChuang) {
        return ParseWenChuang(hitsElement);
    }

    if (mIsGaoKao) {
        return ParseGaoKao(hitsElement);
    }

    if (ParseMakeupdata()) {
        mIsSCMode = true;
        mDebugInfoPtr->AddDebug("use makeup result");
        return true;
    }

    bool hasWenda = false;
    double maxPr = 0.0;
    bool has_pr = false;
    bool hasBaike = false;
    size_t idx;
    TiXmlElement * hitElement;
    for (idx=0, hitElement = hitsElement->FirstChildElement("hit");
            hitElement != NULL; idx+=1 , hitElement = hitElement->NextSiblingElement("hit")) {

        std::string name;
        TiXmlHelper::GetAttribute<std::string>(hitElement, "name", name);
        AGG_LOG(DEBUG, "hit name=%s; doc_idx=%d", name.c_str(), idx);
        //AGG_LOG(ERROR, "body[%s]", TiXmlHelper::Print(hitElement).c_str());

        //[zisen add]若触发SC，则可提早退出循环
        //if (mScPtrVec.size()>0 && idx >= 9){
        //    break;
        //}

        if (mIsAnquanMode && name == "news_uchq") {
            // 安全模型的news不能保证安全站，先进行过滤
            //continue;
        }

        //[zisen add]假期类SC触发逻辑
        bool hit_holiday_sc_flag = false;
        std::string holiday_sc_arr[] = {"operation_topic_22112", "operation_topic_35056", "operation_topic_35671", "operation_topic_26020", "operation_topic_22567", "operation_topic_22275", "operation_topic_32044", "operation_topic_22287", "operation_topic_22691", "operation_topic_22945", "operation_topic_22794", "operation_topic_22803", "operation_topic_22584", "operation_topic_26229", "operation_topic_22867", "operation_topic_22824", "operation_topic_35485", "operation_topic_22281", "operation_topic_22776", "operation_topic_21226", "operation_topic_20637", "operation_topic_35647", "operation_topic_35600", "operation_topic_23033", "operation_topic_35078", "operation_topic_22552", "operation_topic_35179", "operation_topic_22757", "operation_topic_22966", "operation_topic_22238", "operation_topic_22641", "operation_topic_22175", "operation_topic_35672", "operation_topic_22120", "operation_topic_22288", "operation_topic_22335", "operation_topic_22124", "operation_topic_22496", "operation_topic_22568", "operation_topic_22339", "operation_topic_22306", "operation_topic_22979", "operation_topic_22540", "operation_topic_22509", "operation_topic_22521"};
        for (std::string sc_item:holiday_sc_arr){
            if (name == sc_item){
                AGG_LOG(ERROR,"[zisen] hit holiday sc");
                hit_holiday_sc_flag = true;
                break;
            }
        }
        if (hit_holiday_sc_flag){
            if (ParseHoliday(hitElement,name,idx) && idx<3){
                mIsSCMode = true;
                return true;
                //mScPtrVec.emplace_back(mDocPtrVec.back());
            }
            continue;
        }

        //[zisen add]若触发官网sc则返回官网标题和网址
        std::string ow_flag;
        GetPathValue(hitElement, {"fields","ow_flag"}, ow_flag);
        AGG_LOG(DEBUG, "[zisen] 官网=%s->%d", ow_flag.c_str(),(ow_flag.compare("1")) );
        if (ow_flag.compare("1") == 0) {
            if ( ParseOw(hitElement,idx) && idx < 3 ){
                mIsSCMode = true;
                return true;
                //mScPtrVec.emplace_back(mDocPtrVec.back());
            }
            continue;
        }

        //问答SC
        if (name == "wenda_selected") {
            hasWenda = true;
            bool status = ParseWendaSelected(hitElement, idx);
            if (mIsOffline && ! status) { // 离线刷库模式
                return false;
            }
            //if (status) {
            //    mScPtrVec.emplace_back(mDocPtrVec.back());
            //}
            continue;
        }

        if (mIsOffline && ! name.empty()) { // 离线可以跳过其它的sc
            int status = GetFirstScFilterStatus(name);
            if (idx == 0 && status == 0) {
                mDebugInfoPtr->AddDebug(name + " filter");
                AGG_LOG(ERROR, "hit name=%s; doc_idx=%d offline skip", name.c_str(), idx);
                return false;
            }
            if (status == 1) {
                // 可加入，主要是百科类型
            } else {
                AGG_LOG(ERROR, "hit name=%s; doc_idx=%d offline skip", name.c_str(), idx);
                continue;
            }
        }

        if (idx < 3 && name == "weather_moji") { // 天气
            isWeatherSC = true;
            mIsSCMode = true;
            return ParseWeather(hitElement, name, idx);
            //if ( ParseWeather(hitElement, name, idx) ){
            //    mScPtrVec.emplace_back(mDocPtrVec.back());
            //    continue;
            //}
            //else {
            //    AGG_LOG(ERROR,"[zisen] 天气SC解析失败")
            //    return false;
            //}
        }

        if (idx < 3 && name == "news_uchq") { // 新闻
            isNewsSC = true;
            mIsSCMode = true;
            return ParseNewsSc(hitElement, name, idx);
            //if ( ParseNewsSc(hitElement, name, idx) ){
            //    mScPtrVec.emplace_back(mDocPtrVec.back());
            //    continue;
            //}
            //else {
            //    AGG_LOG(ERROR,"[zisen] 新闻SC解析失败")
            //    return false;
            //}
        }

        if (idx < 3 && name == "piaofang_list") { // 票房榜单, List类型
            mIsSCMode = true;
            return ParseFilmList(hitElement, name, idx);
            //if ( ParseFilmList(hitElement, name, idx) ){
            //    mScPtrVec.emplace_back(mDocPtrVec.back());
            //    continue;
            //}
            //else {
            //    AGG_LOG(ERROR,"[zisen] 票房榜单SC解析失败")
            //    return false;
            //}
        }

        if (idx < 3 && name == "customer_phone") { // 电话热线, List类型
            mIsSCMode = true;
            return ParseCustomerPhone(hitElement, name, idx);
            //if ( ParseCustomerPhone(hitElement, name, idx) ){
            //    mScPtrVec.emplace_back(mDocPtrVec.back());
            //    continue;
            //}
            //else {
            //    AGG_LOG(ERROR,"[zisen] 电话热线SC解析失败")
            //    return false;
            //}
        }

        // 限定top3位置，读取sc数据; 股票类限定为top 10
        if (idx < 3 || (idx < 9 && name == "finance_stock_new")) {
			bool status = ParseScByConfig(hitElement, name, mQuery, idx);
			if (status) {
                                mIsSCMode = true;
                                return status;
                                //mScPtrVec.emplace_back(mDocPtrVec.back());
                                //continue;
			}
        }

        // 泛时效性高，不出自然结果
        if (mFresh == 4) {
            continue;
        }

        // 不走自然结果
        if (mQuery.find("年龄") != std::string::npos
                || mQuery.find("天气") != std::string::npos
                || mQuery.find("股票") != std::string::npos
                || mQuery.find("股价") != std::string::npos
                || mQuery.find("市值") != std::string::npos) {
            continue;
        }

        // 2.1 获取url、body、title
        TiXmlElement * fieldElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(hitElement, "fields", fieldElement);
        if (fieldElement == NULL) {
            AGG_LOG(ERROR, "fieldElement null, hitElement=%s", hitElement->GetText());
            continue;
        }

        TiXmlElement * stableElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(fieldElement, "StableTable", stableElement);
        if (stableElement == NULL) {
            AGG_LOG(ERROR, "StableTable null, fieldElement=%s", fieldElement->GetText());
            continue;
        }
        std::string StableTable = GetTextFromXML(stableElement);
        AGG_LOG(DEBUG, "StableTable=%s", StableTable.c_str());

        // 解析URL、Title、Body数据
        std::string RawUrl;
        std::string RawBody;
        std::string Title;
        std::string timeliness_fresh_isvalid;
        std::vector<std::string> lines = autil::StringUtil::split(StableTable, "\n");
        for (auto& line: lines) {
            GetStableValue(line, "RawUrl", RawUrl);
            GetStableValue(line, "Title", Title);
            GetStableValue(line, "V_MAIN_BODY", RawBody);
            GetStableValue(line, "timeliness_fresh_isvalid", timeliness_fresh_isvalid);
        }
        RemoveEM(Title);
        RemoveEM(RawBody);
        // 解析站点name、logo数据
        std::string HostLogo;
        std::string HostName;
        GetPathValue(fieldElement, {"HostLogo"}, HostLogo);
        GetPathValue(fieldElement, {"HostName"}, HostName);

        if (mIsOffline) { // 刷库模式
            std::string risk_title;
            std::string risk_body;
            std::string risk_domain;
            GetPathValue(fieldElement, {"osr_TitleShield"}, risk_title);
            GetPathValue(fieldElement, {"osr_BodyShield"}, risk_body);
            GetPathValue(fieldElement, {"osr_IsOverseaDomain"}, risk_domain);

            // 风险标签过滤
            if (risk_title != "0" || risk_body != "0" || risk_domain != "0") {
                AGG_LOG(ERROR, "mIsOffline continue doc_idx=%d, RawUrl=%s, risk_title=%s, "
                        "risk_body=%s, risk_domain=%s",
                        idx, RawUrl.c_str(), risk_title.c_str(), risk_body.c_str(), risk_domain.c_str());
                continue;
            }
        }

        // host过滤
        AggDataManager * aggDataManager = AggDataManager::GetInstance();
        AggDataWrapper aggDataWrapper(aggDataManager, "host_filter_data");
        const std::vector<std::string>& hostblacklist = aggDataWrapper.Get<ValueData>()->GetValue();
        bool hit_black_host = false;
        for (auto& item : hostblacklist) {
            if (RawUrl.find(item) != std::string::npos) {
                hit_black_host = true;
                break;
            }
        }
        if (hit_black_host) {
            AGG_LOG(INFO, "DOC FILTER, doc_idx=%d, RawUrl=%s, hostblacklist filter", idx, RawUrl.c_str());
            continue;
        }
        // 视频页、列表页等过滤
        if (RawUrl.find("/video/") != std::string::npos) {
            AGG_LOG(INFO, "DOC FILTER, doc_idx=%d, RawUrl=%s, video filter", idx, RawUrl.c_str());
            continue;
        }

        std::string SS_PUBLISHDATE;
        if (GetPathValue(fieldElement, {"SS_PUBLISHDATE"}, SS_PUBLISHDATE)) {
            SS_PUBLISHDATE = GetFormatTime(SS_PUBLISHDATE);
        }

        // 2.2 获取QTC分数
        TiXmlElement * qtcElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(fieldElement, "QtcTeacherQtc", qtcElement);
        std::string qtcStr = GetTextFromXML(qtcElement);
        double qtc = 0;
        SAFE_TYPE_CHANGE(qtcStr, qtc);

        // qt
        TiXmlElement * qtElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(fieldElement, "QtcTeacherQt", qtElement);
        std::string qtStr = GetTextFromXML(qtElement);
        double qt = 0;
        SAFE_TYPE_CHANGE(qtStr, qt);

        // Pr分数
        TiXmlElement * prElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(fieldElement, "deepsum_pr", prElement);
        std::string prStr = GetTextFromXML(prElement);
        double pr = 0.0;
        if (prStr.empty()) {
            pr = 0.2; // default
        } else {
            SAFE_TYPE_CHANGE(prStr, pr);
            maxPr = std::max(maxPr, pr);
            has_pr = true;
        }

        bool validDoc = CheckValidDoc(RawBody);

        AGG_LOG(DEBUG, "RawUrl=%s, RawBody=%s, Title=%s, qtc=%lf, qt=%lf, pr=%lf validDoc=%d, mDocPtrVec=%lu",
                RawUrl.c_str(), RawBody.c_str(), Title.c_str(), qtc, qt, pr, validDoc, mDocPtrVec.size());

        // 2.3 保存数据
        // qtc过滤后5%的数据
        // 安全模型，例如涉政类query，需要返回大搜的安全结果，因此不做qtc限制
        if (! RawUrl.empty() && RawBody.size() >= size_t(200) && ! Title.empty() && (qtc >= 0.5 /*|| mIsAnquanMode*/)
                && validDoc && pr >= 0.1) {
            bool isSc = false;
            if (idx < 3 && (RawUrl.find("baike.com") != std::string::npos
                        || RawUrl.find("baike.sogou.com") != std::string::npos)) {
                hasBaike = true;
                isSc = true;
            }

            // 正文清洗
            BodyClean(RawBody);

            LLMDocPtr docPtr(new LLMDoc(Title, RawBody, RawUrl, idx, qtc, qt, pr,
                        SS_PUBLISHDATE, isSc, HostName, HostLogo));
            mDocPtrVec.emplace_back(docPtr);
            AGG_LOG(INFO, "append docPtr, mDocPtrVec=%lu", mDocPtrVec.size());
        } else {
            AGG_LOG(DEBUG, "idx=%lu, url=%s filter", idx, RawUrl.c_str());
        }
    }

    //[zisen add] SC优先级逻辑
    //if (mScPtrVec.size()>0){
    //    AGG_LOG(ERROR,"[zisen] sc prior dev:%d",mScPtrVec.size());
    //    mIsSCMode = true;
    //    mDocPtrVec.clear();
    //    std::string sc_prior_list[] = {"finance_stock_new"};
    //    for (std::string sc_item:sc_prior_list){
    //        for (auto docPtr: mScPtrVec) {
    //            if (docPtr->mUrl==sc_item){
    //                AGG_LOG(ERROR,"[zisen] sc in prior list:%s",sc_item.c_str());
    //                mDocPtrVec.emplace_back(docPtr);
    //                return true;
    //            }
    //        }
    //    }
    //    AGG_LOG(ERROR,"[zisen] sc not in prior list:%s",mScPtrVec[0]->mUrl.c_str());
    //    mDocPtrVec.emplace_back(mScPtrVec[0]);
    //    return true;
    //}

    // Pr截断
    mMaxPr = maxPr;
    AGG_LOG(DEBUG, "maxPr=%lf", maxPr);
    if (has_pr && maxPr < 0.35 && !mIsAnquanMode) {
        mDebugInfoPtr->AddDebug("maxpr=" + std::to_string(maxPr) + " filter");
        AGG_LOG(ERROR, "CLEAR_RESULT maxPr=%lf filter", maxPr);
        return false;
    }

    if (mIsOffline) {
        // 仅有单个候选Doc，提高qt、pr的筛选阈值
        if(mDocPtrVec.size()==1) {
            if(mDocPtrVec[0]->mQtc <= 1.06 || mDocPtrVec[0]->mPR <= 0.42) {  // 后10%的阈值
                mDocPtrVec.clear();
                AGG_LOG(ERROR, "CLEAR_RESULT single recall and with low qtc||pr");
                return false;
            }
        }

        // 命中黑名单行业，且已经出了精选问答，不再出生成式
        string queryIntent = GetQueryIntent(rootElement);
        AGG_LOG(ERROR, "queryIntent=%s", queryIntent.c_str());

        if (queryIntent == "medical") {
            AGG_LOG(ERROR, "CLEAR_RESULT mQuery=%s, queryIntent=%s filter", mQuery.c_str(), queryIntent.c_str());
            return false;
        }

        if (hasWenda) {
            // 法律、游戏、星座、占卜
            static const set<string> BlackQueryIntent = {
                "law",
                "game",
                "constellation",
                "folklore"
            };
            if (BlackQueryIntent.find(queryIntent) != BlackQueryIntent.end()) {
                AGG_LOG(ERROR, "CLEAR_RESULT queryIntent=%s filter", queryIntent.c_str());
                return false;
            }
            // 教育: 数学公式
            if (queryIntent == "edu"){
                std::vector<std::string> patterns = {"乘法","除法","加法", "减法", "根", "℃","公式", "速率", "矩阵", "数学",
                    "幂", "计算","方程","导数","泰勒","函数", "次方", "二进制"};
                for (std::string& pattern : patterns){
                    if (mQuery.find(pattern) != std::string::npos) {
                        AGG_LOG(ERROR, "CLEAR_RESULT queryIntent=%s filter", queryIntent.c_str());
                        return false;
                    }
                }
            }
        }
    }

    if (mIsAnquanMode) {
        // 安全模式不走重排，直接采用大搜的排序结果
        for (auto docPtr: mDocPtrVec) {
            docPtr->mRankScore = docPtr->mQtc * 10 + docPtr->mQt * 5 + docPtr->mPR * 20;
            if (docPtr->mAggRank < 3) {
                docPtr->mRankScore -= docPtr->mAggRank;
            } else { // top3外一般效果稍差些
                docPtr->mRankScore -= docPtr->mAggRank * 2.0;
            }
        }
        sort(mDocPtrVec.begin(), mDocPtrVec.end(), [](LLMDocPtr x, LLMDocPtr y) {
            return x->mRankScore > y->mRankScore;
        });

        return true;
    }

    // 问答意图判断
    if (mIsOffline) {
        if (bytes.find("<zg_qa_trigger><![CDATA[1]]></zg_qa_trigger>") == std::string::npos) {
            AGG_LOG(ERROR, "CLEAR_RESULT no wenda_trigger");
            mDebugInfoPtr->AddDebug("no qa_trigger");
            mDocPtrVec.clear();
            return false;
        }
        if (mDocPtrVec.size() < size_t(2)) {
            AGG_LOG(ERROR, "CLEAR_RESULT mDocPtrVec.size < 2");
            mDebugInfoPtr->AddDebug("mDocPtrVec.size < 2");
            mDocPtrVec.clear();
            return false;
        }
    } else if (mIsAnquanMode) {
        // 安全模型不走问答过滤
    } else {
        if (bytes.find("<zg_qa_trigger><![CDATA[1]]></zg_qa_trigger>") == std::string::npos && ! hasBaike) {
            AGG_LOG(ERROR, "CLEAR_RESULT no wenda_trigger");
            mDebugInfoPtr->AddDebug("no qa_trigger");
            // TODO：去掉问答过滤，试试效果
            if (needNoQaFilter) {
                mDocPtrVec.clear();
                return false;
            }
        }
    }

    // 拟合一个得分进行排序
    for (auto docPtr: mDocPtrVec) {
        AGG_LOG(INFO, "insert doc for title[%s] url[%s] with mQtc[%f] mQt[%f] mPR[%f]", docPtr->mTitle.c_str(), docPtr->mUrl.c_str(), docPtr->mQtc, docPtr->mQt, docPtr->mPR);
        docPtr->mRankScore = docPtr->mQtc * 10 + docPtr->mQt * 5 + docPtr->mPR * 20;
        if (docPtr->mAggRank < 3) {
            docPtr->mRankScore -= docPtr->mAggRank;
        } else { // top3外一般效果稍差些
            docPtr->mRankScore -= docPtr->mAggRank * 1.5;
        }
    }
    sort(mDocPtrVec.begin(), mDocPtrVec.end(), [](LLMDocPtr x, LLMDocPtr y) {
        return x->mRankScore > y->mRankScore;
    });

    AGG_LOG(ERROR, "mDocPtrVec=%lu, hasBaike=%d", mDocPtrVec.size(), hasBaike);

    return true;
}

void AggWebResult::SetWenChuangMode()
{
    mIsWenChuang = true;
}

void AggWebResult::SetGaoKaoMode()
{
    mIsGaoKao = true;
}

bool AggWebResult::ParseWenChuang(TiXmlElement * hitsElement)
{
    if (!hitsElement)
        return false;

    size_t idx;
    TiXmlElement * hitElement;
    for (idx=0, hitElement = hitsElement->FirstChildElement("hit");
            hitElement != NULL; idx+=1 , hitElement = hitElement->NextSiblingElement("hit")) {

        std::string name;
        TiXmlHelper::GetAttribute<std::string>(hitElement, "name", name);
        AGG_LOG(ERROR, "hit name=%s; doc_idx=%d", name.c_str(), idx);
        //AGG_LOG(ERROR, "body[%s]", TiXmlHelper::Print(hitElement).c_str());

        //问答SC
        if (name == "wenda_selected") {
            bool status = ParseWendaSelected(hitElement, idx);
            continue;
        }

        if (idx < 10 && name == "news_uchq") { // 新闻
            isNewsSC = true;
            mIsSCMode = true;
            if (idx < 3) {
                isNewsFlag = true;
            }
            ParseNewsSc(hitElement, name, idx);
            continue;
        }

        // 2.1 获取url、body、title
        TiXmlElement * fieldElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(hitElement, "fields", fieldElement);
        if (fieldElement == NULL) {
            AGG_LOG(ERROR, "fieldElement null, hitElement=%s", hitElement->GetText());
            continue;
        }

        TiXmlElement * stableElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(fieldElement, "StableTable", stableElement);
        if (stableElement == NULL) {
            AGG_LOG(ERROR, "StableTable null, fieldElement=%s", fieldElement->GetText());
            continue;
        }
        std::string StableTable = GetTextFromXML(stableElement);
        AGG_LOG(DEBUG, "StableTable=%s", StableTable.c_str());

        // 解析URL、Title、Body数据
        std::string RawUrl;
        std::string RawBody;
        std::string Title;
        std::string timeliness_fresh_isvalid;
        std::vector<std::string> lines = autil::StringUtil::split(StableTable, "\n");
        for (auto& line: lines) {
            GetStableValue(line, "RawUrl", RawUrl);
            GetStableValue(line, "Title", Title);
            GetStableValue(line, "V_MAIN_BODY", RawBody);
            GetStableValue(line, "timeliness_fresh_isvalid", timeliness_fresh_isvalid);
        }

        RemoveEM(Title);
        RemoveEM(RawBody);
        // 解析站点name、logo数据
        std::string HostLogo;
        std::string HostName;
        GetPathValue(fieldElement, {"HostLogo"}, HostLogo);
        GetPathValue(fieldElement, {"HostName"}, HostName);

        // 视频页、列表页等过滤
        if (RawUrl.find("/video/") != std::string::npos) {
            AGG_LOG(INFO, "DOC FILTER, doc_idx=%d, RawUrl=%s, video filter", idx, RawUrl.c_str());
            continue;
        }

        std::string SS_PUBLISHDATE;
        if (GetPathValue(fieldElement, {"SS_PUBLISHDATE"}, SS_PUBLISHDATE)) {
            SS_PUBLISHDATE = GetFormatTime(SS_PUBLISHDATE);
        }

        // 2.2 获取QTC分数
        TiXmlElement * qtcElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(fieldElement, "QtcTeacherQtc", qtcElement);
        std::string qtcStr = GetTextFromXML(qtcElement);
        double qtc = 0;
        SAFE_TYPE_CHANGE(qtcStr, qtc);

        // qt
        TiXmlElement * qtElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(fieldElement, "QtcTeacherQt", qtElement);
        std::string qtStr = GetTextFromXML(qtElement);
        double qt = 0;
        SAFE_TYPE_CHANGE(qtStr, qt);

        double pr = 0;

        // 正文清洗
        BodyClean(RawBody);

        if (/*qtc < 0.5 ||*/ RawBody.size() < 10*3) {
            continue;
        }

        LLMDocPtr docPtr(new LLMDoc(Title, RawBody, RawUrl, idx, qtc, qt, pr,
                    SS_PUBLISHDATE, false, HostName, HostLogo));
        mDocPtrVec.emplace_back(docPtr);
        AGG_LOG(INFO, "append docPtr, mDocPtrVec=%lu", mDocPtrVec.size());
    }

    return true;
}

bool AggWebResult::ParseGaoKao(TiXmlElement * hitsElement)
{
    if (!hitsElement)
        return false;

    size_t idx;
    TiXmlElement * hitElement;
    for (idx=0, hitElement = hitsElement->FirstChildElement("hit");
            hitElement != NULL; idx+=1 , hitElement = hitElement->NextSiblingElement("hit")) {

        std::string name;
        TiXmlHelper::GetAttribute<std::string>(hitElement, "name", name);
        AGG_LOG(ERROR, "hit name=%s; doc_idx=%d", name.c_str(), idx);
        //AGG_LOG(ERROR, "body[%s]", TiXmlHelper::Print(hitElement).c_str());

        //问答SC
        if (name == "wenda_selected") {
            bool status = ParseWendaSelected(hitElement, idx);
            continue;
        }

        if (idx < 10 && name == "news_uchq") { // 新闻
            isNewsSC = true;
            mIsSCMode = true;
            if (idx < 3) {
                isNewsFlag = true;
            }
            ParseNewsSc(hitElement, name, idx);
            continue;
        }

        if (name == "gaokao_university_ranking" || name == "lima_new_general_entity_major" || name == "general_entity_college_domestic" || name == "gaokao_schedule_sc" || name == "gaokao_zuowen_zhenti_list_new" || name == "gaokao_zuowen_zhenti_detail_new" || name == "gaokao_zhenti" || name == "major_group" || name == "gaokao_major_ranking" || name == "gaokao_score_and_rank" || name == "gaokao_xuanzhiyuan" || name == "general_entity_college_information") {
            mHitGaokoaSC = true;
        }

        // 限定top10位置，读取baike_sc数据
        if (idx < 10 && name == "baike_sc" ) {
            /* TiXmlPrinter printer; */ 
            /* hitElement->Accept(&printer); */
            /* string hitElementString = printer.CStr(); */
            /* AGG_LOG(ERROR, "hit baike_sc, hitElement=%s", hitElementString.c_str()); */

            bool status = ParseScByConfig(hitElement, name, mQuery, idx);
            /* if (status) { */
                /* mIsSCMode = true; */
                /* return status; */
                //mScPtrVec.emplace_back(mDocPtrVec.back());
                //continue;
            /* } */
            continue;
        }

        // 2.1 获取url、body、title
        TiXmlElement * fieldElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(hitElement, "fields", fieldElement);
        if (fieldElement == NULL) {
            AGG_LOG(ERROR, "fieldElement null, hitElement=%s", hitElement->GetText());
            continue;
        }

        TiXmlElement * stableElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(fieldElement, "StableTable", stableElement);
        if (stableElement == NULL) {
            AGG_LOG(ERROR, "StableTable null, fieldElement=%s", fieldElement->GetText());
            continue;
        }
        std::string StableTable = GetTextFromXML(stableElement);
        AGG_LOG(DEBUG, "StableTable=%s", StableTable.c_str());

        // 解析URL、Title、Body数据
        std::string RawUrl;
        std::string RawBody;
        std::string Title;
        std::string timeliness_fresh_isvalid;
        std::vector<std::string> lines = autil::StringUtil::split(StableTable, "\n");
        for (auto& line: lines) {
            GetStableValue(line, "RawUrl", RawUrl);
            GetStableValue(line, "Title", Title);
            GetStableValue(line, "V_MAIN_BODY", RawBody);
            GetStableValue(line, "timeliness_fresh_isvalid", timeliness_fresh_isvalid);
        }

        RemoveEM(Title);
        RemoveEM(RawBody);
        // 解析站点name、logo数据
        std::string HostLogo;
        std::string HostName;
        GetPathValue(fieldElement, {"HostLogo"}, HostLogo);
        GetPathValue(fieldElement, {"HostName"}, HostName);

        // 视频页、列表页等过滤
        if (RawUrl.find("/video/") != std::string::npos) {
            AGG_LOG(INFO, "DOC FILTER, doc_idx=%d, RawUrl=%s, video filter", idx, RawUrl.c_str());
            continue;
        }

        std::string SS_PUBLISHDATE;
        if (GetPathValue(fieldElement, {"SS_PUBLISHDATE"}, SS_PUBLISHDATE)) {
            SS_PUBLISHDATE = GetFormatTime(SS_PUBLISHDATE);
        }

        // 2.2 获取QTC分数
        TiXmlElement * qtcElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(fieldElement, "QtcTeacherQtc", qtcElement);
        std::string qtcStr = GetTextFromXML(qtcElement);
        double qtc = 0;
        SAFE_TYPE_CHANGE(qtcStr, qtc);

        // qt
        TiXmlElement * qtElement;
        CHECK_FINDFIRSTNODE_UNRET_IMPL(fieldElement, "QtcTeacherQt", qtElement);
        std::string qtStr = GetTextFromXML(qtElement);
        double qt = 0;
        SAFE_TYPE_CHANGE(qtStr, qt);

        double pr = 0;

        // 正文清洗
        BodyClean(RawBody);

        if (/*qtc < 0.5 ||*/ RawBody.size() < 10*3) {
            continue;
        }

        if (name.size() > 0) {
            RawUrl = name + " " + RawUrl;
            AGG_LOG(INFO, "modify url=%s", RawUrl.c_str());
        }

        LLMDocPtr docPtr(new LLMDoc(Title, RawBody, RawUrl, idx, qtc, qt, pr,
                    SS_PUBLISHDATE, false, HostName, HostLogo));
        mDocPtrVec.emplace_back(docPtr);
        AGG_LOG(INFO, "append docPtr, mDocPtrVec=%lu", mDocPtrVec.size());
    }

    return true;
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(agg_web2)::AggWebResult);

