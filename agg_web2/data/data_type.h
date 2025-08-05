#ifndef AGG_WEB_DATA_DATA_TYPE_H
#define AGG_WEB_DATA_DATA_TYPE_H

#include <agg2/data/map_user_data.h>
#include <agg2/data/map_vec_user_data.h>

AGG_BEGIN_NAMESPACE(agg_web2);

typedef MapUserData<std::string, int> QcBertTokenData;
typedef MapVecUserData<std::string, std::vector<std::string>> HostNameLogoData;

AGG_END_NAMESPACE(agg_web2);

#endif //AGG_WEB_DATA_DATA_TYPE_H
