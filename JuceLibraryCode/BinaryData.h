/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   clap0_5x_png;
    const int            clap0_5x_pngSize = 1629;

    extern const char*   clave0_5x_png;
    const int            clave0_5x_pngSize = 1236;

    extern const char*   closedHat0_5x_png;
    const int            closedHat0_5x_pngSize = 1072;

    extern const char*   kick10_5x_png;
    const int            kick10_5x_pngSize = 1452;

    extern const char*   kick20_5x_png;
    const int            kick20_5x_pngSize = 1453;

    extern const char*   openHat0_5x_png;
    const int            openHat0_5x_pngSize = 1195;

    extern const char*   snare0_5x_png;
    const int            snare0_5x_pngSize = 1717;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 7;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
