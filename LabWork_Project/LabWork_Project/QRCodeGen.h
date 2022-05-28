#pragma once
#include <iostream>
#include <string>
#include <vector>
//#include <cmath>
//#include <cassert>
//#include "CImg.h"
using namespace std;


namespace qrcodegen {
    class QRCodeGen
    {
        struct QRCODE {
            string str;
            string bitStr;
            int maskCode = -1;
            int version = 1;
            int correctionLevel = -1;
            vector<vector<string>> informationBlocks;
            vector<vector<string>> correctionBlocks;
        };

        struct OutPutMatrix {
            size_t size;
            int** array;
            OutPutMatrix(size_t size);
        };

        string DecimalToBinary(int number);

        int BinaryToDecimal(string str);

        void StrEncoder(QRCODE& str);

        void AddingServiceFields(QRCODE& str);

        void AddingExtraBits(QRCODE& str);

        void DividingToBlocks(QRCODE& str);

        void CreatingCorrectionBytes(QRCODE& str);

        void CombiningBlocks(QRCODE& str);

        void SetFunctionModule(OutPutMatrix arr, int x, int y, bool isDark);

        void DrawFormatBits(QRCODE str, OutPutMatrix arr);

        void DrawAlignmentPattern(OutPutMatrix arr, int x, int y);

        void DrawSearchPart(OutPutMatrix arr, int x, int y);

        void DrawVersion(QRCODE str, OutPutMatrix arr);

        void ToImg(OutPutMatrix mtx, string t_color, string b_color);

        void Draw(QRCODE str, string t_color, string b_color);

    public:
        void QRCode(string str, int correctionLevel, int maskCode, string t_color, string b_color);
    };

}