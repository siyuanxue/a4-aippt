#ifndef AGG2_AGG_WEB_BASE_TEST_H
#define AGG2_AGG_WEB_BASE_TEST_H

#include <A4/common/define.h>
#include <A4/common/common.h>
#include <agg2/common/define.h>
#include <agg2/common/agg_log.h>
#include <cppunit/extensions/HelperMacros.h>

AGG_BEGIN_NAMESPACE(test);

class BaseTest: public CppUnit::TestFixture
{
public:
    BaseTest();
    ~BaseTest();
public:

private:
    AGG_LOG_DECLARE();
};

typedef std::tr1::shared_ptr<BaseTest> BaseTestPtr;

AGG_END_NAMESPACE(test);

#endif //AGG2_AGG_WEBBASE_TEST_H
