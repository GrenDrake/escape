

#include "escape.h"

Direction rotate(Direction d) {
    switch(d) {
        case Direction::North:  return Direction::East;
        case Direction::East:   return Direction::South;
        case Direction::South:  return Direction::West;
        case Direction::West:   return Direction::North;
    }
    return Direction::North;
}

Direction randomDirection() {
    switch(rand()%4) {
        case 0: return Direction::East;
        case 1: return Direction::South;
        case 2: return Direction::West;
        case 3: return Direction::North;
    }
    return Direction::North;
}

void Coord::shift(Direction d, int distance) {
    switch(d) {
        case Direction::East:   myX += distance;    break;
        case Direction::West:   myX -= distance;    break;
        case Direction::North:  myY -= distance;    break;
        case Direction::South:  myY += distance;    break;
    }
}
void Coord::shift(int dx, int dy) {
    myX += dx;
    myY += dy;
}

Tile Map::tileTypes[] = {
    //  name            glyph   opaque  solid
    {   "wall",         '#',    true,   true },
    {   "floor",        '.',    false,  false },
    {   "door",         '+',    true,   false },
    {   "stair down",   '>',    false,  false },
    {   "stair up",     '<',    false,  false },
    {   "window",       '=',    false,  true },
    {   "fake wall",    '%',    true,   false },

    {   "unknown",      '?',    true,   true }
};

int Map::tile(int x, int y) const {
    int c = coord(x,y);
    if (c < 0) {
        return -1;
    }
    return tiles[c];
}
void Map::tile(int x, int y, int newTile) {
    int c = coord(x,y);
    if (c < 0) {
        return;
    }
    tiles[c] = newTile;
}

void Map::setActor(Actor *actor, int x, int y) {
    actors.insert(actor);
    actor->setPos(x,y);
}
Actor* Map::getActor(int x, int y) {
    for (Actor *actor : actors) {
        if (actor->x() == x && actor->y() == y) {
            return actor;
        }
    }
    return nullptr;
}
bool Map::tryMoveActor(Actor *who, Direction d) {
    if (actors.count(who) == 0) {
        return false;
    }

    Coord c(who->x(), who->y());
    c.shift(d);

    Tile &tiledef = tileTypes[tile(c.x(), c.y())];
    if (tiledef.solid) {
        return false;
    }
    if (getActor(c.x(), c.y())) {
        return false;
    }

    setActor(who, c.x(), c.y());
    return true;
}

int Map::coverage(int forTile) const {
    int count = 0;
    for (int y = 0; y < myHeight; ++y) {
        for (int x = 0; x < myWidth; ++x) {
            if (tile(x,y) == 0) {
                ++count;
            }
        }
    }
    return 100 - (100 * count / (myHeight * myWidth));
}

void Map::clearDist() {
    for (int y = 0; y < myHeight; ++y) {
        for (int x = 0; x < myWidth; ++x) {
            dist[coord(x,y)] = 2000000;
        }
    }
}

void Map::setDist(int x, int y, int newValue) {
    dist[coord(x,y)] = newValue;
}
int Map::getDist(int x, int y) const {
    return dist[coord(x,y)];
}

void Map::calcDist(int startx, int starty) {
    for (int i = 0; i < tileCount; ++i) {
        dist[i] = -1;
}

    std::deque<Coord> tilelist;
    tilelist.push_back(Coord(startx,starty));
    setDist(startx, starty, 0);
    while (!tilelist.empty()) {
        Coord here = tilelist.front();
        tilelist.pop_front();

        int distHere = getDist(here.x(), here.y());
        int newDist = distHere + 1;
    Direction d = Direction::North;
    do {
            Coord there = here;
            there.shift(d, 1);
            if (!solid(there.x(),there.y())) {
                int distThere = getDist(there.x(), there.y());
                if (distThere == -1 || distThere > newDist) {
                    setDist(there.x(), there.y(), newDist);
                    tilelist.push_back(there);
                }
        }
        d = rotate(d);
    } while (d != Direction::North);
}
}

void Map::floodfill(int startx, int starty) {
    clearDist();
    floodfill_helper(startx, starty);
}

void Map::floodfill_helper(int x, int y) {
    dist[coord(x,y)] = 0;
    Direction d = Direction::North;
    do {
        Coord here(x,y);
        here.shift(d,1);
        if (tile(here.x(),here.y()) != 0 && dist[coord(here.x(),here.y())] == 2000000) {
            floodfill_helper(here.x(), here.y());
        }
        d = rotate(d);
    } while (d != Direction::North);
}

int Map::coord(int x, int y) const {
    if (x < 0 || y < 0 || x >= myWidth || y >= myHeight) {
        return -1;
    }
    return x + y * myWidth;
}
