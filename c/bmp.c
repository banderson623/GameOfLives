#include "bmp.h"
#include <stdio.h>

void BMPPrintFileHeader(BMPFileHeader* header){
    printf("BMPFileHeader\n--------------------------\n");
    printf("Size of BMPFileHeader: %ld (should be 14)\n",sizeof(BMPFileHeader));
    printf("Type: %d (size: %ld)\n",header->type, sizeof(header->type));
    printf("fileSize: %d (size: %ld)\n",header->fileSize, sizeof(header->fileSize));
    printf("reserved1: %d (size: %ld)\n",header->reserved1, sizeof(header->reserved1));
    printf("reserved2: %d (size: %ld)\n",header->reserved2, sizeof(header->reserved2));
    printf("dataOffset: %d (size: %ld)\n",header->dataOffset, sizeof(header->dataOffset));
    printf("--------------------\n");
}


int saveGameStateToFile(int** gameTiles, int numberOfRows, int numberOfColumns, char* fileName) {
    const int colorDepth = 24;
    int result = 0;
 
    // typedef struct {
    //     char         type[2];       // magic, either 'B' or 'M'
    //     unsigned int fileSize;
    //     short        reserved1;
    //     short        reserved2;
    //     unsigned int dataOffset;    // Offset before the data
    // } BMPFileHeader;
    
    BMPFileHeader fileHeader;
    fileHeader.type = 19778;
    fileHeader.reserved1 = fileHeader.reserved2 = 0;
    fileHeader.fileSize = colorDepth * numberOfRows * numberOfColumns;
    fileHeader.dataOffset = sizeof(BitmapHeader); //= 54; // 54 this could change...

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
    header.headerSize = sizeof(BitmapHeader) - sizeof(BMPFileHeader); // size of self...
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
    
    // printf("Bitmap File Header: %d\n", sizeof(BMPFileHeader));
    // BMPPrintFileHeader(&fileHeader);
    // printf("Header size: %ld\n", sizeof(BitmapHeader));

    FILE* outputHandle = fopen(fileName, "wb");
    if(outputHandle != NULL){
        // we have a file Handle
        // Write the header...
        size_t writeSize = fwrite(&header, sizeof(char), sizeof(BitmapHeader), outputHandle);
            if(writeSize > 1){
                // Good to go!
                ColoredPixel lifePixel; 
                lifePixel.blue = 255;
                lifePixel.red = 155;
                lifePixel.green = 155;
                lifePixel.alpha = 0;
                
                ColoredPixel deadPixel;
                deadPixel.blue = deadPixel.red = deadPixel.green = deadPixel.alpha = 0;
                
                for(int row = numberOfRows - 1; row >= 0; row--){
                    for(int column = 0; column < numberOfColumns; column++){
                        ColoredPixel pixelToWrite;
                        
                        if(gameTiles[row][column] == 0){
                            pixelToWrite = deadPixel;
                        } else {
                            pixelToWrite = lifePixel;
                        }
                                            // is this correct?
                        writeSize = fwrite((char*)&pixelToWrite, 1, sizeof(ColoredPixel),outputHandle);
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