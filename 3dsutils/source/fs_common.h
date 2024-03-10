// Copyright 2013 Dolphin Emulator Project / 2014-2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <string>

void SanitizeSeparators(std::string* path);
bool CreateFullPath(const std::string& path);
