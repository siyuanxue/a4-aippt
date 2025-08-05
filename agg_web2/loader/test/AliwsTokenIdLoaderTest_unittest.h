#ifndef AGG2_LOADER_ALIWSTOKENIDLOADERTESTTEST_H
#define AGG2_LOADER_ALIWSTOKENIDLOADERTESTTEST_H

#include <agg_web2/common/common.h>
#include <agg_web2/loader/aliws_token_id_loader.h>
#include <cppunit/extensions/HelperMacros.h>
#include "test/test.h"
AGG_BEGIN_NAMESPACE(loader);

class AliwsTokenIdLoaderTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(AliwsTokenIdLoaderTest);
    CPPUNIT_TEST(TestSimpleProcess);
    CPPUNIT_TEST_SUITE_END();
public:
    AliwsTokenIdLoaderTest();
    ~AliwsTokenIdLoaderTest();
public:
    void setUp();
    void tearDown();
    void TestSimpleProcess();
private:
    AGG_LOG_DECLARE();
};

AGG_END_NAMESPACE(loader);

#endif //AGG2_LOADER_ALIWSTOKENIDLOADERTESTTEST_H
