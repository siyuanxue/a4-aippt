#ifndef AGG_WEB2_MEDCHAT_CACHE_PROCESSOR_H
#define AGG_WEB2_MEDCHAT_CACHE_PROCESSOR_H

#include <agg2/common/agg_common.h>
#include <agg2/processor/base_processor.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class MedChatCacheProcessor: public BaseProcessor
{
public:
    MedChatCacheProcessor();
    virtual ~MedChatCacheProcessor();
    
public:
    virtual bool DoInit(const String2String & params) override;
    virtual void DoRegisterMetric(AmonitorRegister *monitorRegister) override;
    virtual BaseProcessor * DoClone() override;
    virtual void DoDestroy() override;

public:
    int ReadCache(AggContext * aggContext);
    int UpdateCache(AggContext * aggContext);
public:
    // register UDF MethodFunc
    virtual bool RegisterMethods() override;

public:
    // register UDF actor
    virtual bool RegisterActors() override;
private:
    std::string mTairNodeName;
    int mMaxHistorySize;
    AGG_LOG_DECLARE();

private:
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(MedChatCacheProcessor);

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_WEB2_MEDCHAT_CACHE_PROCESSOR_H

