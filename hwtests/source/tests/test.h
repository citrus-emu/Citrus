#pragma once

#include <string>
#include <sstream>

typedef void (*TestCaller)(void);

void SoftAssertLog(const std::string& function, int line, const std::string& condition);

// If the condition fails, return false
#define SoftAssert(cond) \
    do { \
        if (!(cond)) { \
            SoftAssertLog(__PRETTY_FUNCTION__, __LINE__, #cond); \
            return false; \
        } \
    } while (0)

#define TestEquals(actual, expected) \
    do { \
        auto var_actual = (actual); \
        auto var_expected = (expected); \
        if (!(var_actual == var_expected)) { \
            std::ostringstream ss; \
            ss << std::hex << #actual << "\nexpected [" << var_expected << "]\ngot [" << var_actual << "]"; \
            SoftAssertLog(__PRETTY_FUNCTION__, __LINE__, ss.str()); \
            return false; \
        } \
    } while (0)

void PrintSuccess(const std::string& group, const std::string& name, bool val);

template <typename T>
bool Test(const std::string& group, const std::string& name, T result, T expected)
{
    PrintSuccess(group, name, result == expected);
    return result == expected;
}
