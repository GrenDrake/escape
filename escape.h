#include <unordered_set>

enum class Direction {
    Here, North, East, South, West
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
    const char *name;
    int glyph;
    int baseAttack;
    int baseMagic;
    int baseHealth;
    int baseAC;
    int unarmedMin;
    int unarmedMax;
    int aiIdent;
};

class Actor {
public:
    Actor(int type);

    int getType() const {
        return type;
    }
    const ActorType* getData() const {
        return data;
    }

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

    int getCurHealth() const {
        return curHealth;
    }

private:
    const ActorType *data;
    int type;
    int myX, myY;
    int curHealth;
};

class Map {
public:
    static ActorType actorTypes[];
    static Tile tileTypes[];

    Map(int width, int height)
    : myWidth(width), myHeight(height), tileCount(width*height)
    {
        tiles = new int[tileCount];
        dist = new int[tileCount];
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

    bool solid(int x, int y) const {
        int tileId = tile(x,y);
        if (tileId < 0 || tileId >= tileUnknown) {
            return false;
        }
        return tileTypes[tileId].solid;
    }

    int tile(int x, int y) const;
    void tile(int x, int y, int newTile);
    void setActor(Actor *actor, int x, int y);
    Actor* getActor(int x, int y);
    bool tryMoveActor(Actor *who, Direction d);
    int coverage(int forTile = 0) const;
    void clearDist();
    void setDist(int x, int y, int newValue);
    int getDist(int x, int y) const;
    void calcDist(int startx, int starty);
    void floodfill(int startx, int starty);
    void endTurn();

private:
    int coord(int x, int y) const;

    std::unordered_set<Actor*> actors;
    const int myWidth, myHeight, tileCount;
    int *tiles;
    int *dist;
};

void makeMap(Map &map, Actor *player);
