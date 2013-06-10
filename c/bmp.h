#ifndef BMP_H__
#define BMP_H__

// Borrowed from: http://stackoverflow.com/questions/11129138/reading-writing-bmp-files-in-c
// and http://www.cplusplus.com/forum/general/6500/


typedef struct {
    // char         type[2];       // magic, either 'B' or 'M'
    short        type;
    unsigned int fileSize;
    short        reserved1;
    short        reserved2;
    unsigned int dataOffset;    // Offset before the data
} BMPFileHeader;

typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char alpha;
} ColoredPixel;

typedef struct {
    BMPFileHeader fileHeader;
    unsigned int  headerSize;
    int           width;
    int           height;
    short         planes;
    short         bitsPerPixel;  // 24
    unsigned int  compression;   
    unsigned int  bitmapSize;
    int           horizontalResolution;
    int           verticalResolution;
    unsigned int  numberOfColors;
    unsigned int  importantColors;   // no idea?
} BitmapHeader;


// Save a game state to file...
int saveGameStateToFile(int** gameTiles, int numberOfRows, int numberOfColumns, char* fileName);

#endif /* end of include guard: BMP_H__ */
