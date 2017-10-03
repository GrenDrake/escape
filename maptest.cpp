#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <png.h>

#include "escape.h"

int doMap(const char *filename, int width, int height);

class TileColor {
public:
    int r, g, b;
};
TileColor tileColours[] = {
    { 0, 0, 0 },        // tileWall
    { 255, 255, 255 },  // tileFloor
    { 160, 82, 45 },    // tileDoor
    { 127, 0, 127 },    // tileDown
    { 0, 127, 127 },    // tileUp
    { 127, 127, 255 },  // tileWindow
    { 127, 127, 127 },  // tileFakeWall
    { 255, 0, 255 },    // tileUnknown
};

int main() {
    char filenamebuf[128];

    long seed = time(0);
    // long seed = 1507020633;
    const int mapCount = 16;
    const int mapWidth = 63;
    const int mapHeight = 63;

    clock_t bigStart = clock();
    for (int i = 0; i < mapCount; ++i) {
        srand(seed);
        sprintf(filenamebuf, "maps/out%03d.png", i+1);
        printf("Creating map %d with seed %ld...", i+1, seed);
        clock_t start = clock();
        doMap(filenamebuf, mapWidth, mapHeight);
        clock_t diff = clock() - start;
        double duration = diff / (double)(CLOCKS_PER_SEC);
        printf("  (took %lf seconds)\n", duration);
        ++seed;
    }
    clock_t fullDiff = clock() - bigStart;
    double duration = fullDiff / (double)(CLOCKS_PER_SEC);
    printf("\nFull Process took %lf seconds.\n", duration);

}


void writeDot(png_bytep row, int x, int r, int g, int b) {
    row[x*3] = r;
    row[x*3+1] = g;
    row[x*3+2] = b;
}


int doMap(const char *filename, int width, int height) {
    Map m(width,height);
    Actor *player = new Actor(0);
    makeMap(m, player);

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "could not open file\n");
        return 1;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fprintf(stderr, "could not allocate write struct\n");
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!png_ptr) {
        fprintf(stderr, "could not allocate info struct\n");
        return 1;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, m.width()*4, m.height()*4, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);


    png_write_info(png_ptr, info_ptr);
    png_bytep row = (png_bytep)malloc(3 * m.width() * 4 * sizeof(png_byte));


    for (int y = 0; y < m.height(); ++y) {
        for (int x = 0; x < m.width(); ++x) {
            int tileId = m.tile(x,y);
            if (tileId > tileUnknown) {
                tileId = tileUnknown;
            }

            for (int i = 0; i < 4; ++i) {
                writeDot(row, x*4+i, tileColours[tileId].r, tileColours[tileId].g, tileColours[tileId].b);
            }
        }
        png_write_row(png_ptr, row);
        png_write_row(png_ptr, row);
        png_write_row(png_ptr, row);
        png_write_row(png_ptr, row);
    }

    png_write_end(png_ptr, nullptr);
    png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    png_destroy_write_struct(&png_ptr, nullptr);
    free(row);
    fclose(fp);

    delete player;
    return 0;
}

