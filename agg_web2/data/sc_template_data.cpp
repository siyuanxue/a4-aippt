#include <fstream>
#include <agg_web2/data/sc_template_data.h>
#include <agg2/util/string_tool.h>
#include <agg2/util/url_util.h>
#include <autil/StringUtil.h>

AGG_BEGIN_NAMESPACE(data);
AGG_LOG_SETUP(agg_web2, ScTemplateData);
using namespace std;

const std::string TEMP_START = "${";
const std::string TEMP_END = "}";
const std::string FUNC_START = "`FUNC`";
const std::string PARAM_START = "`[";
const std::string PARAM_END = "`]";
const std::string PARAM_SP = "`";

FuncMap ScTemplateData::mFuncMap = {
    {"plus", &ScTemplateData::Plus},
    {"subtract", &ScTemplateData::Subtract},
    {"multiply", &ScTemplateData::Multiply},
    {"divide", &ScTemplateData::Divide},
    {"time_format", &ScTemplateData::TimeFormat}
};

bool ScTemplateData::Init(const std::string &input) 
{
    mTemplatesVec.clear();
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

    auto cmp = [](ScTemplate& a, ScTemplate& b) {
        return a.rawText.size() > b.rawText.size();
    };
    for(auto& tmp:mTemplatesVec) {
        std::vector<ScTemplate>& tmpList = tmp.second;
        sort(tmpList.begin(), tmpList.end(), cmp);
    }

    // DebugPrintTemplates();

    return true;
}

bool ScTemplateData::Init(const Json::Value& root)
{
    if(!root.isObject()) {
        AGG_LOG(ERROR, "Invalid json object");
        return false;
    }

    std::string name;
    std::string scStype;
    std::string templateStr;
    std::string query;
    if(root.isMember("sc_name") && root["sc_name"].isString()){
        name = root["sc_name"].asString();
    }
    if(root.isMember("sc_stype") && root["sc_stype"].isString()){
        scStype = root["sc_stype"].asString();
    }
    if(root.isMember("template") && root["template"].isString()){
        templateStr = root["template"].asString();
    }
    if(root.isMember("query") && root["query"].isString()){
        query = root["query"].asString();
    }

    // sc_stype可以为空，匹配缺sc_stype的sc
    if (name.empty() || templateStr.empty()) {
        AGG_LOG(WARN, "Bad formated template [%s][%s][%s]",
                name.c_str(), scStype.c_str(), templateStr.c_str());
        return false;
    }

    AGG_LOG(WARN, "init template [%s][%s]", name.c_str(), templateStr.c_str());

    ScTemplate scTemplate;
    if (!ParseTemplate(templateStr, scTemplate)) {
        AGG_LOG(WARN, "Failed to parse sc template. template str=[%s]", templateStr.c_str());
        return false;
    }

    std::string key = name + '`' + scStype;
    mTemplatesVec[key].push_back(std::move(scTemplate));

    if (! query.empty()) {
        mQuerys[key] = query;
    }

    return true;
}

// 待修改
// void ScTemplateData::DebugPrintTemplates()
// {
//     for (auto it : mTemplates) {
//         ScTemplate& temp = it.second;
//         size_t len = temp.nodeList.size();
//         size_t lenTxt = temp.txtList.size();
//         if (lenTxt != len + 1 || lenTxt == 0) {
//             AGG_LOG(WARN, "Invalid temp, name=[%s].", it.first.c_str());
//             continue;
//         }
// 
//         std::string tempStr = temp.txtList[0];
//         for (size_t i = 0; i < len; ++i) {
//             TemplateNode& node = temp.nodeList[i];
//             if (node.type == TYPE_TXT) {
//                 if (node.pathVec.size() == 0) {
//                     AGG_LOG(WARN, "Invalid node.");
//                     break;
//                 }
//                 tempStr += TEMP_START + node.pathVec[0] + TEMP_END;
//             }
//             else {
//                 tempStr += TEMP_START + FUNC_START + node.name + PARAM_START;
//                 bool first = true;
//                 for (auto param : node.pathVec) {
//                     if (!first) {
//                         tempStr += PARAM_SP;
//                     }
//                     tempStr += param;
//                     first = false;
//                 }
//                 tempStr += PARAM_END + TEMP_END;
//             }
//             tempStr += temp.txtList[i + 1];
//         }
// 
//         AGG_LOG(WARN, "[debug sc template] name=[%s], tmeplateStr=[%s]", it.first.c_str(), tempStr.c_str());
//     }
// }

