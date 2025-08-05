#ifndef AGG_WEB2_BIZ_QLOG_REQUEST_QP_PROCESSOR_H
#define AGG_WEB2_BIZ_QLOG_REQUEST_QP_PROCESSOR_H

#include <agg2/common/agg_common.h>
#include <agg2/processor/base_processor.h>
#include <agg2/common/qp/qp_request.h>
#include <agg2/common/qp/qp_result.h>
#include <agg2/common/qp/qp_analyzer.h>
#include <A4/util/string_tokenizer.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class RequestQpProcessor : public BaseProcessor
{
public:
    RequestQpProcessor();
    virtual ~RequestQpProcessor();
    
public:
    virtual bool DoInit(const String2String & params) override;
    virtual void DoRegisterMetric(AmonitorRegister *monitorRegister) override;
    virtual BaseProcessor * DoClone() override;
    virtual void DoDestroy() override;
    virtual int DoProcess(AggContext * aggContext) override;

public:
    // register UDF MethodFunc
    virtual bool RegisterMethods() override;

public:
    // register UDF actor
    virtual bool RegisterActors() override;

    bool InitQpRequest(AggContext *aggContext, QpRequest & qpRequest,const string &query,const String2String &kvItemMap,const String2String &configItemMap,const std::string &channels);
    QpResultPtr GetQpResult(QpRequestPtr qpRequest, bool setQpResult);

    bool GenerateSubContextWithQpResult(QpResultPtr qpResult, AggContext* context);
    void ReleaseQrsClauseMapVec(std::vector<QrsClauseMap>& qrsClauseMapVec);
    bool GenerateQrsClauseMapByQpHit(QpHit* qpHit, QrsClauseMap& qrsClauseMap);
    bool GenerateSubContext(AggContext* context, std::vector<QrsClauseMap>& qrsClauseMapVec);

private:
    std::string mQpNodeName;

    String2String mKvItemMap;
    String2String mConfigItemMap;
    std::string mChannels;
    std::vector<std::vector<std::string> > mQpRuleItemVec; 


private:
    AGG_LOG_DECLARE();
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(RequestQpProcessor);

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_WEB2_BIZ_QLOG_REQUEST_QP_PROCESSOR_H
