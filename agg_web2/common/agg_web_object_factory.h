#ifndef AGG_WEB_OBJECT_FACTORY_H
#define AGG_WEB_OBJECT_FACTORY_H

#include <agg2/common/agg_common.h>
#include <agg2/common/agg_object_factory.h>

AGG_BEGIN_NAMESPACE(common);

class AggWebObjectFactory: public AggObjectFactory
{
public:
    AggWebObjectFactory();
    virtual ~AggWebObjectFactory();
public:
    virtual bool AggInit(const String2String& params) override;
    virtual SerializablePtr AggCreateObject(const std::string& className) override;
private:
    AGG_LOG_DECLARE();
private:
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(AggWebObjectFactory);

AGG_END_NAMESPACE(common);
BOOST_CLASS_EXPORT_KEY(AGG_NS(common)::AggWebObjectFactory);

#endif //AGG_WEB_OBJECT_FACTORY_H

