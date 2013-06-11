#ifndef BMP_H__
#define BMP_H__

// Borrowed from: http://stackoverflow.com/questions/11129138/reading-writing-bmp-files-in-c
// and http://www.cplusplus.com/forum/general/6500/
// http://en.wikipedia.org/wiki/BMP_file_format

// http://stackoverflow.com/questions/232785/use-of-pragma-in-c
// This will ensure that a structure is packed tightly (i.e. no padding between members)


// And then to get from BMP to animated gif using image magick
// http://apple.stackexchange.com/a/30564

#pragma pack(push, 1)
typedef struct {
    // char         type[2];       // magic, either 'B' or 'M'
    short type;
    unsigned int fileSize;
    short        reserved1;
    short        reserved2;
    unsigned int dataOffset;    // Offset before the data
} BMPFileHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char alpha;
} ColoredPixel;
#pragma pack(pop)

#pragma pack(push, 1)
// Maps to BITMAPINFOHEADER 
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
#pragma pack(pop)

// Save a game state to file...
int saveGameStateToFile(int** gameTiles, int numberOfRows, int numberOfColumns, char* fileName);

#endif /* end of include guard: BMP_H__ */
