#include "defs.h"
#include "levelhandler.h"
#include "codec.h"
#include "mdcwindow.h"

Tile nullTile = {0, 0, 0, 0};

// Byte arrays that define different aspects of the level
QByteArray levelHeader,
           tileMap,
           entities,
           heightMap,
           bumperMap,
           GFX_preDisp,
           GFX_postDisp,
           GFX_tileOffsets,
           GFX_geometryTiles,
           GFX_entityTiles,
           DATA_TABLE_11; // Unknown purpose

// Constant locations of pointer tables for the data
const int TABLE_01_OFFSET = 0x42770, // Level header
          TABLE_02_OFFSET = 0x42A70, // Tile types
          TABLE_03_OFFSET = 0x42D70, // Entity types
          TABLE_04_OFFSET = 0x43070, // Heightmap
          TABLE_05_OFFSET = 0x43370, // Bumper types
          TABLE_06_OFFSET = 0x43670, // GFX row displacement data
          TABLE_07_OFFSET = 0x43970, // GFX row end data
          TABLE_08_OFFSET = 0x43C70, // GFX tile data offsets for rows
          TABLE_09_OFFSET = 0x43F70, // GFX tile data (geometry tiles)
          TABLE_10_OFFSET = 0x44270, // GFX tile data (entities)
          TABLE_11_OFFSET = 0x44570; // Unknown

// Variables used to define the level
std::vector<Tile> tiles;
u16 HEADER_01,         // Unknown
    levelWidth,        // SE direction
    levelHeight,       // NE direction
    HEADER_04,         // Unknown
    cameraHoriz,       // Camera horizontal offset
    cameraVert,        // Camera vertical offset
    entityOffsetHoriz, // This should always be zero!
    entityOffsetVert,  // Vertical offset for entity graphics
    currentLevel;      // Currently loaded level

/* Level layout:
 *
 *       X
 *      / \
 *     X 2 X
 *    / \ / \
 *   X 0 X 3 X
 *    \ / \ /
 *     X 1 X
 *      \ /
 *       X
 */

void loadLevel(int levelnum)
{
    // Load the level header (uncompressed)
    levelHeader = file.mid(ADDRESS(TABLE_01_OFFSET + levelnum*3), 0x20);

    // Load tile geometry data
    tileMap = decompress(ADDRESS(TABLE_02_OFFSET + levelnum*3));

    // Load entity data
    entities = decompress(ADDRESS(TABLE_03_OFFSET + levelnum*3));

    // Load heightmap data
    heightMap = decompress(ADDRESS(TABLE_04_OFFSET + levelnum*3));

    // Load bumpermap data
    bumperMap = decompress(ADDRESS(TABLE_05_OFFSET + levelnum*3));

    // Load pre-GFX FG offset data
    GFX_preDisp = decompress(ADDRESS(TABLE_06_OFFSET + levelnum*3));

    // Load post-GFX FG offset data
    GFX_postDisp = decompress(ADDRESS(TABLE_07_OFFSET + levelnum*3));

    // Load tile data offsets for rows
    GFX_tileOffsets = decompress(ADDRESS(TABLE_08_OFFSET + levelnum*3));

    // Load FG GFX data
    GFX_geometryTiles = decompress(ADDRESS(TABLE_09_OFFSET + levelnum*3));

    // Load entity GFX data
    GFX_entityTiles = decompress(ADDRESS(TABLE_10_OFFSET + levelnum*3));

    // Load unused data table
    DATA_TABLE_11 = decompress(ADDRESS(TABLE_11_OFFSET + levelnum*3));

    // Load level properties from the header data
    HEADER_01 = (byte)levelHeader[1] * 0x100 + (byte)levelHeader[0];
    levelWidth = (byte)levelHeader[3] * 0x100 + (byte)levelHeader[2];
    levelHeight = (byte)levelHeader[5] * 0x100 + (byte)levelHeader[4];
    HEADER_04 = (byte)levelHeader[7] * 0x100 + (byte)levelHeader[6];
    cameraHoriz = (byte)levelHeader[9] * 0x100 + (byte)levelHeader[8];
    cameraVert = (byte)levelHeader[11] * 0x100 + (byte)levelHeader[10];
    entityOffsetHoriz = (byte)levelHeader[13] * 0x100 + (byte)levelHeader[12];
    entityOffsetVert = (byte)levelHeader[15] * 0x100 + (byte)levelHeader[14];

    // Create an array of Tiles from the level data
    tiles = std::vector<Tile>();
    for(unsigned int i = 0; i < levelWidth*levelHeight; ++i)
    {
        Tile t = {tileMap[i], heightMap[i], entities[i], bumperMap[i]};
        tiles.push_back(t);
    }
}