bool ScTemplateData::ParseTemplate(const std::string& txt, ScTemplate& temp)
{
    temp.rawText = txt;
    // txtList首尾有可能是空字符串
    string::size_type begin = 0, end = 0;
    string::size_type pre = 0;
    while(FindNode(txt, pre, begin, end)) {
        temp.txtList.emplace_back(txt.substr(pre, begin - pre));
        begin += TEMP_START.length();
        std::string tempStr = txt.substr(begin, end - begin);
        TemplateNode node;
        if (!ParseFunc(tempStr, node)) {
            return false;
        }
        temp.nodeList.emplace_back(node);
        pre = end + TEMP_END.length();
    }

    if (end == 0) {
        temp.txtList.emplace_back(txt);
    }
    else {
        temp.txtList.emplace_back(txt.substr(end + TEMP_END.length()));
    }

    if (temp.txtList.size() != temp.nodeList.size() + 1) {
        AGG_LOG(WARN, "txtList[%zu] is not equal to nodeList[%zu] + 1.",
                temp.txtList.size(), temp.nodeList.size());
        return false;
    }

    return true;
}

bool ScTemplateData::ParseFunc(const std::string& txt, TemplateNode& temp)
{
    string::size_type pos = txt.find(FUNC_START);
    if (pos == std::string::npos) {
        temp.type = TYPE_TXT;
        temp.func = NULL;
        temp.pathVec.emplace_back(txt);
        return true;
    }

    pos += FUNC_START.length();
    string::size_type left = txt.find(PARAM_START, pos);
    if (left == std::string::npos) {
        AGG_LOG(WARN, "Template function is not valid(miss left). template str[%s]", txt.c_str());
        return false;
    }

    std::string funcName = txt.substr(pos, left - pos);
    if (funcName.empty() || mFuncMap.find(funcName) == mFuncMap.end()) {
        AGG_LOG(WARN, "Template function is not valid(miss func). func=[%s], template str[%s]",
                funcName.c_str(), txt.c_str());
        return false;
    }

    temp.type = TYPE_FUNC;
    temp.name = funcName;
    temp.func = mFuncMap[funcName];

    left += PARAM_START.length();
    string::size_type right = txt.find(PARAM_END, left);
    if (right == std::string::npos) {
        AGG_LOG(WARN, "Template function is not valid(miss right). template str[%s]", txt.c_str());
        return false;
    }

    std::string paramStr = txt.substr(left, right - left);
    string::size_type sp = paramStr.find(PARAM_SP, left);
    if (sp == std::string::npos) {
        temp.pathVec.emplace_back(paramStr);
    }
    else {
        string::size_type preSp = 0;
        while(sp != std::string::npos) {
            temp.pathVec.emplace_back(paramStr.substr(preSp, sp - preSp));
            preSp = sp + PARAM_SP.length();
            sp = paramStr.find(PARAM_SP, preSp);
        }
        temp.pathVec.emplace_back(paramStr.substr(preSp));
    }

    return true;
}

bool ScTemplateData::FindNode(const std::string& txt, size_t startPos,
                              std::string::size_type& begin, std::string::size_type& end)
{
    begin = txt.find(TEMP_START, startPos);
    if (begin == std::string::npos) {
        return false;
    }

    end = txt.find(TEMP_END, begin);
    if (end == std::string::npos) {
        return false;
    }

    return true;
}

bool ScTemplateData::CheckName(const std::string& name)
{
    auto it = mTemplatesVec.find(name);
    return it != mTemplatesVec.end();
}

