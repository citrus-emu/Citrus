#include "draw.h"
#include "hid.h"
#include "i2c.h"
#include "fatfs/ff.h"
#include "gamecart/protocol.h"
#include "gamecart/command_ctr.h"
#include "headers.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h> //For memalign

extern s32 CartID;
extern s32 CartID2;

// File IO utility functions
static FATFS fs;
static FIL file;

static void ClearTop(void) {
    ClearScreen(TOP_SCREEN0, RGB(255, 255, 255));
    ClearScreen(TOP_SCREEN1, RGB(255, 255, 255));
    current_y = 0;
}

static void wait_key(void) {
    Debug("Press key to continue...");
    InputWait();
}

static void Reboot()
{
    i2cWriteRegister(I2C_DEV_MCU, 0x20, 1 << 2);
    while(true);
}

struct Context {
    u8* buffer;
    size_t buffer_size;

    u32 cart_size;
    u32 media_unit;
};

static int dump_cart_region(u32 start_sector, u32 end_sector, FIL* output_file, struct Context* ctx) {
    u32 read_size = 1u * 1024 * 1024 / ctx->media_unit; // 1MiB default

    // Dump remaining data
    u32 current_sector = start_sector;
    while (current_sector < end_sector) {
        unsigned int percentage = current_sector * 100 / ctx->cart_size;
        Debug("Dumping %08X / %08X - %3u%%", current_sector, ctx->cart_size, percentage);

        u8* read_ptr = ctx->buffer;
        while (read_ptr < ctx->buffer + ctx->buffer_size && current_sector < end_sector) {
            Cart_Dummy();
            Cart_Dummy();

            // If there is less data to read than the current read_size, fix it
            if (end_sector - current_sector < read_size)
            {
                read_size = end_sector - current_sector;
            }
            CTR_CmdReadData(current_sector, ctx->media_unit, read_size, read_ptr);
            read_ptr += ctx->media_unit * read_size;
            current_sector += read_size;
        }

        u8* write_ptr = ctx->buffer;
        while (write_ptr < read_ptr) {
            unsigned int bytes_written = 0;
            f_write(output_file, write_ptr, (size_t)(read_ptr - write_ptr), &bytes_written);
            Debug("Wrote 0x%x bytes...", bytes_written);

            if (bytes_written == 0) {
                Debug("Writing failed! :( SD full?");
                return -1;
            }

            write_ptr += bytes_written;
        }
    }

    return 0;
}

