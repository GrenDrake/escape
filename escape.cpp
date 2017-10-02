#include <stdio.h>
#include <stdlib.h>

#include <ctime>
#include <deque>
#include <vector>

#include <ncurses.h>


#include "escape.h"

void generate_randomMap(Map &map);

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



int main() {
    srand(time(0));

    Map m(31,21);
    Actor *player = new Actor(0);

    do {
        generate_randomMap(m);

        int startX, startY;
        do {
            startX = rand() % m.width();
            startY = rand() % m.height();
        } while (m.tile(startX,startY) != tileFloor);
        m.tile(startX,startY,4);

        m.calcDist(startX,startY);
        int maxX = -1, maxY = -1, maxDist = 0;
        for (int y = 0; y < m.height(); ++y) {
            for (int x = 0; x < m.width(); ++x) {
                int distHere = m.getDist(x,y);
                if (distHere > maxDist && distHere != 2000000) {
                    maxDist = distHere;
                    maxX = x;
                    maxY = y;
                }
            }
        }
        m.tile(maxX,maxY,tileUp);
        m.setActor(player, startX, startY);
    } while (m.coverage() < 35);


    initscr();
    keypad(stdscr,true);

    int cx=0, cy=0;
    while (1) {


        for (int y = 0; y < m.height(); ++y) {
            for (int x = 0; x < m.width(); ++x) {
                // if (cx == x && cy == y) {
                //     attron(A_REVERSE);
                // } else {
                //     attroff(A_REVERSE);
                // }
                Actor *here = m.getActor(x, y);
                if (here) {
                    mvaddch(y,x*2,'@');
                } else {
                    int tileId = m.tile(x,y);
                    Tile &tile = Map::tileTypes[tileId];
                    mvaddch(y,x*2,tile.glyph);
                }
            }
        }
        move(23,0);
        clrtoeol();
        if (cx >= 0 && cy >= 0 && cx < m.width() && cy < m.height()) {
            int tileId = m.tile(cx,cy);
            Tile &tile = Map::tileTypes[tileId];
            mvprintw(23,0,"%d,%d - %s (%d) - %d      %d", cx, cy, tile.name, tileId, m.getDist(cx,cy), m.coverage());
        }

        int key = getch();
        if (key == 'Q') {
            break;
        } else if (key == KEY_LEFT) {
            m.tryMoveActor(player, Direction::West);
        } else if (key == KEY_RIGHT) {
            m.tryMoveActor(player, Direction::East);
        } else if (key == KEY_UP) {
            m.tryMoveActor(player, Direction::North);
        } else if (key == KEY_DOWN) {
            m.tryMoveActor(player, Direction::South);
        }

    }

    endwin();
    return 0;
}

