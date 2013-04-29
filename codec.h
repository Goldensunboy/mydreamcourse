#ifndef CODEC_H
#define CODEC_H

#include "qbytearray.h"

/**
 * Decompress data starting at a position, and return it as a byte array.
 * @param start The beginning of the data to decompress. The end is indeterminate.
 * @return A byte array representing the decompressed data from the currently opened ROM.
 */
QByteArray decompress(int start);

/**
 * Reverse the ordering of the bits in a char.
 * @param c The character whose bits will be reversed.
 * @return The reversed char. Example: 10110010 -> 01001101
 */
char reverse(char c);

#endif // CODEC_H
