#include <cstdlib>

#include <deque>
#include <vector>

#include "escape.h"


class Room {
public:
    Room(int x, int y, int w, int h)
    : x(x), y(y), w(w), h(h)
    { }
    int x, y;
    int w, h;
};


void generate_clear(Map &map) {
    for (int y = 0; y < map.width(); ++y) {
        for (int x = 0; x < map.height(); ++x) {
            map.tile(x,y,tileWall);
        }
    }
}


void generate_rooms(Map &map, std::vector<Room> &rooms) {
    for (int i = 0; i < 20; ++i) {
        int startX = 2 * (rand() % (map.width() / 2));
        int startY = 2 * (rand() % (map.height() / 2));
        int w = 2 * (2 + rand() % 2);
        int h = 2 * (2 + rand() % 2);

        int badRoom = 0;
        for (int x = startX; x <= startX + w; ++x) {
            for (int y = startY; y <= startY + h; ++y) {
                if (map.tile(x,y) != tileWall) {
                    badRoom = 1;
                }
            }
        }
        if (badRoom) {
            continue;
        }

        rooms.push_back(Room(startX,startY,w,h));

        for (int x = startX; x <= startX + w; ++x) {
            for (int y = startY; y <= startY + h; ++y) {
                if (x == startX || y == startY || x == startX+w || y == startY+h) {
                    map.tile(x,y,tileWall);
                } else {
                    map.tile(x,y,tileWorkTemp);
                }
            }
        }
    }
}

bool validPositionForDoor(Map &map, int x, int y) {
    Coord sideN(x,y);
    Coord sideS(x,y);
    Coord sideE(x,y);
    Coord sideW(x,y);
    sideN.shift(Direction::North,1);
    sideS.shift(Direction::South,1);
    sideE.shift(Direction::East,1);
    sideW.shift(Direction::West,1);

    if (map.tile(sideN.x(), sideN.y()) == tileWall && map.tile(sideS.x(), sideS.y()) == tileWall
            && map.tile(sideW.x(), sideW.y()) == tileFloor && map.tile(sideE.x(), sideE.y()) == tileFloor) {
        return true;
    }
    if (map.tile(sideN.x(), sideN.y()) == tileWall && map.tile(sideS.x(), sideS.y()) == tileWall
            && map.tile(sideW.x(), sideW.y()) == tileFloor && map.tile(sideE.x(), sideE.y()) == tileFloor) {
        return true;
    }
    return false;
}

void generate_randomMap(Map &map) {
    generate_clear(map);

    // add some random rooms to the map
    std::vector<Room> rooms;
    generate_rooms(map, rooms);

    // picking a starting point
    int startX, startY;
    do {
        startX = 1 + 2 * (rand() % (map.width() / 2));
        startY = 1 + 2 * (rand() % (map.height() / 2));
    } while (map.tile(startX,startY) != 0);
    map.tile(startX, startY, tileFloor);

    // build a maze outside of the rooms
    std::deque<Coord> points;
    points.push_back(Coord(startX,startY));
    while (!points.empty()) {
        Coord here = points.back();

        Direction startDir = randomDirection();
        Direction dir = startDir;
        int found_dir = 0;
        do {
            Coord there = here;
            there.shift(dir, 2);
            if (map.tile(there.x(), there.y()) == tileWall) {
                map.tile(there.x(), there.y(), tileFloor);
                there = here;
                there.shift(dir, 1);
                map.tile(there.x(), there.y(), tileFloor);
                there.shift(dir, 1);
                points.push_back(there);
                found_dir = 1;
                break;
            }
            dir = rotate(dir);
        } while (dir != startDir);

        if (!found_dir) {
            points.pop_back();
        }
    }

    // find a spot on the map located in one of the corridors
    do {
        startX = 1 + 2 * (rand() % (map.width() / 2));
        startY = 1 + 2 * (rand() % (map.height() / 2));
    } while (map.tile(startX,startY) != tileFloor);

    // add doors to rooms so the player can actually get into them
    for (Room room : rooms) {
        for (int y = room.y+1; y < room.y+room.h; ++y) {
            for (int x = room.x+1; x < room.x+room.w; ++x) {
                map.tile(x,y,tileFloor);
            }
        }

        for (int i = 0; i < 2; ++i) {
            Direction d = randomDirection();
            int x = 1 + room.x + rand() % (room.w - 2);
            int y = 1 + room.y + rand() % (room.h - 2);

            Coord here(x,y);
            while (map.tile(here.x(),here.y()) == tileFloor) {
                here.shift(d, 1);
            }
            if (validPositionForDoor(map, here.x(), here.y())) {
                map.tile(here.x(),here.y(),tileDoor);
            }
        }
    }

    // remove disconnected rooms
    map.floodfill(startX,startY);
    for (Room room : rooms) {
        if (map.getDist(room.x+1,room.y+1) == 0) {
            continue;
        }

        for (int y = room.y; y < room.y+room.h; ++y) {
            for (int x = room.x; x < room.x+room.w; ++x) {
                map.tile(x,y,tileWall);
            }
        }
    }

    // add some random doors and windows
    for (int i = 0; i < 75; ++i) {
        int x = rand() % map.width();
        int y = rand() % map.height();
        if (validPositionForDoor(map, x, y)) {
            if (map.tile(x,y) == tileWall && rand() % 2 == 0) {
                map.tile(x,y,tileWindow);
            } else {
                map.tile(x,y, rand()%3==0 ? tileFakeWall : tileDoor);
            }
        }
    }
}

