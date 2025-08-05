#include <fstream>
#include <agg_web2/data/rewrite_query_pool_data.h>
#include <agg2/util/string_tool.h>
#include <agg2/util/url_util.h>
#include <autil/StringUtil.h>

AGG_BEGIN_NAMESPACE(data);
AGG_LOG_SETUP(agg_web2, RewriteQueryPoolData);
using namespace std;


bool RewriteQueryPoolData::Init(const std::string &input)
{
    mQuerys.clear();
    std::ifstream fstream(input);
    std::string str((std::istreambuf_iterator<char>(fstream)),
                    std::istreambuf_iterator<char>());

    Json::Value value;
    Json::Reader reader;
    try {
        if (!reader.parse(str, value))
        {
            AGG_LOG(ERROR, "parse json string failed [%s]", str.c_str());
            return false;
        }
    }
    catch (...)
    {
        AGG_LOG(ERROR, "parse json string failed [%s]", str.c_str());
        return false;
    }

    if (value.isArray()) {
        int size = value.size();
        for (int i = 0; i < size; i++) {
            Init(value[i]);
        }
    }
    return true;
}

bool RewriteQueryPoolData::Init(const Json::Value& root)
{
    if(!root.isObject()) {
        AGG_LOG(ERROR, "Invalid json object");
        return false;
    }

    std::string query;
    double qt = 2.9;
    if(root.isMember("query") && root["query"].isString()){
        query = root["query"].asString();
    }
    if(root.isMember("qt") && root["qt"].isDouble()){
        qt = root["qt"].asDouble();
    }
    
    if(root.isMember("qt") && root["qt"].isString()){
        string qtstr = root["qt"].asString();
        if (qtstr.size() > 0)
            qt = autil::StringUtil::strToDoubleWithDefault(qtstr.c_str(), 3.0);
    }

    // sc_stype可以为空，匹配缺sc_stype的sc
    if (query.empty()) {
        AGG_LOG(WARN, "Bad formated template [%s]", query.c_str());
        return false;
    }
    mQuerys[query] = qt;
    AGG_LOG(DEBUG, "query:[%s], qt:[%lf]", query.c_str(), qt);
    return true;
}

double RewriteQueryPoolData::GetApproximateSimilarityWithMaxLen(const std::string &source, const std::string &dest, size_t maxLen) {
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

bool RewriteQueryPoolData::GetMostSimQuery(const std::string& query, std::string& max_sim_query, double & qt_threshold, size_t max_len, double sim_threshold){
    double cur_max_sim = 0;
    for(const auto& elem: mQuerys){
        double sim = GetApproximateSimilarityWithMaxLen(query, elem.first, max_len);
        if(sim > sim_threshold && sim > cur_max_sim){
            max_sim_query = elem.first;
            qt_threshold = elem.second;
            cur_max_sim = sim;
//            return true;
        }
    }
    return cur_max_sim > 0;
}
bool RewriteQueryPoolData::GetMostSimQuery(const std::string& query, std::vector<pair<std::string, double>>& sim_querys, size_t max_len, double sim_threshold, size_t topn){
    std::vector<pair<double, pair<std::string, double>>> vec;
    for(const auto& elem: mQuerys){
        double sim = GetApproximateSimilarityWithMaxLen(query, elem.first, max_len);
        if(sim > sim_threshold){
            vec.emplace_back(make_pair(sim, make_pair(elem.first, elem.second)));
        }
    }
    std::sort(vec.begin(), vec.end(), [](pair<double, pair<std::string, double>> &a1, pair<double, pair<std::string, double>> &a2){return a1.first > a2.first;});
    for(size_t i=0; i!=vec.size() && i<topn; ++i){
        sim_querys.emplace_back(vec[i].second);
        AGG_LOG(DEBUG, "return sim querys:[%s] qt:[%lf] sim:[%lf]", vec[i].second.first.c_str(), vec[i].second.second, vec[i].first);
    }
    return !sim_querys.empty();
}

AGG_END_NAMESPACE(data);
