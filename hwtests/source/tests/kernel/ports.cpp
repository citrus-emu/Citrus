#include <memory>
#include <cstring>
#include <3ds.h>

#include "common/scope_exit.h"
#include "tests/test.h"

namespace Kernel {
namespace Ports {

static bool Test_ConnectToPort() {
    const Result ERR_NOT_FOUND = 0xD88007FA;
    const Result ERR_PORT_NAME_TOO_LONG = 0xE0E0181E;
    Handle handle;

    // Test for correct Result codes in various situations
    // NULL port name
    TestEquals(svcConnectToPort(&handle, nullptr), ERR_NOT_FOUND);
    // empty port name
    TestEquals(svcConnectToPort(&handle, ""), ERR_NOT_FOUND);
    // port name too long
    TestEquals(svcConnectToPort(&handle, "SuperExtremelyUltraMegaVeryLongString"), ERR_PORT_NAME_TOO_LONG);
    TestEquals(svcConnectToPort(&handle, "0123456789A"), ERR_NOT_FOUND); // Just right (11 characters)
    TestEquals(svcConnectToPort(&handle, "0123456789AB"), ERR_PORT_NAME_TOO_LONG); // too long
    // non-registered port name
    TestEquals(svcConnectToPort(&handle, "xyz:"), ERR_NOT_FOUND);
    // connecting to srv services using ConnectToPort should fail
    TestEquals(svcConnectToPort(&handle, "APT:U"), ERR_NOT_FOUND);

    // Connecting to "srv:" should succeed
    TestEquals(svcConnectToPort(&handle, "srv:"), 0);

    return true;
}

void TestAll() {
    Test("Kernel::Ports", "ConnectToPort", Test_ConnectToPort(), true);
}

} // namespace
} // namespace
