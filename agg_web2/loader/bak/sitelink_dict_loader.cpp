#include <agg_web2/loader/sitelink_dict_loader.h>
#include <A4/util/string_tokenizer.h>
#include <A4/util/string_util.h>


AGG_BEGIN_NAMESPACE(agg_web2);
AGG_LOG_SETUP(agg_web2,SitelinkDictLoader);

BEGIN_DEFINE_SERIALIZATION_FUNC(SitelinkDictLoader)
    // ar & BOOST_SERIALIZATION_NVP(mField);
END_DEFINE_SERIALIZATION_FUNC(SitelinkDictLoader)

SitelinkDictLoader::SitelinkDictLoader()
{
}

SitelinkDictLoader::~SitelinkDictLoader()
{
}

bool SitelinkDictLoader::Init(const String2String& params)
{
    return true;
}

DataLoader* SitelinkDictLoader::Clone()
{
    return new SitelinkDictLoader(*this);
}

void SitelinkDictLoader::Destroy()
{
    delete this;
}

UserData* SitelinkDictLoader::Load(const std::string& filePath)
{
    std::ifstream fin(filePath.c_str());
    if (!fin) {
        AGG_LOG(ERROR, "open file [%s] failed", filePath.c_str());
        return NULL;
    }

    SitelinkDict* dict = new SitelinkDict;
    std::string line;
    do {
        OmitBlankLine(fin, line);
        if (fin) {
            SitelinkHit *hit = LoadSitelinkHit(fin, line);
            if (hit == NULL) {
                AGG_LOG(ERROR, "invalid site link hit");
                A4_DELETE_AND_SET_NULL(dict);
                return NULL;
            }
            dict->Set(hit->GetMainUrl(), hit);
        }
    } while (fin);

    return dict;
}

SitelinkHit* SitelinkDictLoader::LoadSitelinkHit(std::ifstream& fin, std::string& line)
{
    StringTokenizer tokenizer;
    size_t count = tokenizer.Tokenize(line, "\t", 
            StringTokenizer::TOKEN_IGNORE_EMPTY | StringTokenizer::TOKEN_TRIM);
    if (count != 2 && count != 3) {
        AGG_LOG(ERROR, "illegal line[%s]", line.c_str());
        return NULL;
    }
    SitelinkHit *mainHit = new SitelinkHit;
    mainHit->SetMainUrl(tokenizer[1]);
    mainHit->SetMainTitle(tokenizer[0]);
    if(count == 3){
        mainHit->SetOwFlag(tokenizer[2]); 
    }
    while (getline(fin, line)) {
        if (line.empty()) {
            return mainHit;
        }
        StringTokenizer itemTokenizer;
        size_t itemCount = itemTokenizer.Tokenize(line, "\t", 
                StringTokenizer::TOKEN_IGNORE_EMPTY | StringTokenizer::TOKEN_TRIM);
        if (itemCount != 2) {
            AGG_LOG(ERROR, "illegal line[%s]", line.c_str());
            A4_DELETE_AND_SET_NULL(mainHit);
            return NULL;
        }
        mainHit->AddItem(SitelinkHit::SitelinkItem(itemTokenizer[0], itemTokenizer[1]));
    }

    return mainHit;
}

void SitelinkDictLoader::OmitBlankLine(std::ifstream& fin, std::string& line)
{
    while (getline(fin, line)) {
        StringUtil::TrimString(line);
        if (!line.empty()) {
            break;
        }
    }
}

AGG_END_NAMESPACE(agg_web2);
BOOST_CLASS_EXPORT_IMPLEMENT(AGG_NS(loader)::SitelinkDictLoader);

