#include <unordered_set>

enum class Direction {
    North, East, South, West
};

Direction rotate(Direction d);
Direction randomDirection();

class Coord {
public:
    Coord()
    : myX(0), myY(0)
    { }

    Coord(int x, int y)
    : myX(x), myY(y)
    { }

    int x() const {
        return myX;
    }
    int y() const {
        return myY;
    }

    void shift(Direction d, int distance = 1);
    void shift(int dx, int dy);
private:
    int myX, myY;
};

class Tile {
public:
    Tile(const char *name, int glyph, int opaque, int solid)
    : name(name), glyph(glyph), opaque(opaque), solid(solid)
    { }

    const char *name;
    int glyph;
    int opaque;
    int solid;
};

const int tileWall      = 0;
const int tileFloor     = 1;
const int tileDoor      = 2;
const int tileDown      = 3;
const int tileUp        = 4;
const int tileWindow    = 5;
const int tileFakeWall  = 6;

const int tileUnknown   = 7;
const int tileWorkTemp  = 99;

class ActorType {
public:
private:
    int a;
};

class Actor {
public:
    Actor(int type)
    :type(type)
    { }

    void setPos(int x, int y) {
        myX = x;
        myY = y;
    }
    int x() const {
        return myX;
    }
    int y() const {
        return myY;
    }

private:
    int type;
    int myX, myY;
};

class Map {
public:
    static Tile tileTypes[];

    Map(int width, int height)
    : myWidth(width), myHeight(height)
    {
        tiles = new int[width*height];
        dist = new int[width*height];
    }
    ~Map() {
        delete[] tiles;
        delete[] dist;
    }

    int width() const {
        return myWidth;
    }
    int height() const {
        return myHeight;
    }

    int tile(int x, int y) const {
        int c = coord(x,y);
        if (c < 0) {
            return -1;
        }
        return tiles[c];
    }
    void tile(int x, int y, int newTile) {
        int c = coord(x,y);
        if (c < 0) {
            return;
        }
        tiles[c] = newTile;
    }

    void setActor(Actor *actor, int x, int y) {
        actors.insert(actor);
        actor->setPos(x,y);
    }
    Actor* getActor(int x, int y) {
        for (Actor *actor : actors) {
            if (actor->x() == x && actor->y() == y) {
                return actor;
            }
        }
        return nullptr;
    }
    bool tryMoveActor(Actor *who, Direction d) {
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

    int coverage() const {
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

    void clearDist() {
        for (int y = 0; y < myHeight; ++y) {
            for (int x = 0; x < myWidth; ++x) {
                dist[coord(x,y)] = 2000000;
            }
        }
    }
    int getDist(int x, int y) const {
        return dist[coord(x,y)];
    }

    void calcDist(int startx, int starty) {
        clearDist();
        calcDist_helper(startx, starty, 0);
    }
    void calcDist_helper(int x, int y, int curDist) {
        if (dist[coord(x,y)] < curDist) {
            return;
        }
        dist[coord(x,y)] = curDist;
        Direction d = Direction::North;
        do {
            Coord here(x,y);
            here.shift(d,1);
            if (tile(here.x(),here.y()) != 0 && dist[coord(here.x(),here.y())] > curDist) {
                calcDist_helper(here.x(), here.y(), curDist+1);
            }
            d = rotate(d);
        } while (d != Direction::North);
    }

    void floodfill(int startx, int starty) {
        clearDist();
        floodfill_helper(startx, starty);
    }
    void floodfill_helper(int x, int y) {
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

private:
    int coord(int x, int y) const {
        if (x < 0 || y < 0 || x >= myWidth || y >= myHeight) {
            return -1;
        }
        return x + y * myWidth;
    }

    std::unordered_set<Actor*> actors;
    int myWidth, myHeight;
    int *tiles;
    int *dist;
};
