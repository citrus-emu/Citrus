#include "test.h"

#include <3ds.h>

#include "output.h"
#include "common/string_funcs.h"

void SoftAssertLog(const std::string& function, int line, const std::string& condition)
{
    Log(Common::FormatString("SOFTASSERT FAILURE: `%s`\n", condition.c_str()));
    Log(Common::FormatString("    At `%s` L%i\n", function.c_str(), line));
}

void PrintSuccess(const std::string& group, const std::string& name, bool val)
{
    Log(Common::FormatString("%s: [%s] %s\n", val ? "SUCCESS" : "FAILURE", group.c_str(), name.c_str()));
}
