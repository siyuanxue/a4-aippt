#ifndef AGG_WEB2_BIZ_KNOWLEDGE_MOUNT_TRIGGER_PROCESSOR_H
#define AGG_WEB2_BIZ_KNOWLEDGE_MOUNT_TRIGGER_PROCESSOR_H

#include <agg2/common/agg_common.h>
#include <agg2/processor/base_processor.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class MountTriggerProcessor : public BaseProcessor
{
public:
    MountTriggerProcessor();
    virtual ~MountTriggerProcessor();

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

private:
    void RequestFormatResult();
    void FormatValidResult(const std::string rawQuery, const std::string matchQuery, const std::string matchView, const double qt_score, const std::string message);
    void FormatEmptyResult(const std::string message);

private:
    struct ZxfRankNode {
        std::string mQuery;
        std::string mMatchQuery;
        std::string mMatchView;
        double mQt;
    };

private:
    std::string mDebugInfo;
    std::unordered_map<std::string, std::string> mZxfViewKvMap;
    std::vector<std::string> mMatchQueryVec;
    std::vector<ZxfRankNode> mZxfQueryViewRankVec;

    std::vector<std::string> GetQueryGaokaoIntentForMount(std::string& query);
    bool GetZxfViewJyData();
    bool MatchSeedQuery(const std::string query, std::string& seedView);
    bool MatchMajorNameWithQuery(const std::string query);
    std::vector<double> GetQtForMount(AggContext* aggContext, const std::string& query, const std::vector<std::string>& titles);

private:
    AGG_LOG_DECLARE();
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(MountTriggerProcessor);

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_WEB2_BIZ_KNOWLEDGE_MOUNT_TRIGGER_PROCESSOR_H
