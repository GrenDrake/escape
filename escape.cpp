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
                Actor *here = m.getActor(x, y);
                if (here) {
                    if (here == player) {
                        attron(A_REVERSE);
                    }
                    mvaddch(y,x*2,here->getData()->glyph);
                    attroff(A_REVERSE);
                } else {
                    int tileId = m.tile(x,y);
                    Tile &tile = Map::tileTypes[tileId];
                    mvaddch(y,x*2,tile.glyph);
                }
            }
        }

        move(21,0); clrtoeol();
        mvprintw(21,0, "PLAYER  Atk: %-2d  Mag: %-2d  AC: %-2d  HP: %-2d/%2d",
                        player->getData()->baseAttack,
                        player->getData()->baseMagic,
                        player->getData()->baseAC,
                        player->getCurHealth(),
                        player->getData()->baseHealth);

        move(23,0);
        clrtoeol();
        if (cx >= 0 && cy >= 0 && cx < m.width() && cy < m.height()) {
            int tileId = m.tile(player->x(),player->y());
            Tile &tile = Map::tileTypes[tileId];
            mvprintw(23,0,  "%d,%d - %s (%d) - dist:%d",
                            player->x(),    player->y(),
                            tile.name,      tileId,
                            m.getDist(player->x(),player->y()));
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

