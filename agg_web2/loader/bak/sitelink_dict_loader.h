#ifndef AGG_WEB_LOADER_SITELINK_DICT_LOADER_H
#define AGG_WEB_LOADER_SITELINK_DICT_LOADER_H

#include <agg_web2/common/common.h>
#include <agg_web2/data/sitelink_dict.h>
#include <A4/data_engine/data_loader.h>

AGG_BEGIN_NAMESPACE(agg_web2);

class SitelinkDictLoader : public DataLoader
{
public:
    SitelinkDictLoader();
    virtual ~SitelinkDictLoader();
public:
    virtual bool Init(const String2String& params);
    virtual DataLoader* Clone();
    virtual void Destroy();

public:
    virtual UserData* Load(const std::string& filePath);

private:
    SitelinkHit* LoadSitelinkHit(std::ifstream& fin, std::string& line);
    void OmitBlankLine(std::ifstream& fin, std::string& line);

private:
    AGG_LOG_DECLARE();

private:
    DECLARE_SERIALIZATION_FUNC();
};

AGG_TYPEDEF_PTR(SitelinkDictLoader);

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_KEY(AGG_NS(loader)::SitelinkDictLoader);

#endif //AGG_WEB_LOADER_SITELINK_DICT_LOADER_H