int main() {
    // Saves the framebuffer information somewhere safe.
    DrawInit();

    // Arbitrary target buffer
    // aligning to 32 bits in case other parts of the software assume alignment
    const u32 target_buf_size = 16u * 1024u * 1024u; // 16MB
    u32* const target = memalign(4, target_buf_size);

    u32* const ncchHeaderData = memalign(4, sizeof(NCCH_HEADER));
    NCCH_HEADER* const ncchHeader = (NCCH_HEADER*)ncchHeaderData;

    NCSD_HEADER* const ncsdHeader = (NCSD_HEADER*)target;

restart_program:
    // Setup boring stuff - clear the screen, initialize SD output, etc...
    ClearTop();

    Debug("Uncart: ROM dump tool v0.2");
    Debug("Insert your game cart now.");
    wait_key();

    memset(target, 0, target_buf_size); // Clear our buffer

    // ROM DUMPING CODE STARTS HERE

    Cart_Init();
    Debug("Cart id is %08x", Cart_GetID());
    Debug("Reading NCCH header...");
    CTR_CmdReadHeader(ncchHeader);
    Debug("Done reading NCCH header.");

    // Check that the NCCH header magic is there
    if (strncmp((const char*)(ncchHeader->magic), "NCCH", 4)) {
        Debug("NCCH magic not found in header!!!");
        Debug("Press A to continue anyway.");
        if (!(InputWait() & BUTTON_A))
            goto restart_prompt;
    }

    u32 sec_keys[4];
    Cart_Secure_Init(ncchHeaderData, sec_keys);

    // Guess 0x200 first for the media size. this will be set correctly once the cart header is read
    // Read out the header 0x0000-0x1000
    Cart_Dummy();
    Debug("Reading NCSD header...");
    CTR_CmdReadData(0, 0x200, 0x1000 / 0x200, target);
    Debug("Done reading NCSD header.");

    // Check for NCSD magic
    if (strncmp((const char*)(ncsdHeader->magic), "NCSD", 4)) {
        Debug("NCSD magic not found in header!!!");
        Debug("Press A to continue anyway.");
        if (!(InputWait() & BUTTON_A))
            goto restart_prompt;
    }

    Debug("Uncart can either dump the entire ROM (including");
    Debug("empty space), or a trimmed version based on the");
    Debug("size of the cart partitions.");
    Debug("");

    u32 input;
    do {
        Debug("Press A to dump all of ROM, B for only the");
        Debug("trimmed version.");
        input = InputWait();
    }
    while (!(input & BUTTON_A) && !(input & BUTTON_B));


    const u32 mediaUnit = 0x200 * (1u << ncsdHeader->partition_flags[MEDIA_UNIT_SIZE]); //Correctly set the media unit size

    u32 cartSize;
    // Maximum number of blocks in a single file
    u32 file_max_blocks;

    if (input & BUTTON_B) {
        // Calculate the actual size by counting the adding the size of each
        // partition, plus the initial offset size is in media units

        // The 3DS carts have up to 8 partitions in their carts
        cartSize = ncsdHeader->offsetsize_table[0].offset;
        for(size_t i = 0; i < 8; i++) {
            cartSize += ncsdHeader->offsetsize_table[i].size;
        }

        Debug("Cart data size: %llu MB", (u64)cartSize * (u64)mediaUnit / 1024ull / 1024ull);
        // Maximum number of blocks in a single file
        file_max_blocks = 0xFFFFFFFFu / mediaUnit + mediaUnit; // 4GiB - 512
    }
    else
    {
        cartSize = ncsdHeader->media_size;
        // Maximum number of blocks in a single file
        file_max_blocks = 0x80000000u / mediaUnit; // 2GiB
    }

    struct Context context = {
        .buffer = (u8*)target,
        .buffer_size = target_buf_size,
        .cart_size = cartSize,
        .media_unit = mediaUnit,
    };

    u32 current_part = 0;

    while (current_part * file_max_blocks < cartSize) {
        // Create output file
        char filename_buf[32];
        char extension_digit = cartSize <= file_max_blocks ? 's' : '0' + current_part;
        snprintf(filename_buf, sizeof(filename_buf), "/%.16s.3d%c", ncchHeader->product_code, extension_digit);
        Debug("Writing to file: \"%s\"", filename_buf);
        Debug("Change the SD card now and/or press a key.");
        Debug("(Or SELECT to cancel)");
        if (InputWait() & BUTTON_SELECT)
            break;

        if (f_mount(&fs, "0:", 0) != FR_OK) {
            Debug("Failed to f_mount... Retrying");
            wait_key();
            goto cleanup_none;
        }

        if (f_open(&file, filename_buf, FA_READ | FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
            Debug("Failed to create file... Retrying");
            wait_key();
            goto cleanup_mount;
        }

        f_lseek(&file, 0);

        u32 region_start = current_part * file_max_blocks;
        u32 region_end = region_start + file_max_blocks;
        if (region_end > cartSize)
            region_end = cartSize;

        if (dump_cart_region(region_start, region_end, &file, &context) < 0)
            goto cleanup_file;

        if (current_part == 0) {
            // Write header - TODO: Not sure why this is done at the very end..
            f_lseek(&file, 0x1000);
            unsigned int written = 0;
            // Fill the 0x1200-0x4000 unused area with 0xFF instead of random garbage.
            memset((u8*)ncchHeaderData + 0x200, 0xFF, 0x3000 - 0x200);
            f_write(&file, ncchHeader, 0x3000, &written);
        }

        Debug("Done!");
        current_part += 1;

cleanup_file:
        // Done, clean up...
        f_sync(&file);
        f_close(&file);
cleanup_mount:
        f_mount(NULL, "0:", 0);
cleanup_none:
        ;
    }

restart_prompt:
    Debug("Press B to exit, any other key to restart.");
    if (!(InputWait() & BUTTON_B))
        goto restart_program;

    free(ncchHeaderData);
    free(target);

    Reboot();
    return 0;
}
