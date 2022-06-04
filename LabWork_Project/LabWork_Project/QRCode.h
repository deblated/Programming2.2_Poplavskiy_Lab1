#pragma once
#include <string>
#include <vector>

using std::string;  using std::vector; 

enum text_colors{ black, green, blue, brown };
enum background_colors { white, yellow, orange };
enum encoding_mode { alphanumeric, binary };

class OutPutMatrix;

class QR {
    private:

        string textStr;
        string bitStr;
        int maskCode = -1;
        int version = 1;
        int correctionLevel = -1;
        encoding_mode mode;
        vector<vector<string>> informationBlocks;
        vector<vector<string>> correctionBlocks;
        
        void StrEncodeBit();
        string DecimalToBinary(int number);
        int BinaryToDecimal(string str);
        void StrEncoder();
        void AddingServiceFields();
        void AddingExtraBits();
        void DividingToBlocks();
        void CreatingCorrectionBytes();
        void CombiningBlocks();
        void DrawingArray(text_colors t_color, background_colors b_color);

    public:

        int GetCorrectionLevel();
        int GetMaskCode();
        string GetInfo();
        encoding_mode GetMode();
        void Draw(QR qrcode, text_colors t_color, background_colors b_color);
        QR(string textStr, encoding_mode mode, int maskCode = rand() % 8, int correctionLevel = 2) : textStr(textStr), maskCode(maskCode), correctionLevel(correctionLevel), mode(mode)
        {}
    };