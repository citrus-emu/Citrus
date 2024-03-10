// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>

#include "utils/savedatacheck/savedatacheck.h"

static unsigned int util_counter = 0;
static void (*utils[]) (void) = {
    SaveDataCheck::Dump,
};

int main()
{
    aptInit();
    gfxInitDefault();
    consoleInit(GFX_TOP, nullptr);

    consoleClear();
    printf("Press A to begin...\n");

    while (aptMainLoop()) {
        gfxFlushBuffers();
        gfxSwapBuffers();

        hidScanInput();
        if (hidKeysDown() & KEY_START) {
            break;
        } else if (hidKeysDown() & KEY_A) {
            consoleClear();

            if (util_counter < (sizeof(utils) / sizeof(utils[0]))) {
                utils[util_counter]();
                util_counter++;
            } else {
                break;
            }

            printf("\n");
            printf("Press A to continue...\n");
        }

        gspWaitForEvent(GSPGPU_EVENT_VBlank0, false);
    }

    consoleClear();

    gfxExit();
    aptExit();
    return 0;
}
