#include <pthread.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;

#pragma pack(1)

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct bit_map {
    vector<vector<unsigned char >> r;
    vector<vector<unsigned char >> g;
    vector<vector<unsigned char >> b;
} file;

file image, img_copy;

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

int rows, cols;

bool fillAndAllocate(char *&buffer, const char *fileName, int &bufferSize) {
    ifstream file(fileName);

    if (file) {
        file.seekg(0, ios::end);
        streampos length = file.tellg();
        file.seekg(0, ios::beg);

        buffer = new char[length];
        file.read(&buffer[0], length);

        PBITMAPFILEHEADER file_header;
        PBITMAPINFOHEADER info_header;

        file_header = (PBITMAPFILEHEADER) (&buffer[0]);
        info_header = (PBITMAPINFOHEADER) (&buffer[0] + sizeof(BITMAPFILEHEADER));
        rows = info_header->biHeight;
        cols = info_header->biWidth;
        bufferSize = file_header->bfSize;
        return true;
    } else {
        cout << "File " << fileName << " doesn't exist!" << endl;
        return false;
    }
}

void getPixlesFromBMP24(int end, const char *fileReadBuffer) {
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        image.r.emplace_back(cols, 0);
        image.g.emplace_back(cols, 0);
        image.b.emplace_back(cols, 0);

        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 0:
                        image.r[i][j] = fileReadBuffer[end - count++];
                        break;
                    case 1:
                        image.g[i][j] = fileReadBuffer[end - count++];
                        break;
                    case 2:
                        image.b[i][j] = fileReadBuffer[end - count++];
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize) {
    ofstream write(nameOfFileToCreate);
    if (!write) {
        cout << "Failed to write " << nameOfFileToCreate << endl;
        return;
    }
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                    case 0:
                        fileBuffer[bufferSize - count] = image.r[i][j];
                        count++;
                        break;
                    case 1:
                        fileBuffer[bufferSize - count] = image.g[i][j];
                        count++;
                        break;
                    case 2:
                        fileBuffer[bufferSize - count] = image.b[i][j];
                        count++;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    write.write(fileBuffer, bufferSize);
}

unsigned char pixel_avg(char channel, int i, int j) {
    int result = 0;
    for (int k = i - 1; k <= i + 1; ++k) {
        for (int l = j - 1; l <= j + 1; ++l) {
            if (channel == 'r') result += img_copy.r[k][l];
            else if (channel == 'g') result += img_copy.g[k][l];
            else if (channel == 'b') result += img_copy.b[k][l];
        }
    }
    return result / 9;
}

void *smoothing_r(void *) {
    for (int i = 1; i < rows - 1; ++i) {
        for (int j = 1; j < cols - 1; ++j) {
            image.r[i][j] = pixel_avg('r', i, j);
        }
    }
    return nullptr;
}

void *smoothing_g(void *) {
    for (int i = 1; i < rows - 1; ++i) {
        for (int j = 1; j < cols - 1; ++j) {
            image.g[i][j] = pixel_avg('g', i, j);
        }
    }
    return nullptr;
}

void *smoothing_b(void *) {
    for (int i = 1; i < rows - 1; ++i) {
        for (int j = 1; j < cols - 1; ++j) {
            image.b[i][j] = pixel_avg('b', i, j);
        }
    }
    return nullptr;
}

void smoothing() {
    img_copy = image;
    pthread_t threads[3];
    pthread_create(&threads[0], nullptr, &smoothing_r, nullptr);
    pthread_create(&threads[1], nullptr, &smoothing_g, nullptr);
    pthread_create(&threads[2], nullptr, &smoothing_b, nullptr);
    for (unsigned long thread : threads) pthread_join(thread, nullptr);
}

void *sepia_r(void *) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int red = (img_copy.r[i][j] * 0.393) + (img_copy.g[i][j] * 0.769) + (img_copy.b[i][j] * 0.189);
            if (red >= 255) image.r[i][j] = 255;
            else image.r[i][j] = red;
        }
    }
    return nullptr;
}

