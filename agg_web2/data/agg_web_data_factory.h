#ifndef AGG_WEB_DATA_DATA_FACTORY_H
#define AGG_WEB_DATA_DATA_FACTORY_H

#include <agg2/common/agg_common.h>
#include <agg2/data/default_user_data.h>

AGG_BEGIN_NAMESPACE(data);

class AggWebDataFactory
{
public:
    AggWebDataFactory();
    ~AggWebDataFactory();
public:
    static DefaultUserData * Create(const std::string & dataType,
                                    const String2String & params = {});
private:
    AGG_LOG_DECLARE();
};

AGG_END_NAMESPACE(data);

#endif // AGG_WEB_DATA_DATA_FACTORY_H