bool ScTemplateData::ParseScXml(const std::string& name, TiXmlElement * root, std::string& fmtStr)
{
    auto it = mTemplatesVec.find(name);
    if (it == mTemplatesVec.end()) {
        AGG_LOG(WARN, "No such sc config, name=[%s].", name.c_str());
        return false;
    }
    std::vector<ScTemplate>& tempVec = it->second;
    for (auto& temp:tempVec) {
        size_t len = temp.nodeList.size();
        size_t lenTxt = temp.txtList.size();
        if (lenTxt != len + 1 || lenTxt == 0) {
            AGG_LOG(WARN, "Invalid template, name=[%s].", it->first.c_str());
            continue;
        }
        TiXmlElement * hitsElement;
        CHECK_FINDFIRSTNODE_RET(root, "hits", hitsElement);
        if (hitsElement == NULL) {
            AGG_LOG(ERROR, "GetAggWebResult hitsElement error");
            continue;
        }
        if (temp.txtList.size() == 0) {
            AGG_LOG(ERROR, "temp.txtList.size == 0");
            continue;
        }

        std::string tempStr = temp.txtList[0];
        bool parseError = false;
        for (size_t i = 0; i < len; ++i) {
            TemplateNode& node = temp.nodeList[i];
            if (node.type == TYPE_TXT) {
                if (node.pathVec.size() == 0) {
                    AGG_LOG(WARN, "Invalid node.");
                    parseError = true;
                    break;
                }
                std::string val;
                if (GetPathValue(hitsElement, node.pathVec[0], val)) {
                    tempStr += val;
                } else {
                    parseError = true;
                    break;
                }
            }
            else {
                TemplateParam param;
                if (!ParseFuncParams(hitsElement, node, param)) {
                    AGG_LOG(WARN, "Invalid node.");
                    parseError = true;
                    break;
                }
                TemplateFuncPtr funcPtr = node.func;
                bool status = (this->*funcPtr)(param);
                if (!status) {
                    AGG_LOG(WARN, "Invalid function [%s].", node.name.c_str());
                    parseError = true;
                    break;
                }
                tempStr += param.result;
            }
            tempStr += temp.txtList[i + 1];
        }
        if(!parseError) {
            fmtStr = std::move(tempStr);
            AGG_LOG(DEBUG, "ParseScXml reuslt=[%s]", fmtStr.c_str());
            return true;
        }
    }

    return false;
}

bool ScTemplateData::ParseFuncParams(TiXmlElement *element, const TemplateNode& node, TemplateParam& param)
{
    int cnt = 0;
    for (const auto& path : node.pathVec) {
        std::string val;
        if (!GetPathValue(element, path, val)) {
            AGG_LOG(WARN, "Invalid param path [%s].", path.c_str());
            return false;
        }
        if (cnt == 0) {
            param.param1 = val;
        }
        else if (cnt == 1) {
            param.param2 = val;
        }
        else {
            AGG_LOG(WARN, "Invalid param path vactor.");
            return false;
        }
        cnt++;
    }

    return true;
}

bool ScTemplateData::GetPathValue(TiXmlElement *element, const std::string& pathStr, std::string& val)
{
    if (pathStr.find("`1") != std::string::npos) {
        return GetListValue(element, pathStr, val);
    }

    std::vector<std::string> path = autil::StringUtil::split(pathStr, "/");
    for (const auto& key: path) {
        TiXmlElement * ele;
        CHECK_FINDFIRSTNODE_RET(element, key, ele);
        if (ele == NULL) {
            AGG_LOG(ERROR, "Get sc element error, key=%s", key.c_str());
            return false;
        }
        element = ele;
    }

    if (element && element->GetText()) {
        val = element->GetText();
        return true;
    }

    return false;
}

