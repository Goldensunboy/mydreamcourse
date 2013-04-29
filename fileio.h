#ifndef FILEIO_H
#define FILEIO_H

#include "defs.h"
#include "qstring.h"
#include "qbytearray.h"

/**
 * Validate the ROM based on the file size.
 * @param array The byte array to validate.
 * @return A default QString if successful; otherwise, a QString with the error message.
 */
QString validateROM(QByteArray& array);

#endif // FILEIO_H
