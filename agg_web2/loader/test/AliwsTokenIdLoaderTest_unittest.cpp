#include <agg_web2/loader/test/AliwsTokenIdLoaderTest_unittest.h>
#include <agg_web2/data/aliws_token_id_data.h>
#include <cppunit/TestAssert.h>
using namespace std;
AGG_BEGIN_NAMESPACE(loader);
AGG_LOG_SETUP(loader, AliwsTokenIdLoaderTest);

CPPUNIT_TEST_SUITE_REGISTRATION(AliwsTokenIdLoaderTest);


AliwsTokenIdLoaderTest::AliwsTokenIdLoaderTest()
{
}

AliwsTokenIdLoaderTest::~AliwsTokenIdLoaderTest()
{
    AliwsTokenIdLoader loader;
    AliwsTokenIdData* data = dynamic_cast<AliwsTokenIdData*>(loader.Load(TEST_DATA_PATH"/biz/index/dnn_word_map"));
    if (data == NULL) return;
    const std::map<std::string, std::string>& token2IdMap = data->GetToken2IdMap();
    const std::map<std::string, std::string>& id2TokenMap = data->GetId2TokenMap();
    
    std::string id = token2IdMap.find("你")->second;
    std::string token = id2TokenMap.find(id)->second;
    cout << id << " " << token << endl;
}

void AliwsTokenIdLoaderTest::setUp()
{
    AGG_LOG(INFO, "AliwsTokenIdLoaderTest setUp\n");
}

void AliwsTokenIdLoaderTest::tearDown()
{
    AGG_LOG(INFO, "AliwsTokenIdLoaderTest tearDown\n\n");
}

void AliwsTokenIdLoaderTest::TestSimpleProcess()
{
}

AGG_END_NAMESPACE(loader);

