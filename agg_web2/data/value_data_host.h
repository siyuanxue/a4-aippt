#ifndef AGG_WEB_DATA_VALUE_DATA_HOST_H
#define AGG_WEB_DATA_VALUE_DATA_HOST_H

#include <agg2/data/default_user_data.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(data);

class ValueDataHost : public DefaultUserData
{
public:
    ValueDataHost() = default;
    ~ValueDataHost() = default;
    virtual bool Init(const std::string &input);

    const std::unordered_map<std::string, std::string>& GetValue() {
        return mHostNameLogoMap;
    }
	const std::string GetHostName(const std::string& url);

private:
    std::unordered_map<std::string, std::string> mHostNameLogoMap;

private:
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(ValueDataHost);

AGG_END_NAMESPACE(data);
#endif

