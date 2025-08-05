#ifndef AGG_WEB_DATA_SC_TEMPLATE_DATA_H
#define AGG_WEB_DATA_SC_TEMPLATE_DATA_H

#include <agg2/data/default_user_data.h>
#include <agg2/helper/tixml_helper.h>
#include <jsoncpp/json/json.h>

AGG_BEGIN_NAMESPACE(data);

struct TemplateParam
{
    std::string param1;
    std::string param2;
    std::string result;
};

class ScTemplateData;
typedef bool (ScTemplateData::*TemplateFuncPtr)(TemplateParam&);
typedef std::map<std::string, TemplateFuncPtr> FuncMap;

enum TemplateType
{
    TYPE_TXT,
    TYPE_FUNC
};

struct TemplateNode
{
    TemplateType type;
    std::vector<std::string> pathVec;
    TemplateFuncPtr func;
    std::string name;
};

struct ScTemplate
{
    std::vector<std::string> txtList;
    std::vector<TemplateNode> nodeList;
    std::string rawText;
};

class ScTemplateData : public DefaultUserData
{
public:
    ScTemplateData() = default;
    ~ScTemplateData() = default;
    virtual bool Init(const std::string &input);
    bool Init(const Json::Value& root);

    // 从agg结果中根据配置解析并拼接摘要字符串
    bool ParseScXml(const std::string& name, TiXmlElement * root, std::string& fmtStr);
	bool CheckName(const std::string& name);

    std::string GetQuery(const std::string& name) {
        auto iter = mQuerys.find(name);
        if (iter != mQuerys.end()) {
            return iter->second;
        }
        return "";
    }

private:
    bool FindNode(const std::string& txt, size_t startPos, std::string::size_type& begin, std::string::size_type& end);
    bool ParseFunc(const std::string& txt, TemplateNode& temp);
    bool ParseTemplate(const std::string& txt, ScTemplate& temp);
    bool ParseFuncParams(TiXmlElement *element, const TemplateNode& node, TemplateParam& param);
    bool GetPathValue(TiXmlElement *element, const std::string& pathStr, std::string& val);
    bool GetListValue(TiXmlElement *element, const std::string& pathStr, std::string& val);
    // void DebugPrintTemplates();

    // 用户自定义函数
    bool Plus(TemplateParam& param);
    bool Subtract(TemplateParam& param);
    bool Multiply(TemplateParam& param);
    bool Divide(TemplateParam& param);
    bool TimeFormat(TemplateParam& param);

private:
    static FuncMap mFuncMap;
    std::unordered_map<std::string, std::string> mQuerys;
    std::unordered_map<std::string, std::vector<ScTemplate>> mTemplatesVec;

private:
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(ScTemplateData);

AGG_END_NAMESPACE(data);
#endif //AGG_WEB_DATA_SC_TEMPLATE_DATA_H

