// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <memory>
#include <cstdlib>
#include <cstdio>

#include <3ds.h>

#include "fs_common.h"

#define TITLE_PATH "/3dsutils/nand/00000000000000000000000000000000/title/"

namespace SaveDataCheck {

std::string BuildSharedRomFSDirname(u32* lowpath) {
    char* filename_buffer;
    asprintf(&filename_buffer, "%s/%08lx/%08lx/content/", TITLE_PATH, lowpath[1], lowpath[0]);
    std::string filename(filename_buffer);
    free(filename_buffer);
    return filename;
}

void DumpSharedRomFS(u32* archive_binary_lowpath) {
    std::string output_dir = BuildSharedRomFSDirname(archive_binary_lowpath);
    SanitizeSeparators(&output_dir);
    if (!CreateFullPath(output_dir)) {
        printf("Creating path (%s) failed! Aborting!\n", output_dir.c_str());
        return;
    }
    std::string output_file = output_dir + "00000000.app.romfs";

    // Read RomFS bin from SaveDataCheck...

    Handle romfs_handle;
    u64    romfs_size        = 0;
    u32    romfs_bytes_read  = 0;

    FS_Path    savedatacheck_path       = { PATH_BINARY, 16, (u8*)archive_binary_lowpath };
    u8         file_binary_lowpath[20]  = {};
    FS_Path    romfs_path               = { PATH_BINARY, 20, file_binary_lowpath };

    printf("Dumping SaveDataCheck RomFS (%s)... ", output_file.c_str());

    FSUSER_OpenFileDirectly(&romfs_handle, (FS_ArchiveID)0x2345678a, savedatacheck_path, romfs_path, FS_OPEN_READ, 0);
    FSFILE_GetSize(romfs_handle, &romfs_size);

    std::unique_ptr<u8> romfs_data_buffer(new u8[romfs_size]);
    FSFILE_Read(romfs_handle, &romfs_bytes_read, 0, romfs_data_buffer.get(), romfs_size);
    FSFILE_Close(romfs_handle);

    // Dump RomFS bin to SDMC...

    FILE* out_file = fopen(output_file.c_str(), "wb");
    size_t bytes_written = fwrite(romfs_data_buffer.get(), 1, romfs_size, out_file);
    fclose(out_file);

    if (bytes_written == romfs_size)
        printf("Done!\n");
    else
        printf("Failed!\n");
}

void Dump() {
    // savedatacheck/000400db00010302.bin
    u64 binary_lowpath_000400db00010302[] = { 0x000400db00010302, 0x00000001ffffff00 };
    DumpSharedRomFS((u32*)binary_lowpath_000400db00010302);

    // savedatacheck/0004009b00010202.bin
    u64 binary_lowpath_0004009b00010202[] = { 0x0004009b00010202, 0x00000002ffffff00 };
    DumpSharedRomFS((u32*)binary_lowpath_0004009b00010202);

    // savedatacheck/0004009b00010402.bin
    u64 binary_lowpath_0004009b00010402[] = { 0x0004009b00010402, 0x00000001ffffff00 };
    DumpSharedRomFS((u32*)binary_lowpath_0004009b00010402);

    // savedatacheck/0004009b00014002.bin
    u64 binary_lowpath_0004009b00014002[] = { 0x0004009b00014002, 0x00000001ffffff00 };
    DumpSharedRomFS((u32*)binary_lowpath_0004009b00014002);
    
    // savedatacheck/0004009b00014102.bin
    u64 binary_lowpath_0004009b00014102[] = { 0x0004009b00014102, 0x00000001ffffff00 };
    DumpSharedRomFS((u32*)binary_lowpath_0004009b00014102);
    
    // savedatacheck/0004009b00014202.bin
    u64 binary_lowpath_0004009b00014202[] = { 0x0004009b00014202, 0x00000001ffffff00 };
    DumpSharedRomFS((u32*)binary_lowpath_0004009b00014202);
    
    // savedatacheck/0004009b00014302.bin
    u64 binary_lowpath_0004009b00014302[] = { 0x0004009b00014302, 0x00000001ffffff00 };
    DumpSharedRomFS((u32*)binary_lowpath_0004009b00014302);
}

} // namespace
