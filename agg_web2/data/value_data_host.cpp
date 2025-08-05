#include <fstream>
#include <agg_web2/data/value_data_host.h>
#include <agg2/util/string_tool.h>
#include <agg2/util/url_util.h>
#include <autil/StringUtil.h>

AGG_BEGIN_NAMESPACE(data);
AGG_LOG_SETUP(agg_web2, ValueDataHost);
using namespace std;

bool ValueDataHost::Init(const std::string &input) 
{
	mHostNameLogoMap.clear();
    std::ifstream fstream(input);
    std::string str((std::istreambuf_iterator<char>(fstream)), std::istreambuf_iterator<char>());

    auto mValue = autil::StringUtil::split(str, "\n");
	for (auto& value : mValue){
	    vector<string> datatmp = autil::StringUtil::split(value, "\t");
		if (datatmp.size() == 3){
		    mHostNameLogoMap[datatmp[0]] = datatmp[1];
		}

	}
    AGG_LOG(INFO, "ValueDataHost Init Successed!");
    return true;
}

const std::string ValueDataHost::GetHostName(const std::string& url)
{
	string host = UrlUtil::GetHost(url);
	if (host == "page.sm.cn") {
		// find host from h
		vector<string> host1 = autil::StringUtil::split(url, "?");
		if (host1.size() == size_t(2)) {
			vector<string> host2 = autil::StringUtil::split(host1[1], "&");
			for (auto& kv: host2) {
				vector<string> k_v = autil::StringUtil::split(kv, "=");
				if (k_v.size() == size_t(2) && k_v[0] == "h") {
					host = k_v[1];
					break;
				}
			}
		}
	}
	string domain = host;
	while (true) {
		auto iter = mHostNameLogoMap.find(host);
		if (iter != mHostNameLogoMap.end()) {
			return mHostNameLogoMap[host];
		}
		auto it = host.find(".");
		if (it== string::npos) {
			break;
		}
		host = host.substr(it + 1);
	}
	return domain;
}

AGG_END_NAMESPACE(data);
