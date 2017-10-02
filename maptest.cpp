#include <cstdio>
#include <cstdlib>

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
    { 127, 0, 127 },  // tileDown
    { 0, 127, 127 },  // tileUp
    { 127, 127, 255 },  // tileWindow
    { 127, 127, 127 },  // tileFakeWall
    { 255, 0, 255 },    // tileUnknown
};

int main() {
    srand(time(0));
    char filenamebuf[128];

    int sizes[] = {
        63, 0
    };
    for (int i = 0; sizes[i] != 0; ++i) {
        sprintf(filenamebuf, "out%03d.png", i);
        doMap(filenamebuf, sizes[i], sizes[i]);
    }

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

