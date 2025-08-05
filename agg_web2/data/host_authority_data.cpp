#include <fstream>
#include <agg_web2/data/host_authority_data.h>
#include <agg2/util/string_tool.h>
#include <agg2/util/url_util.h>
#include <autil/StringUtil.h>

AGG_BEGIN_NAMESPACE(data);
AGG_LOG_SETUP(agg_web2, HostAuthorityData);
using namespace std;


bool HostAuthorityData::Init(const std::string &input)
{
    _mKKV.clear();

    std::ifstream fstream(input);
    std::string str((std::istreambuf_iterator<char>(fstream)), std::istreambuf_iterator<char>());

    std::vector<std::string> lines = autil::StringUtil::split(str, "\n");
    int cnt = 0;
    for (auto line : lines) {
        vector<string> arrs = autil::StringUtil::split(line, "\t");
        if (arrs.size() != 8 || arrs[0].size() == 0 || arrs[1].size() == 0) {
            continue;
        }
        _mKKV[arrs[1]][arrs[0]] = std::stoi(arrs[2]);
        cnt += 1;
    }
    AGG_LOG(INFO, "load data success, path:[%s], count:[%d]", input.c_str(), cnt);

    AGG_LOG(INFO, "HostAuthorityData Init Successed!");
    return true;
}

bool HostAuthorityData::GetValue(const std::string& domain, const std::string& url, int& weight) {
    if (_mKKV.find(domain) == _mKKV.end()) {
        return false;
    }
    for (auto iter = _mKKV[domain].begin(); iter != _mKKV[domain].end(); iter++) {
        if (url.find(iter->first) != std::string::npos) {
            weight = iter->second;
            return true;
        }
    }
    return false;
}


AGG_END_NAMESPACE(data);
