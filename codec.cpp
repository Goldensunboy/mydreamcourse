#include "defs.h"
#include "codec.h"

/* The first three bits of each opcode is used to determine what type of decompression
 *     method will be used on the immediately following bytes:
 *
 * #$00 (000LLLLL) Type 1: Direct Copy
 *   For the length, copy that many bytes from the input array to the output array.
 *
 * #$20 (001LLLLL) Type 2: 8-bit RLE
 *   For the length, copy the next byte that many times to the output array.
 *
 * #$40 (010LLLLL) Type 3: 16-bit RLE
 *   For the length, copy the next two bytes (both at once per write sequence) that
 *     many times to the output array.
 *
 * #$60 (011LLLLL) Type 4: Incrementing 8-bit RLE
 *   Copy the next byte to the output array and then increment it before the next write,
 *     for the number of times specified by the length.
 *
 * #$80 (100LLLLL) Type 5: Forward recopy
 *   For the length, use the next 2 bytes as an offset from the start of the
 *     DECOMPRESSED data (beginning at 0x0000), and recopy that many values to the output array.
 *     The pointer is little-endian.
 *
 * #$A0 (101LLLLL) Type 6: Forward recopy (reversed bits)
 *   For the length, use the next 2 bytes as an offset from the start of the
 *     DECOMPRESSED data (beginning at 0x0000), and recopy that many values to the output
 *     array after reversing the order of the bits in each byte.
 *     The pointer is little-endian.
 *
 * #$C0 (110LLLLL) Type 7: Backward recopy
 *   For the length, use the next bytes as an offset from the start of the
 *     DECOMPRESSED data (beginning at 0x0000), and recopy that many values working
 *     backwards from the initial offset, to the output array.
 *     The pointer is little-endian.
 *
 * #$E0 (111PPPAA) Type 8: Long Notation
 *   Set the decompression length beforehand to a number larger than the max possible
 *     0x1F + 1 as with the previous types of compression by using the next byte + 1 as
 *     a length, and the additional write length bits A to count 0x100 writes for each
 *     value of #$01. Then, recast the opcode to one of the the previous 7 types with the
 *     opcode bits P, and utilize however many (1 or 2) bytes after the write length byte
 *     to do so.
 */
QByteArray decompress(int start)
{
    // This is the array the data will be stored in as it is decompressed.
    QByteArray array;

    // This is the current decompression code, and the length for any decompression instruction.
    int code, length, index;
    char val, val2;
    while(static_cast<byte>(file[start]) != 0xFF)
    {
        code = (file[start] >> 5) & 7;
        length = (file[start] & 0x1F) + 1;

        // If long notation, change values to reflect this.
        if(code == 7)
        {
            code = (file[start] >> 2) & 7;
            length = (file[start] & 3) * 0x100 + static_cast<byte>(file[start + 1]) + 1;
            start += 2;
        }
        else
        {
            ++start;
        }

        // Determine the decompression type
        switch(code)
        {
        case 0: // Direct copy
            while(length--)
            {
                array.append(file[start++]);
            }
            break;

        case 1: // 8-bit RLE
            val = file[start++];
            while(length--)
            {
                array.append(val);
            }
            break;

        case 2: // 16-bit RLE
            val = file[start++];
            val2 = file[start++];
            while(length--)
            {
                array.append(val);
                array.append(val2);
            }
            break;

        case 3: // Incrementing 8-bit RLE
            val = file[start++];
            while(length--)
            {
                array.append(val++);
            }
            break;

        case 4: // Forward recopy
            val = file[start++];
            index = static_cast<byte>(file[start++]) + static_cast<byte>(val) * 0x100;
            while(length--)
            {
                array.append(array[index++]);
            }
            break;

        case 5: // Forward recopy (reversed bits)
            val = file[start++];
            index = static_cast<byte>(file[start++]) + static_cast<byte>(val) * 0x100;
            while(length--)
            {
                array.append(reverse(array[index++]));
            }
            break;

        case 6: // Backward recopy
            val = file[start++];
            index = static_cast<byte>(file[start++]) + static_cast<byte>(val) * 0x100;
            while(length--)
            {
                array.append(array[index--]);
            }
            break;

        default: // ERROR: $FC, $FD, or $FE encountered (illegal)
            throw "Attempted to decompress malformed data!";
        }
    }

    return array;
}

char reverse(char c)
{
    char x = 0;
    for(int i = 0; i < 8; ++i)
    {
        x |= ((c >> i) & 1) << (7 - i);
    }
    return x;
}
