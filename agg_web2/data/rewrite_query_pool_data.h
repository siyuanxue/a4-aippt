#ifndef AGG_WEB_DATA_REWRITE_QUERY_POOL_H
#define AGG_WEB_DATA_REWRITE_QUERY_POOL_H

#include <agg2/data/default_user_data.h>
#include <agg2/helper/tixml_helper.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(data);

class RewriteQueryPoolData : public DefaultUserData
{
public:
    RewriteQueryPoolData() = default;
    ~RewriteQueryPoolData() = default;
    virtual bool Init(const std::string &input);
    bool Init(const Json::Value& root);
public:
    std::unordered_map<std::string, double>& GetQueryMap() {
        return mQuerys;
    }
    bool GetMostSimQuery(const std::string& query, std::string& max_sim_query, double & qt_threshold, size_t max_len=100, double sim_threshold=0.5);
    bool GetMostSimQuery(const std::string& query, std::vector<std::pair<std::string, double>>& sim_querys, size_t max_len=100, double sim_threshold=0.5, size_t topn=5);

private:
    std::unordered_map<std::string, double> mQuerys;
private:
    double GetApproximateSimilarityWithMaxLen(const std::string &source, const std::string &dest, size_t maxLen);

private:
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(RewriteQueryPoolData);

AGG_END_NAMESPACE(data);
#endif //AGG_WEB_DATA_SC_TEMPLATE_DATA_H

