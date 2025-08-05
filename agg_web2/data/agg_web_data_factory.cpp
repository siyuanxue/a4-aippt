#include <agg_web2/data/agg_web_data_factory.h>
#include <agg_web2/data/sc_template_data.h>
#include <agg_web2/data/rewrite_query_pool_data.h>
#include <agg_web2/data/key_value_data.h>
#include <agg_web2/data/value_data.h>
#include <agg_web2/data/host_authority_data.h>

AGG_BEGIN_NAMESPACE(data);
AGG_LOG_SETUP(agg_web2, AggWebDataFactory);

const std::string BOOSTING_TYPE = "type";

AggWebDataFactory::AggWebDataFactory()
{}

AggWebDataFactory::~AggWebDataFactory()
{}

DefaultUserData * AggWebDataFactory::Create(const std::string & dataType,
                                    const String2String & params)
{
    if (dataType == "ScTemplateData"){
        return (new ScTemplateData);
    } else if (dataType == "RewriteQueryPoolData"){
        return (new RewriteQueryPoolData);
    } else if (dataType == "KeyValueData"){
        return (new KeyValueData);
    } else if (dataType == "ValueData") {
        return (new ValueData);
    } else if (dataType == "HostAuthorityData") {
        return (new HostAuthorityData);
    }
    return NULL;
}

AGG_END_NAMESPACE(data);

