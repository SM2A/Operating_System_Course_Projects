#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct bit_map{
    std::vector<std::vector<unsigned char >> r;
    std::vector<std::vector<unsigned char >> g;
    std::vector<std::vector<unsigned char >> b;
} file;

file image;

typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

int rows;
int cols;

bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize) {
    std::ifstream file(fileName);

    if (file) {
        file.seekg(0, std::ios::end);
        std::streampos length = file.tellg();
        file.seekg(0, std::ios::beg);

        buffer = new char[length];
        file.read(&buffer[0], length);

        PBITMAPFILEHEADER file_header;
        PBITMAPINFOHEADER info_header;

        file_header = (PBITMAPFILEHEADER) (&buffer[0]);
        info_header = (PBITMAPINFOHEADER) (&buffer[0] + sizeof(BITMAPFILEHEADER));
        rows = info_header->biHeight;
        cols = info_header->biWidth;
        bufferSize = file_header->bfSize;
        return 1;
    } else {
        cout << "File " << fileName << " doesn't exist!" << endl;
        return 0;
    }
}

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer) {
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        image.r.emplace_back(cols,0);
        image.g.emplace_back(cols,0);
        image.b.emplace_back(cols,0);
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 0:
                        image.r[i][j] = fileReadBuffer[end - count];
                        break;
                    case 1:
                        image.g[i][j] = fileReadBuffer[end - count];
                        break;
                    case 2:
                        image.b[i][j] = fileReadBuffer[end - count];
                        break;
                }
            }
        }
    }
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize) {
    std::ofstream write(nameOfFileToCreate);
    if (!write) {
        cout << "Failed to write " << nameOfFileToCreate << endl;
        return;
    }
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 0:
                        fileBuffer[bufferSize - count] = image.r[i][j];
                        break;
                    case 1:
                        fileBuffer[bufferSize - count] = image.g[i][j];
                        break;
                    case 2:
                        fileBuffer[bufferSize - count] = image.b[i][j];
                        break;
                }
            }
    }
    write.write(fileBuffer, bufferSize);
}

int main(int argc, char *argv[]) {
    char *fileBuffer;
    int bufferSize;
    char *fileName = argv[1];
    if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize)) {
        cout << "File read error" << endl;
        return 1;
    }

    getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer);
    // apply filters
    writeOutBmp24(fileBuffer,"test.bmp",bufferSize);

    return 0;
}