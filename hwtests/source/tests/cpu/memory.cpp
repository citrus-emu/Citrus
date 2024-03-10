#include <limits>
#include <string>
#include "output.h"
#include "common/string_funcs.h"
#include "tests/test.h"

namespace CPU {
namespace Memory {

// LDRSH
static bool Ldrsh() {
    u32 input = 0x0000F5E3;
    u32 output;
    asm volatile ("LDRSH %[out], %[in]" : [out] "=r"(output) : [in] "m"(input));
    SoftAssert(output == 0xFFFFF5E3);

    return true;
}

// STRH
static bool Strh() {
    u32 input = 0xFFFFF5E3;
    u32 output[2] = { 0x12121212, 0x12121212 };
    asm volatile ("STRH %[in], %[out]" : [out] "+m"(output[0]) : [in] "r"(input) : "memory");
    SoftAssert(output[0] == 0x1212F5E3);
    SoftAssert(output[1] == 0x12121212);

    return true;
}

void TestAll() {
    const std::string tag = "Memory";

    Test(tag, "LDRSH", Ldrsh(), true);
    Test(tag, "STRH",  Strh(),  true);
}

}
}
