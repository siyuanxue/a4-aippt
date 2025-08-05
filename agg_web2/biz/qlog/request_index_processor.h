#ifndef AGG_WEB2_BIZ_QLOG_REQUEST_INDEX_PROCESSOR_H
#define AGG_WEB2_BIZ_QLOG_REQUEST_INDEX_PROCESSOR_H

#include <agg2/processor/base_processor.h>
#include <agg2/common/agg_context.h>
#include <agg_web2/common/common.h>
#include <agg2/util/string_tool.h>
#include <agg_web2/util/AggWebStringTool.h>
#include <jsoncpp/json/json.h>
#include <agg2/common/qrs/qrs_response.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class RequestIndexProcessor : public  BaseProcessor
{
public:
    RequestIndexProcessor() {}
    ~RequestIndexProcessor() {}

public:
    bool DoInit(const String2String& params) override;
    bool RegisterMethods() override;
    BaseProcessor* DoClone() override;
    void DoDestroy() override;

public:
    int GenQrsClause(AggContext *aggContext);
    int ReadCache(AGG::AggContext *aggContext);
    int UpdateCache(AGG::AggContext *aggContext);
    int RequestIndex(AGG::AggContext *aggContext);

private:
    struct QrsClauseParam
    {
        String2String _paramMap;
        String2String _configMap;
        String2String _kvMap;
    };
    typedef std::map<std::string, QrsClauseParam> QrsClauseParamMap;
    typedef std::map<std::string, QrsClauseParam>::iterator QrsClauseParamMapIter;
    typedef std::map<std::string, QrsClauseParam>::const_iterator QrsClauseParamMapConstIter;
    
    struct SubContextInfo
    {
        SubContextInfo(AggSubContext* subContext)
            : _subContext(subContext)
        {}
        
        AggSubContext* _subContext;
        std::vector<QrsResponsePtr> _qrsResponseVec;
    };

private:
    // gen qrs clause
    bool ParseConfig(const std::string& configStr, QrsClauseParam& qrsClauseParam);
    bool ParseConfigItem(const std::string& str, String2String& kvMap);
    void SetQrsClauseMapByParamMap(const QrsClauseParamMap& paramMap, 
                                   QrsClauseMap& qrsClauseMap);
    void SetQrsClauseParam(QrsClause* qrsClause, const std::string& key, 
                           const std::string& value);
    
    // request index
    void CalculateTotalHits(const std::vector<SubContextInfo>& subContextInfoVec);
    
private:
    QrsClauseParamMap mQrsClauseParamMap;

private:
    AGG_LOG_DECLARE();
    AGG_LOG_DECLARE_WITH_NAME(displayLog);

private:
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(RequestIndexProcessor);
AGG_END_NAMESPACE(agg_web2)
BOOST_CLASS_EXPORT_KEY(AGG_NS(medical)::RequestIndexProcessor)

#endif //AGG_WEB2_BIZ_QLOG_REQUEST_INDEX_PROCESSOR_H

