#include <fstream>
#include <agg_web2/data/key_value_data.h>
#include <agg2/util/string_tool.h>
#include <agg2/util/url_util.h>
#include <autil/StringUtil.h>

AGG_BEGIN_NAMESPACE(data);
AGG_LOG_SETUP(agg_web2, KeyValueData);
using namespace std;

bool KeyValueData::Init(const std::string &input) 
{
    mKv.clear();
    std::ifstream fstream(input);
    std::string str((std::istreambuf_iterator<char>(fstream)), std::istreambuf_iterator<char>());

    Json::Value value;
    Json::Reader reader;
    try {
        if (!reader.parse(str, value)) {
            AGG_LOG(ERROR, "parse json string failed [%s]", str.c_str());
            return false;
        }
    } catch (...) {
        AGG_LOG(ERROR, "parse json string failed [%s]", str.c_str());
        return false;
    }

    if (value.isArray()) {
        int size = value.size();
        for (int i = 0; i < size; i++) {
            Init(value[i]);
        }
    }

    AGG_LOG(INFO, "KeyValueData Init Successed!");
    return true;
}

bool KeyValueData::Init(const Json::Value& root)
{
    if (!root.isObject()) {
        AGG_LOG(ERROR, "Invalid json object");
        return false;
    }

    std::string key, value;
    if (root.isMember("key") && root["key"].isString()) {
        key = root["key"].asString();
    }
    if (root.isMember("value") && root["value"].isString()) {
        value = root["value"].asString();
    }
    if (! key.empty()) {
        AGG_LOG(DEBUG, "KeyValueData::Init key=%s, value=%s", key.c_str(), value.c_str());
        mKv[key] = value;
    }
    return true;
}

AGG_END_NAMESPACE(data);
