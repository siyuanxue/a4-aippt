#ifndef AGG_WEB_MODULE_FACTORY_H
#define AGG_WEB_MODULE_FACTORY_H

#include <agg_web2/common/common.h>
#include <agg2/agg_module_factory.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class AggWeb2ModuleFactory : public AggModuleFactory
{
public:
    AggWeb2ModuleFactory();
    virtual ~AggWeb2ModuleFactory();

public:
    virtual bool AggInit(const A4_NS(common)::String2String& params) override;
    virtual Processor* AggCreateProcessor(const std::string& className) override;
    virtual A4_NS(module)::AppSelector* AggCreateAppSelector(const std::string& className) override;
    virtual A4_NS(data_engine)::DataLoader* AggCreateDataLoader(const std::string& className) override;
    virtual bool OnAggPreStartup(const GlobalManager* globalMgr) override;

    virtual void Destroy() override;
    
    static std::string GetLocalIp() { return mLocalIpStr;}

private:
    String2String mParams;   // 保留一份初始化参数MAP副本

private:
    static std::string mLocalIpStr;
    AGG_LOG_DECLARE();
};

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_WEB_MODULE_FACTORY_H
