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

    int tile(int x, int y) const;
    void tile(int x, int y, int newTile);
    void setActor(Actor *actor, int x, int y);
    Actor* getActor(int x, int y);
    bool tryMoveActor(Actor *who, Direction d);
    int coverage(int forTile = 0) const;
    void clearDist();
    int getDist(int x, int y) const;
    void calcDist(int startx, int starty);
    void floodfill(int startx, int starty);

private:
    void calcDist_helper(int x, int y, int curDist);
    void floodfill_helper(int x, int y);
    int coord(int x, int y) const;

    std::unordered_set<Actor*> actors;
    int myWidth, myHeight;
    int *tiles;
    int *dist;
};

void makeMap(Map &map, Actor *player);
