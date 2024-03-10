#include <3ds.h>

#include "output.h"

#include <fstream>

//#include "common/string_funcs.h"

static FILE* log_file = nullptr;

void InitOutput()
{
    sdmcInit();
    consoleInit(GFX_TOP, nullptr);
    log_file = fopen("hwtest_log.txt", "w");
}

void Print(const std::string& text)
{
    printf("%s", text.c_str());
    gfxFlushBuffers();
    gfxSwapBuffers();
}

void Log(const std::string& text)
{
    Print(text);
    LogToFile(text);
}

void LogToFile(const std::string& text)
{
    svcOutputDebugString(text.c_str(), text.length());
    fprintf(log_file, "%s", text.c_str());
    fflush(log_file);
}

void DeinitOutput()
{
    fclose(log_file);
    log_file = nullptr;
    sdmcExit();
}
