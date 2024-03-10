// Copyright 2013 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "font.h"
#include "draw.h"

u8 *TOP_SCREEN0;
u8 *TOP_SCREEN1;
u8 *BOT_SCREEN0;
u8 *BOT_SCREEN1;

size_t current_y = 0;

void DrawInit(void)
{
#ifdef BRAHMA
    TOP_SCREEN0 = (u8*)(0x20000000);
    TOP_SCREEN1 = (u8*)(0x20046500);
    BOT_SCREEN0 = (u8*)(0x2008CA00);
    BOT_SCREEN1 = (u8*)(0x200C4E00);
#elif A9LH
    TOP_SCREEN0 = (u8*)(*(u32*)0x23FFFE00);
    TOP_SCREEN1 = (u8*)(*(u32*)0x23FFFE00);
    BOT_SCREEN0 = (u8*)(*(u32*)0x23FFFE08);
    BOT_SCREEN1 = (u8*)(*(u32*)0x23FFFE08);
#else
	#error "BRAHMA or A9LH must be defined!"
#endif
}

void ClearScreen(unsigned char *screen, int color)
{
    int i;
    unsigned char *screenPos = screen;
    for (i = 0; i < (SCREEN_HEIGHT * SCREEN_WIDTH); i++) {
        *(screenPos++) = color >> 16;  // B
        *(screenPos++) = color >> 8;   // G
        *(screenPos++) = color & 0xFF; // R
    }
}

void DrawCharacter(unsigned char *screen, int character, size_t x, size_t y, int color, int bgcolor)
{
    for (size_t yy = 0; yy < 8; yy++) {
        size_t xDisplacement = (x * BYTES_PER_PIXEL * SCREEN_WIDTH);
        size_t yDisplacement = ((SCREEN_WIDTH - (y + yy) - 1) * BYTES_PER_PIXEL);

        unsigned char *screenPos = screen + xDisplacement + yDisplacement;
        unsigned char charPos = font[(size_t)character * 8 + yy];
        for (int xx = 7; xx >= 0; xx--) {
            if ((charPos >> xx) & 1) {
                *(screenPos + 0) = color >> 16;  // B
                *(screenPos + 1) = color >> 8;   // G
                *(screenPos + 2) = color & 0xFF; // R
            } else {
                *(screenPos + 0) = bgcolor >> 16;  // B
                *(screenPos + 1) = bgcolor >> 8;   // G
                *(screenPos + 2) = bgcolor & 0xFF; // R
            }
            screenPos += BYTES_PER_PIXEL * SCREEN_WIDTH;
        }
    }
}

void DrawString(unsigned char *screen, const char *str, size_t x, size_t y, int color, int bgcolor)
{
    const size_t string_len = strlen(str);

    for (size_t i = 0; i < string_len; i++)
        DrawCharacter(screen, str[i], x + i * 8, y, color, bgcolor);
}

void DrawStringF(size_t x, size_t y, const char *format, ...)
{
    char str[256];
    va_list va;

    va_start(va, format);
    vsnprintf(str, 256, format, va);
    va_end(va);

    DrawString(TOP_SCREEN0, str, x, y, RGB(0, 0, 0), RGB(255, 255, 255));
    DrawString(TOP_SCREEN1, str, x, y, RGB(0, 0, 0), RGB(255, 255, 255));
}

void Debug(const char *format, ...)
{
    char str[51];
    const char* spaces = "                                                 X";
    va_list va;

    va_start(va, format);
    vsnprintf(str, sizeof(str), format, va);
    va_end(va);
    snprintf(str, sizeof(str), "%s%s", str, spaces);

    DrawString(TOP_SCREEN0, str, 0u, current_y, RGB(255, 0, 0), RGB(255, 255, 255));
    DrawString(TOP_SCREEN0, spaces, 0u, current_y + 10, RGB(255, 0, 0), RGB(255, 255, 255));
    DrawString(TOP_SCREEN1, str, 0u, current_y, RGB(255, 0, 0), RGB(255, 255, 255));
    DrawString(TOP_SCREEN1, spaces, 0u, current_y + 10, RGB(255, 0, 0), RGB(255, 255, 255));

    current_y += 10;
    if (current_y >= 240) {
        current_y = 0;
    }
}
