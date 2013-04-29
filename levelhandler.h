#ifndef LEVELHANDLER_H
#define LEVELHANDLER_H

#include <vector>
#include <qbytearray.h>

typedef struct tile_t
{
    char type;
    char height;
    char entity;
    char bumper;
} Tile;

extern std::vector<Tile> tiles;
extern u16 HEADER_01, levelWidth, levelHeight, HEADER_04, cameraHoriz, cameraVert, entityOffsetHoriz, entityOffsetVert, currentLevel;

/**
 * Opens a level.
 * @param levelnum The level to load.
 */
void loadLevel(int levelnum);

#endif // LEVELHANDLER_H