bool ScTemplateData::GetListValue(TiXmlElement *element, const std::string& pathStr, std::string& val)
{
    std::vector<std::string> arr = autil::StringUtil::split(pathStr, "`1");
    if (arr.size() != 4 && arr.size() != 5) {
        AGG_LOG(ERROR, "Wrong formated list template=%s", pathStr.c_str());
        return false;
    }

    // 格式： 父一级xml path`1子一级key列表`1文字列表`1分隔符
    // 文字列表长度是key列表长度+1，两个文字夹一个xml value
    // list的各个元素直接由分隔符连接
    std::string rootStr = arr[0];
    std::string keyStr = arr[1];
    std::string txtStr = arr[2];
    std::string spStr = arr[3];
    std::string totalnumStr;
    if(arr.size()>4) {
        totalnumStr = arr[4];
    }
    int totalnum = -1;
    if(totalnumStr.size()>0) {
        SAFE_TYPE_CHANGE(totalnumStr, totalnum);
    }

    std::vector<std::string> keys = autil::StringUtil::split(keyStr, "`2");
    std::vector<std::string> txts = autil::StringUtil::split(txtStr, "`2", false);
    
    if (keys.size() + 1 != txts.size()) {
        AGG_LOG(ERROR, "Wrong formated list template=%s", pathStr.c_str());
        return false;
    }

    // 找到根节点
    std::vector<std::string> path = autil::StringUtil::split(rootStr, "/");
    for (const auto& key: path) {
        TiXmlElement * ele;
        CHECK_FINDFIRSTNODE_RET(element, key, ele);
        if (ele == NULL) {
            AGG_LOG(ERROR, "Get sc element error, key=%s", key.c_str());
            return false;
        }
        element = ele;
    }

    // 遍历list
    std::vector<std::vector<std::string>> vals;
    TiXmlElement *root = element;
    const std::string ListRootStr = element->Value();
    int cnt = 0;
    if (root) {
        for(; (root != nullptr && root->Value()==ListRootStr); root = root->NextSiblingElement()) {
            std::vector<std::string> oneVal;
            for (const auto& key: keys) {
                TiXmlElement *keyEle = root;
                std::vector<std::string> key_path = autil::StringUtil::split(key, "/");
                
                for(auto& node:key_path) {
                    TiXmlElement *ele;
                    CHECK_FINDFIRSTNODE_RET(keyEle, node, ele);
                    if (ele == NULL) {
                        AGG_LOG(ERROR, "Get sc element error, key=%s", key.c_str());
                        return false;
                    }
                    keyEle = ele;
                }

                if (keyEle && keyEle->GetText()) {
                    std::string text = keyEle->GetText();
                    oneVal.emplace_back(text);
                }
            }
            if(oneVal.size() != keys.size()) {
                continue;
            }
            cnt += 1;
            vals.emplace_back(oneVal);
            if(totalnum>0 && cnt==totalnum) {
                break;
            }
        }
    }

    // 组装结果
    std::vector<std::string> rows;
    size_t count = vals.size();
    size_t keyCnt = keys.size();
    for (size_t i = 0; i < count; ++i) {
        std::vector<std::string>& oneVal = vals[i];
        std::string item;
        for (size_t j = 0; j < keyCnt; ++j) {
            item += txts[j];
            item += oneVal[j];
        }
        item += txts[keyCnt];
        rows.emplace_back(item);
    }

    val = autil::StringUtil::toString(rows, spStr);
    AGG_LOG(DEBUG, "get list, key=%s, val=%s", pathStr.c_str(), val.c_str());

    return true;
}

bool ScTemplateData::Plus(TemplateParam& param)
{
    float left = 0, right = 0;
    SAFE_TYPE_CHANGE(param.param1, left);
    SAFE_TYPE_CHANGE(param.param2, right);
    float ret = left + right;
    param.result = autil::StringUtil::toString<float>(ret);
    return true;
}

bool ScTemplateData::Subtract(TemplateParam& param)
{
    float left = 0, right = 0;
    SAFE_TYPE_CHANGE(param.param1, left);
    SAFE_TYPE_CHANGE(param.param2, right);
    float ret = left - right;
    param.result = autil::StringUtil::toString<float>(ret);
    return true;
}

bool ScTemplateData::Multiply(TemplateParam& param)
{
    float left = 0, right = 0;
    SAFE_TYPE_CHANGE(param.param1, left);
    SAFE_TYPE_CHANGE(param.param2, right);
    float ret = left * right;
    param.result = autil::StringUtil::toString<float>(ret);
    return true;
}

bool ScTemplateData::Divide(TemplateParam& param)
{
    float left = 0, right = 0;
    SAFE_TYPE_CHANGE(param.param1, left);
    SAFE_TYPE_CHANGE(param.param2, right);
    if (std::abs(right = 0.0) < std::numeric_limits<double>::epsilon()) {
        param.result = "";
        return false;
    }
    float ret = left - right;
    param.result = autil::StringUtil::toString<float>(ret);
    return true;
}

bool ScTemplateData::TimeFormat(TemplateParam& param)
{
    std::string CurrenttimeStr = param.param1;
    int Currenttime = 0;
    SAFE_TYPE_CHANGE(CurrenttimeStr, Currenttime);
    if (Currenttime < 1600000000) {
        return false;
    }

    char outStr[256];
    time_t t = Currenttime;
    struct tm *tmp = localtime(&t);
    strftime(outStr, sizeof(outStr), "%Y-%m-%d %H:%M:%S", tmp);

    param.result = std::string(outStr);

    return true;
}

AGG_END_NAMESPACE(data);
