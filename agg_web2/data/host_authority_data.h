#ifndef AGG_WEB_DATA_HOST_AUTHORITY_DATA_H
#define AGG_WEB_DATA_HOST_AUTHORITY_DATA_H

#include <agg2/data/default_user_data.h>

AGG_BEGIN_NAMESPACE(data);

class HostAuthorityData : public DefaultUserData
{
public:
    HostAuthorityData() = default;
    ~HostAuthorityData() = default;

    virtual bool Init(const std::string &input);
    bool GetValue(const std::string& domain, const std::string& url, int& weight);

private:
    // refer to domain, host, weight
    std::unordered_map<std::string, std::unordered_map<std::string, int>> _mKKV;

private:
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(HostAuthorityData);

AGG_END_NAMESPACE(data);
#endif

