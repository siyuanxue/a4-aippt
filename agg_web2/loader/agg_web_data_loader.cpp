#include <agg_web2/loader/agg_web_data_loader.h>
#include <agg_web2/data/agg_web_data_factory.h>

AGG_BEGIN_NAMESPACE(loader);

BEGIN_DEFINE_SERIALIZATION_FUNC(AggWebDataLoader){
    //ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseProcessor);
    //ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(member);
} END_DEFINE_SERIALIZATION_FUNC(AggWebDataLoader)

AGG_LOG_SETUP(agg_web2, AggWebDataLoader);

AggWebDataLoader::AggWebDataLoader()
{
}

AggWebDataLoader::~AggWebDataLoader() { 
}

bool AggWebDataLoader::Init(const String2String& params)
{
    return DefaultDataLoader::Init(params);
}

DataLoader* AggWebDataLoader::Clone()
{
    return new AggWebDataLoader(*this);
}

void AggWebDataLoader::Destroy()
{
    delete this;
}

DefaultUserData* AggWebDataLoader::CreateDefaultUserData()
{
    DefaultUserData* defaultUserData = AggWebDataFactory::Create(mDataType, mDataInitParam);
    if(NULL == defaultUserData) {
        defaultUserData = DefaultDataLoader::CreateDefaultUserData();
    }
    return defaultUserData;
}


AGG_END_NAMESPACE(loader);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(loader)::AggWebDataLoader);

