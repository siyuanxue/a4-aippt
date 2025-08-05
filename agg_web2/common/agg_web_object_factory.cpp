#include <agg_web2/common/agg_web_object_factory.h>
#include <A4/util/string_util.h>

AGG_BEGIN_NAMESPACE(common);
AGG_LOG_SETUP(common, AggWebObjectFactory);

// serialize class object
BEGIN_DEFINE_SERIALIZATION_FUNC(AggWebObjectFactory)
    // Example: serialize mField
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(AggWebObjectFactory)
        
AggWebObjectFactory::AggWebObjectFactory()
{
}

AggWebObjectFactory::~AggWebObjectFactory() 
{ 
}

bool AggWebObjectFactory::AggInit(const String2String& params)
{
    return true;
}

SerializablePtr AggWebObjectFactory::AggCreateObject(const std::string& className)
{ 
    if (className.empty() || className == "unimplemented"){
        AGG_LOG(ERROR, "unimplemented class name");
        SerializablePtr object; 
        return object;       
    }
    AGG_LOG(ERROR, "unknown processor class name: %s", className.c_str());
    SerializablePtr object;
    return object;
}
AGG_END_NAMESPACE(common);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(common)::AggWebObjectFactory);