void *sepia_g(void *) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int green = (img_copy.r[i][j] * 0.349) + (img_copy.g[i][j] * 0.686) + (img_copy.b[i][j] * 0.168);
            if (green >= 255) image.g[i][j] = 255;
            else image.g[i][j] = green;
        }
    }
    return nullptr;
}

void *sepia_b(void *) {
    img_copy = image;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int blue = (img_copy.r[i][j] * 0.272) + (img_copy.g[i][j] * 0.534) + (img_copy.b[i][j] * 0.131);
            if (blue >= 255) image.b[i][j] = 255;
            else image.b[i][j] = blue;
        }
    }
    return nullptr;
}

void sepia() {
    img_copy = image;
    pthread_t threads[3];
    pthread_create(&threads[0], nullptr, &sepia_r, nullptr);
    pthread_create(&threads[1], nullptr, &sepia_g, nullptr);
    pthread_create(&threads[2], nullptr, &sepia_b, nullptr);
    for (unsigned long thread : threads) pthread_join(thread, nullptr);
}

void *channel_avg(void *channel) {
    unsigned long long int result = 0;
    long c = (long) channel;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (c == 'r') result += image.r[i][j];
            else if (c == 'g') result += image.g[i][j];
            else if (c == 'b') result += image.b[i][j];
        }
    }
    return (void *) (result / (rows * cols));
}

void washed_out() {
    void *red, *green, *blue;
    char r = 'r', g = 'g', b = 'b';

    pthread_t threads[3];
    pthread_create(&threads[0], nullptr, &channel_avg, (void *) r);
    pthread_create(&threads[1], nullptr, &channel_avg, (void *) g);
    pthread_create(&threads[2], nullptr, &channel_avg, (void *) b);

    pthread_join(threads[0], &red);
    pthread_join(threads[1], &green);
    pthread_join(threads[2], &blue);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            image.r[i][j] = (image.r[i][j] * 0.4) + ((long) red * 0.6);
            image.g[i][j] = (image.g[i][j] * 0.4) + ((long) green * 0.6);
            image.b[i][j] = (image.b[i][j] * 0.4) + ((long) blue * 0.6);
        }
    }
}

void *cross_main(void *) {
    for (int i = 1; i < rows - 1; ++i) {
        image.r[i][i] = 255;
        image.g[i][i] = 255;
        image.b[i][i] = 255;

        image.r[i + 1][i] = 255;
        image.g[i + 1][i] = 255;
        image.b[i + 1][i] = 255;

        image.r[i - 1][i] = 255;
        image.g[i - 1][i] = 255;
        image.b[i - 1][i] = 255;
    }
    return nullptr;
}

void *cross_side(void *) {
    for (int i = 1; i < rows - 1; ++i) {
        for (int j = 0; j < cols; ++j) {
            if ((i + j) == rows) {
                image.r[i][j] = 255;
                image.g[i][j] = 255;
                image.b[i][j] = 255;

                image.r[i + 1][j] = 255;
                image.g[i + 1][j] = 255;
                image.b[i + 1][j] = 255;

                image.r[i - 1][j] = 255;
                image.g[i - 1][j] = 255;
                image.b[i - 1][j] = 255;
            }
        }
    }
    return nullptr;
}

void cross() {
    pthread_t threads[2];
    pthread_create(&threads[0], nullptr, &cross_main, nullptr);
    pthread_create(&threads[1], nullptr, &cross_side, nullptr);
    for (unsigned long thread : threads) pthread_join(thread, nullptr);
}

int main(int argc, char *argv[]) {
    char *fileBuffer;
    int bufferSize;
    char *fileName = argv[1];
    auto begin = chrono::high_resolution_clock::now();

    if (!fillAndAllocate(fileBuffer, fileName, bufferSize)) {
        cout << "File read error" << endl;
        return 1;
    }

    getPixlesFromBMP24(bufferSize, fileBuffer);
    smoothing();
    sepia();
    washed_out();
    cross();
    writeOutBmp24(fileBuffer, "filtered.bmp", bufferSize);

    auto end = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << endl;

    return 0;
}