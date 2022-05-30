#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <cassert>
#include "CImg.h"
using namespace cimg_library;

using std::string; using std::to_string; using std::swap; using std::max; using std::vector;
class QR {
    private:

        friend class OutPutMatrix;

        string textStr;
        string bitStr;
        int maskCode = -1;
        int version = 1;
        int correctionLevel = -1;
        bool bitCoding = false;
        vector<vector<string>> informationBlocks;
        vector<vector<string>> correctionBlocks;
        
        string DecimalToBinary(int number);
        int BinaryToDecimal(string str);
        void StrEncoder();
        void AddingServiceFields();
        void AddingExtraBits();
        void DividingToBlocks();
        void CreatingCorrectionBytes();
        void CombiningBlocks();
        void DrawToImg(string t_color, string b_color);

    public:

        friend void Draw(QR qrcode, string t_color, string b_color);
        QR(string textStr, int maskCode = rand() % 8, int correctionLevel = 2) : textStr(textStr), maskCode(maskCode), correctionLevel(correctionLevel)
        {}
    };

class OutPutMatrix {
    friend class QR;

    size_t size;
    int** array;

    OutPutMatrix(size_t size);
    ~OutPutMatrix();

    void SetFunctionModule(int x, int y, bool isDark);
    void DrawFormatBits(QR qr);
    void DrawAlignmentPattern(int x, int y);
    void DrawSearchPart(int x, int y);
    void DrawVersion(QR qr);
    void ToImg(string t_color, string b_color);
};

void Draw(QR qrcode, string t_color, string b_color);