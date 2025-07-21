// Matthias Wandel
// http://www.sentex.net/~mwandel/jhead/
//
//--------------------------------------------------------------------------
// Parse some maker specific onformation.
// (Very limited right now - add maker specific stuff to this module)
//--------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include "jhead.h"

//--------------------------------------------------------------------------
// Process exif format directory, as used by Canon maker note
//--------------------------------------------------------------------------
void ProcessCanonMakerNoteDir(unsigned char * DirStart, unsigned char * OffsetBase, 
        unsigned ExifLength)
{
    int de;
    int NumDirEntries;
    NumDirEntries = Get16u(DirStart);
    #define DIR_ENTRY_ADDR(Start, Entry) (Start+2+12*(Entry))
    {
        unsigned char * DirEnd;
        DirEnd = DIR_ENTRY_ADDR(DirStart, NumDirEntries);
        if (DirEnd > (OffsetBase+ExifLength)){
            ErrNonfatal("Illegally sized directory",0,0);
            return;
        }
    }
    for (de=0;de<NumDirEntries;de++){
        int Tag, Format, Components;
        unsigned char * ValuePtr;
        int ByteCount;
        unsigned char * DirEntry;
        DirEntry = DIR_ENTRY_ADDR(DirStart, de);
        Tag = Get16u(DirEntry);
        Format = Get16u(DirEntry+2);
        Components = Get32u(DirEntry+4);
        if ((Format-1) >= NUM_FORMATS) {
            // (-1) catches illegal zero case as unsigned underflows to positive large.
            ErrNonfatal("Illegal number format %d for tag %04x", Format, Tag);
            continue;
        }
        if ((unsigned)Components > 0x10000){
            ErrNonfatal("Illegal number of components %d for tag %04x", Components, Tag);
            continue;
        }
        ByteCount = Components * BytesPerFormat[Format];
        if (ByteCount > 4){
            unsigned OffsetVal;
            OffsetVal = Get32u(DirEntry+8);
            // If its bigger than 4 bytes, the dir entry contains an offset.
            if (OffsetVal+ByteCount > ExifLength){
                // Bogus pointer offset and / or bytecount value
                ErrNonfatal("Illegal value pointer for tag %04x", Tag,0);
                continue;
            }
            ValuePtr = OffsetBase+OffsetVal;
        }else{
            // 4 bytes or less and value is in the dir entry itself
            ValuePtr = DirEntry+8;
        }
        if (Tag == 1 && Components > 16){
            int IsoCode = Get16u(ValuePtr + 16*sizeof(unsigned short));
            if (IsoCode >= 16 && IsoCode <= 24){
                ImageInfo.ISOequivalent = 50 << (IsoCode-16);
            } 
        }
        if (Tag == 4 && Format == FMT_USHORT){
            if (Components > 7){
                int WhiteBalance = Get16u(ValuePtr + 7*sizeof(unsigned short));
                switch(WhiteBalance){
                    // 0=Auto, 6=Custom
                    case 1: ImageInfo.LightSource = 1; break; // Sunny
                    case 2: ImageInfo.LightSource = 1; break; // Cloudy
                    case 3: ImageInfo.LightSource = 3; break; // Thungsten
                    case 4: ImageInfo.LightSource = 2; break; // Fourescent
                    case 5: ImageInfo.LightSource = 4; break; // Flash
                }
            }
            if (Components > 19 && ImageInfo.Distance <= 0) {
                // Inidcates the distance the autofocus camera is focused to.
                // Tends to be less accurate as distance increases.
                int temp_dist = Get16u(ValuePtr + 19*sizeof(unsigned short));
                if (temp_dist != 65535){
                    ImageInfo.Distance = (float)temp_dist/100;
                }else{
                    ImageInfo.Distance = -1 /* infinity */;
                }
            }
        }
    }
}

//--------------------------------------------------------------------------
// Process maker note - to the limited extent that its supported.
//--------------------------------------------------------------------------
void ProcessMakerNote(unsigned char * ValuePtr, int ByteCount, 
        unsigned char * OffsetBase, unsigned ExifLength)
{
    if (strstr(ImageInfo.CameraMake, "Canon")){
        ProcessCanonMakerNoteDir(ValuePtr, OffsetBase, ExifLength);
    }
}
