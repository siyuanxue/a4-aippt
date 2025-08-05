#ifndef AGG_WEB_DATA_KEY_VALUE_DATA_H
#define AGG_WEB_DATA_KEY_VALUE_DATA_H

#include <agg2/data/default_user_data.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(data);

class KeyValueData : public DefaultUserData
{
public:
    KeyValueData() = default;
    ~KeyValueData() = default;
    virtual bool Init(const std::string &input);
    std::string GetValue(const std::string& name) {
        auto iter = mKv.find(name);
        if (iter != mKv.end()) {
            return iter->second;
        }
        return "";
    }
    const std::unordered_map<std::string, std::string>& GetkvData() {return mKv;}
    bool Init(const Json::Value& root);

private:
    std::unordered_map<std::string, std::string> mKv;

private:
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(KeyValueData);

AGG_END_NAMESPACE(data);
#endif

