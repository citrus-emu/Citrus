#pragma once

#include <string>

#include <3ds.h>

void InitOutput();

/// Prints `text` to `screen`.
void Print(const std::string& text);

/// Prints `text` to `screen`, and logs it in the log file.
void Log(const std::string& text);

/// Logs `text` to the log file.
void LogToFile(const std::string& text);

void DeinitOutput();
