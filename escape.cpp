#include <stdio.h>
#include <stdlib.h>

#include <ctime>
#include <deque>
#include <vector>

#include <ncurses.h>


#include "escape.h"

int main() {
    srand(time(0));

    Map m(31,21);
    Actor *player = new Actor(0);
    makeMap(m, player);


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
                    mvaddch(y,x*2,here->getType() == 0 ? '@' : 'g');
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

