#include <agg_web2/loader/agg_web_http_data_loader.h>
#include <agg_web2/data/agg_web_data_factory.h>

AGG_BEGIN_NAMESPACE(loader);

BEGIN_DEFINE_SERIALIZATION_FUNC(AggWebHttpDataLoader){
    //ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseProcessor);
    //ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(member);
} END_DEFINE_SERIALIZATION_FUNC(AggWebHttpDataLoader)

AGG_LOG_SETUP(agg_web2, AggWebHttpDataLoader);

AggWebHttpDataLoader::AggWebHttpDataLoader()
{
}

AggWebHttpDataLoader::~AggWebHttpDataLoader() { 
}

bool AggWebHttpDataLoader::Init(const String2String& params)
{
    return DefaultHttpDataLoader::Init(params);
}

DataLoader* AggWebHttpDataLoader::Clone()
{
    return new AggWebHttpDataLoader(*this);
}

void AggWebHttpDataLoader::Destroy()
{
    delete this;
}

DefaultUserData* AggWebHttpDataLoader::CreateDefaultUserData()
{
    DefaultUserData* defaultUserData = AggWebDataFactory::Create(mDataType, mDataInitParam);
    if(NULL == defaultUserData) {
        defaultUserData = DefaultHttpDataLoader::CreateDefaultUserData();
    }
    return defaultUserData;
}


AGG_END_NAMESPACE(loader);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(loader)::AggWebHttpDataLoader);

