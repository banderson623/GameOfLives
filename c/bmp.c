#include "bmp.h"
#include <stdio.h>

int saveGameStateToFile(int** gameTiles, int numberOfRows, int numberOfColumns, char* fileName) {
    int colorDepth = 24;
    int result = 0;
 
    // typedef struct {
    //     char         type[2];       // magic, either 'B' or 'M'
    //     unsigned int fileSize;
    //     short        reserved1;
    //     short        reserved2;
    //     unsigned int dataOffset;    // Offset before the data
    // } BMPFileHeader;
    
    BMPFileHeader fileHeader;
    // fileHeader.type =  'B';
    fileHeader.type = 19778;
    fileHeader.fileSize = colorDepth * numberOfRows * numberOfColumns;
    fileHeader.dataOffset = 54; // this could change...

    // typedef struct {
    //     BMPFileHeader fileHeader;
    //     unsigned int  headerSize;
    //     int           width;
    //     int           height;
    //     short         planes;
    //     short         bitsPerPixel;  // 24
    //     unsigned int  compression;   
    //     unsigned int  bitmapSize;
    //     int           horizontalResolution;
    //     int           verticalResolution;
    //     unsigned int  numberOfColors;
    //     unsigned int  importantColors;   // no idea?
    // } BitmapHeader;
    BitmapHeader header;
    header.fileHeader = fileHeader;
    header.headerSize = sizeof(BitmapHeader); // size of self...
    header.width = numberOfColumns;
    header.height = numberOfRows;
    header.planes = 1;
    header.bitsPerPixel = colorDepth;
    header.compression = 0,
    header.bitmapSize = fileHeader.fileSize; //depth * height * width
    header.horizontalResolution = 1;
    header.verticalResolution = 1;
    header.numberOfColors = 0;
    header.importantColors = 0;

    FILE* outputHandle = fopen(fileName, "wb");
    if(outputHandle != NULL){
        // we have a file Handle
        // Write the header...
        size_t writeSize = fwrite(&header, sizeof(char), sizeof(BitmapHeader), outputHandle);
            if(writeSize > 1){
                // Good to go!
                ColoredPixel pixel; 
                pixel.blue = 255;
                pixel.red = 0;
                pixel.green = 0;
                pixel.alpha = 0;
                for(int row = 0; row < numberOfRows; row++){
                    for(int column = 0; column < numberOfColumns; column++){
                        writeSize = fwrite((char*)&pixel, 1, sizeof(ColoredPixel),outputHandle);
                    }
                }
            }
            fclose(outputHandle);
    }
    
    // 
    // 
    // Header header = {19778, colorDepth*numberOfRows*numberOfColumns , 0, 0, 54};
    // Info   info   = {sizeof(Info), w, h, 1, sizeof(Color)*8, 0, ( sizeof(Color) *(w*h) ), 1, 1, 0, 0};
    // Color  color  = {0,255,90,0};
    return result;
}