#include <fstream>
#include <agg_web2/data/value_data.h>
#include <agg2/util/string_tool.h>
#include <agg2/util/url_util.h>
#include <autil/StringUtil.h>

AGG_BEGIN_NAMESPACE(data);
AGG_LOG_SETUP(agg_web2, ValueData);
using namespace std;

bool ValueData::Init(const std::string &input)
{
    mValue.clear();
    std::ifstream fstream(input);
    std::string str((std::istreambuf_iterator<char>(fstream)), std::istreambuf_iterator<char>());

    mValue = autil::StringUtil::split(str, "\n");
    AGG_LOG(INFO, "load data success, path:[%s], count:[%d]", input.c_str(), mValue.size());

    AGG_LOG(INFO, "ValueData Init Successed!");
    return true;
}

AGG_END_NAMESPACE(data);
