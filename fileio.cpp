#include "fileio.h"

QString validateROM(QByteArray& array)
{
    // Check for files that are too small to be valid
    if(array.size() < 0x100000)
    {
        return QString("File too small to be a valid ROM image of Kirby's Dream Course.");
    }

    // Generate the header size (0 or 0x200)
    int h = array.size() % 0x100000;

    // If not a multiple of 0x100000 + 0 or 0x200, invalid
    if(h != 0 && h != 0x200)
    {
        return QString("File size does not reflect a valid ROM image.");
    }

    // Check for files that are too large to be valid
    if(array.size() - h > 0x400000)
    {
        return QString("File too large to be a valid ROM image of Kirby's Dream Course.");
    }

    return QString();
}
