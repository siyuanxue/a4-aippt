#ifndef AGG_WEB_DATA_VALUE_DATA_H
#define AGG_WEB_DATA_VALUE_DATA_H

#include <agg2/data/default_user_data.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(data);

class ValueData : public DefaultUserData
{
public:
    ValueData() = default;
    ~ValueData() = default;
    virtual bool Init(const std::string &input);
    const std::vector<std::string>& GetValue() {
        return mValue;
    }

private:
    std::vector<std::string> mValue;

private:
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(ValueData);

AGG_END_NAMESPACE(data);
#endif

