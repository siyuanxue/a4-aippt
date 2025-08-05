#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <A4/common/common.h>
#include <A4/common/log.h>
#include <test/test.h>

using namespace std;
using namespace CppUnit;

int main(int argc, char* argv[])
{
    A4_LOG_CONFIG(DOTEST_LOGGER_CONF);
    TextUi::TestRunner runner;
    runner.setOutputter(new CompilerOutputter(&runner.result(), std::cerr));
    TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    bool ok = runner.run("", false);
    A4_LOG_SHUTDOWN();
    return ok ? 0 : 1;
}
