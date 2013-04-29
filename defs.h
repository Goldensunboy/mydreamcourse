#ifndef DEFS_H
#define DEFS_H

#include "qbytearray.h"

typedef unsigned char byte;
typedef unsigned short u16;
typedef unsigned int u32;

// Converts an SNES address from a location in the ROM into an offset
#define ADDRESS(a) (((byte)file[a+header+2]-0x80)*0x8000+((byte)file[a+header+1]-0x80)*0x100+(byte)file[a+header]+header)

// The currently opened file
extern QByteArray file;

// The header size added to addresses
extern u16 header;

// Debug printing routine
void debugPrint(QByteArray array);
void debugPrint(QByteArray array, int len);

#endif // DEFS_H
