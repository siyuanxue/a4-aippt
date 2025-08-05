#ifndef AGG_WEB_LOADER_AGG_WEB_HTTP_DATA_LOADER_H
#define AGG_WEB_LOADER_AGG_WEB_HTTP_DATA_LOADER_H

#include <agg_web2/common/common.h>
#include <agg2/loader/default_http_data_loader.h>

AGG_BEGIN_NAMESPACE(loader);

class AggWebHttpDataLoader: public DefaultHttpDataLoader
{
    DECLARE_SERIALIZATION_FUNC();

public:
    AggWebHttpDataLoader();
    virtual ~AggWebHttpDataLoader();

public:
    virtual bool Init(const String2String& params) override;
    virtual DataLoader* Clone() override;
    virtual void Destroy() override;

private:
    virtual DefaultUserData * CreateDefaultUserData() override;

private:
    AGG_LOG_DECLARE();
};

AGG_TYPEDEF_PTR(AggWebHttpDataLoader);

AGG_END_NAMESPACE(loader);
BOOST_CLASS_EXPORT_KEY(AGG_NS(loader)::AggWebHttpDataLoader);

#endif //AGG2_LOADER_DEFAULT_DATA_LOADER_H
