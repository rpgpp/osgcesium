/* iowin32.c -- IO base function header for compress/uncompress .zip
   files using zlib + zip or unzip API
   This IO API version uses the Win32 API (for Microsoft Windows)

   Version 1.01e, February 12th, 2005

   Copyright (C) 1998-2005 Gilles Vollant
*/

#include <stdlib.h>
#include "zlib.h"
#include "ioapi.h"
#include "iostd.h"

#include <sstream>

#if _WIN32

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE (0xFFFFFFFF)
#endif

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

voidpf ZCALLBACK std_open_file_func OF((
    voidpf opaque,
    const char* filename,
    int mode));

uLong ZCALLBACK std_read_file_func OF((
    voidpf opaque,
    voidpf stream,
    void* buf,
    uLong size));

uLong ZCALLBACK std_write_file_func OF((
    voidpf opaque,
    voidpf stream,
    const void* buf,
    uLong size));

long ZCALLBACK std_tell_file_func OF((
    voidpf opaque,
    voidpf stream));

long ZCALLBACK std_seek_file_func OF((
    voidpf opaque,
    voidpf stream,
    uLong offset,
    int origin));

int ZCALLBACK std_close_file_func OF((
    voidpf opaque,
    voidpf stream));

int ZCALLBACK std_error_file_func OF((
    voidpf opaque,
    voidpf stream));

typedef struct
{
    HANDLE hf;
    int error;
} WIN32FILE_IOWIN;
#endif

voidpf ZCALLBACK std_open_file_func(voidpf opaque, const char* filename, int mode)
{
    return opaque;
}


uLong ZCALLBACK std_read_file_func(voidpf opaque, voidpf stream, void* buf, uLong size)
{
    uLong ret = 0;

    return ret;
}


uLong ZCALLBACK std_write_file_func(voidpf opaque, voidpf stream, const void* buf, uLong size)
{
    ((std::ostream*)stream)->write((const char*)buf,size);
    return size;
}

long ZCALLBACK std_tell_file_func(voidpf opaque, voidpf stream)
{
    long ret = -1;
    ret = (long)((std::ostream*)stream)->tellp();
    return ret;
}

long ZCALLBACK std_seek_file_func(voidpf opaque, voidpf stream, uLong offset, int origin)
{
    long ret = 0;
    #if _WIN32
    ((std::ostream*)stream)->seekp(offset, origin);
    #else
    ((std::ostream*)stream)->seekp(offset);
    #endif
    return ret;
}

int ZCALLBACK std_close_file_func(voidpf opaque,voidpf stream)
{
    //delete ((std::ostream*)opaque);
    return 0;
}

int ZCALLBACK std_error_file_func(voidpf opaque, voidpf stream)
{
    int ret = -1;
    return ret;
}


void fill_std_filefunc(zlib_filefunc_def* pzlib_filefunc_def)
{
    pzlib_filefunc_def->zopen_file = std_open_file_func;
    pzlib_filefunc_def->zread_file = std_read_file_func;
    pzlib_filefunc_def->zwrite_file = std_write_file_func;
    pzlib_filefunc_def->ztell_file = std_tell_file_func;
    pzlib_filefunc_def->zseek_file = std_seek_file_func;
    pzlib_filefunc_def->zclose_file = std_close_file_func;
    pzlib_filefunc_def->zerror_file = std_error_file_func;
    pzlib_filefunc_def->opaque = new std::stringstream;
}
