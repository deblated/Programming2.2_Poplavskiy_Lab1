#pragma once

#include <string>
#include <vector>
#include "../LabWork_Project/Constants.h"
#include "../LabWork_Project/CImg.h"
#include <cmath>
#include <ctime>
#include <cassert>
#include <bitset>

using std::max;
using std::bitset; using std::to_string; using std::swap; using std::max; using std::abs; using std::string;
using std::begin; using std::end; using std::copy; using std::stoi; using std::vector;
   
using namespace cimg_library;

enum text_colors{ black, green, blue, brown };
enum background_colors { white, yellow, orange };
enum encoding_mode { alphanumeric, binary };

class QRTestClass;

class OutPutMatrix {
private:

    friend class QR;

    size_t size;
    int** array;


    OutPutMatrix(size_t size);

    ~OutPutMatrix();

    void SetFunctionModule(int x, int y, bool isDark);
    void DrawFormatBits(QR qr);
    void DrawAlignmentPattern(int x, int y);
    void DrawSearchPattern(int x, int y);
    void DrawVersion(QR qr);
    void ToImg(text_colors t_color, background_colors b_color);
};

class QR {
    private:
        friend class QRTestClass;

        string textStr;
        string bitStr;
        int maskCode = -1;
        int version = 1;
        int correctionLevel = -1;
        encoding_mode mode;
        vector<vector<string>> informationBlocks;
        vector<vector<string>> correctionBlocks;
        
        void StrEncodeBinary();
        string DecimalToBinary(int number);
        int BinaryToDecimal(string str);
        void StrEncoderAlphanumeric();
        void AddingServiceFields(bool needToPlusVersion=false);
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
        void Draw(text_colors t_color, background_colors b_color);

        QR(string textStr, encoding_mode mode, int maskCode = rand() % 8, int correctionLevel = 2) : textStr(textStr), maskCode(maskCode), correctionLevel(correctionLevel), mode(mode)
        {}
    };

void QR::StrEncodeBinary() {
    string result = "";
    for (char& _char : textStr) {
        result += bitset<8>(_char).to_string();
    }
    bitStr = result;
}
string QR::DecimalToBinary(int number) {
    string buffer;
    do {
        buffer += to_string(number % 2);
        number = number / 2;
    } while (number > 0);

    for (size_t j = 0; j < buffer.size() / 2; j++) {
        swap(buffer[j], buffer[buffer.size() - j - 1]);
    }
    return buffer;
}
int QR::BinaryToDecimal(string str) {
    int num = stoi(str);
    int dec_value = 0;

    int base = 1;

    int temp = num;
    while (temp) {
        int last_digit = temp % 10;
        temp = temp / 10;

        dec_value += last_digit * base;

        base = base * 2;
    }

    return dec_value;
}
void QR::StrEncoderAlphanumeric() {
    string result;
    string buffer;
    int* pairs = new int[(textStr.size() / 2) + 1];

    //splitting numbers into pairs
    int j = 0;
    for (size_t i = 0; i <= textStr.size() - 1; i += 2) {
        assert(!((textStr[i] && CodingTable[textStr[i]] == 0 && textStr[i] != '0') || (textStr[i + 1] && CodingTable[textStr[i + 1]] == 0 && textStr[i + 1] != '0')) && "Invalid symbol is found. Try to examine the list of valid symbols");

        if (textStr[i] && textStr[i + 1]) {
            pairs[j] = CodingTable[textStr[i]] * 45 + CodingTable[textStr[i + 1]];
        }
        else {
            pairs[j] = CodingTable[textStr[i]];
        }
        j++;
    }
    //making a binary string from these numbers
    int number;
    for (size_t i = 0; i < (textStr.size() / 2) + (textStr.size() % 2); i++) {
        number = pairs[i];
        buffer = DecimalToBinary(number);
        if (textStr.size() % 2 == 0) {
            while (buffer.size() != 11) {
                buffer = '0' + buffer;
            }
        }
        if (textStr.size() % 2 != 0) {
            while ((i == textStr.size() / 2) ? buffer.size() != 6 : buffer.size() != 11) {
                buffer = '0' + buffer;
            }
        }
        result += buffer;
        buffer = "";
    }
    delete[]pairs;

    bitStr = result;
}
void QR::AddingServiceFields(bool needToPlusVersion) {
    string buffer;
    string result;

    //binary number of string length
    int number;
    switch (GetMode()) {
    case alphanumeric:
        number = textStr.size();
        buffer = DecimalToBinary(number);
        break;
    case binary:
        number = bitStr.size() / 8;
        buffer = DecimalToBinary(number);
        break;
    }

    //looking for the right version
    //switch (GetMode()) {
    //case alphanumeric:
    //    while (textStr.size() > MaximalAmountOfInfo[correctionLevel][version] && version < 40) {
    //        version++;
    //    }
    //    break;
    //case binary:
        while (bitStr.size() > MaxAmountOfInfo[correctionLevel][version] && version < 40) {
            version++;
        }
    //    break;
    //}

    if (needToPlusVersion == true) {
        version++;
    }

    //length adjustment
    if (version >= 1 && version <= 9) {
        switch (GetMode()) {
        case alphanumeric:
            while (buffer.size() != 9) {
                buffer = '0' + buffer;
            }
            break;
        case binary:
            while (buffer.size() != 8) {
                buffer = '0' + buffer;
            }
            break;
        }
    }
    else if (version >= 10 && version <= 26) {
        switch (GetMode()) {
        case alphanumeric:
            while (buffer.size() != 11) {
                buffer = '0' + buffer;
            }
            break;
        case binary:
            while (buffer.size() != 16) {
                buffer = '0' + buffer;
            }
            break;
        }
    }
    else {
        switch (GetMode()) {
        case alphanumeric:
            while (buffer.size() != 13) {
                buffer = '0' + buffer;
            }
            break;
        case binary:
            while (buffer.size() != 16) {
                buffer = '0' + buffer;
            }
            break;
        }
    }

    //merging info
    switch (GetMode()) {
    case alphanumeric:
        buffer = "0010" + buffer;
        break;
    case binary:
        buffer = "0100" + buffer;
        break;
    }

    result = buffer + bitStr;

    if (result.size() > MaxAmountOfInfo[correctionLevel][version]) {
        if (version == 40) {
            assert(!(result.size() > MaxAmountOfInfo[correctionLevel][40]) && "Length of string is more than allowed");
        }
        AddingServiceFields(true);
    }
    else {
        bitStr = result;
    }
}
void QR::AddingExtraBits() {
    string result;
    if (MaxAmountOfInfo[correctionLevel][version] - bitStr.size() > 4) {
        for (size_t i = 0; i < 4; i++) {
            bitStr += '0';
        }
    }
    else {
        for (size_t i = 0; i < MaxAmountOfInfo[correctionLevel][version] - bitStr.size(); i++) {
            bitStr += '0';
        }
    }

    while (bitStr.size() % 8 != 0) {
        bitStr = bitStr + '0';
    }

    int number = ((MaxAmountOfInfo[correctionLevel][version] - bitStr.size()) / 8);
    for (size_t i = 1; i <= number; i++) {
        if (i % 2 == 1) {
            bitStr = bitStr + "11101100";
        }
        else if (i % 2 == 0) {
            bitStr = bitStr + "00010001";
        }
    }
}
void QR::DividingToBlocks() {
    vector<string> buffer;
    string strBuffer;
    string ch;
    int counter = 0;
    int amountOfBlocks = AmountOfBlocks[correctionLevel][version];
    int numberOfBytesInBlock = (MaxAmountOfInfo[correctionLevel][version] / 8) / amountOfBlocks;
    int numberOfAugmentedBlocks = (MaxAmountOfInfo[correctionLevel][version] / 8) % amountOfBlocks;

    //filling blocks
    for (size_t i = 0; i < amountOfBlocks - numberOfAugmentedBlocks; i++) {
        for (size_t j = 0; j < numberOfBytesInBlock * 8; j++) {
            ch = bitStr[counter];
            strBuffer = strBuffer + ch;
            counter++;
            if (strBuffer.size() == 8) {
                buffer.push_back(strBuffer);
                strBuffer = "";
            }
        }
        informationBlocks.push_back(buffer);
        buffer.clear();
    }

    //filling larger blocks
    for (size_t i = 0; i < numberOfAugmentedBlocks; i++) {
        for (size_t j = 0; j < (numberOfBytesInBlock + 1) * 8; j++) {
            ch = bitStr[counter];
            strBuffer = strBuffer + ch;
            counter++;
            if (strBuffer.size() == 8) {
                buffer.push_back(strBuffer);
                strBuffer = "";
            }
        }
        informationBlocks.push_back(buffer);
        buffer.clear();
    }
}
void QR::CreatingCorrectionBytes() {
    string buffer;
    int number;
    int A, B, C;
    int Poly[30];
    int PolyBuf[30];
    int counter = 2;
    vector<string> block;

    int amountOfCorrectionBytes = AmountOfCorrectionBytesPerBlock[correctionLevel][version];

    for (size_t i = 0; i < 13; i++) {
        if (GeneratingPolynomial[i][0] == amountOfCorrectionBytes) {
            while (GeneratingPolynomial[i][counter] != -1 && counter != 32) {
                Poly[counter - 2] = GeneratingPolynomial[i][counter];
                counter++;
            }
        }
    }
    counter = counter - 2;

    for (size_t i = 0; i < informationBlocks.size(); i++) {
        int maximal = max((int)informationBlocks[i].size(), amountOfCorrectionBytes);
        int* arr = new int[maximal] {0};

        for (size_t j = 0; j < informationBlocks[i].size(); j++) {
            arr[j] = BinaryToDecimal(informationBlocks[i][j]);
        }

        for (size_t j = 0; j < informationBlocks[i].size(); j++) {
            copy(begin(Poly), end(Poly), begin(PolyBuf));

            A = arr[0];
            for (size_t f = 1; f < maximal; f++) {
                arr[f - 1] = arr[f];
            }
            arr[maximal - 1] = 0;

            if (A != 0) {
                B = IGF[A];
                for (size_t z = 0; z < amountOfCorrectionBytes; z++) {
                    PolyBuf[z] = ((PolyBuf[z] + B) % 255);
                }
                for (size_t y = 0; y < amountOfCorrectionBytes; y++) {
                    arr[y] = GF[PolyBuf[y]] ^ arr[y];
                }
            }

        }

        for (size_t j = 0; j < maximal; j++) {
            number = arr[j];

            buffer = DecimalToBinary(number);

            while (buffer.size() != 8) {
                buffer = '0' + buffer;
            }
            block.push_back(buffer);
            buffer = "";
        }

        correctionBlocks.push_back(block);
        block.clear();
        delete arr;
    }
}
void QR::CombiningBlocks() {
    bitStr = "";
    int amountOfBlocks = AmountOfBlocks[correctionLevel][version];
    int numberOfAugmentedBlocks = (MaxAmountOfInfo[correctionLevel][version] / 8) % amountOfBlocks;

    //combining information blocks and correction blocks
    for (size_t i = 0; i < informationBlocks[amountOfBlocks - numberOfAugmentedBlocks - 1].size(); i++) {
        for (size_t j = 0; j < informationBlocks.size(); j++) {
            bitStr += informationBlocks[j][i];
        }
    }
    for (size_t i = amountOfBlocks - numberOfAugmentedBlocks; i < amountOfBlocks; i++) {
        bitStr += informationBlocks[i][informationBlocks[i].size() - 1];
    }
    for (size_t i = 0; i < AmountOfCorrectionBytesPerBlock[correctionLevel][version]; i++) {
        for (size_t j = 0; j < informationBlocks.size(); j++) {
            bitStr += correctionBlocks[j][i];
        }
    }
}
void QR::DrawingArray(text_colors t_color, background_colors b_color) {
    int numAlign = 0;
    int size = -1;

    //counting number of alignment pattern and size of array
    for (size_t i = 0; i < 7; i++) {
        if (AlignmentPatterns[version - 1][i] != -1) {
            numAlign++;
        }
        if (AlignmentPatterns[version - 1][i] > size) {
            size = AlignmentPatterns[version - 1][i];
        }
    }
    (size == -1) ? size = 21 : size += 7;

    OutPutMatrix arr(size);

    //drawing sync strips
    for (size_t i = 0; i < arr.size; i++) {
        arr.SetFunctionModule(6, i, i % 2 == 0);
        arr.SetFunctionModule(i, 6, i % 2 == 0);
    }

    //drawing search patterns
    arr.DrawSearchPattern(3, 3);
    arr.DrawSearchPattern(arr.size - 4, 3);
    arr.DrawSearchPattern(3, arr.size - 4);

    //drawing alignment patterns
    for (size_t i = 0; i < numAlign; i++) {
        for (size_t j = 0; j < numAlign; j++) {
            if (version > 6) {
                if (!((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0)))
                    arr.DrawAlignmentPattern(AlignmentPatterns[version - 1][i], AlignmentPatterns[version - 1][j]);
            }
            else {
                arr.DrawAlignmentPattern(AlignmentPatterns[version - 1][i], AlignmentPatterns[version - 1][j]);
            }
        }
    }

    //drawing version, mask code and correction level
    arr.DrawVersion(*this);
    arr.DrawFormatBits(*this);

    //inserting info to the array (by zigzag)
    size_t i = 0;
    for (int right = size - 1; right >= 1; right -= 2) {
        if (right == 6)
            right = 5;
        for (int vert = 0; vert < size; vert++) {
            for (int j = 0; j < 2; j++) {
                size_t x = static_cast<size_t>(right - j);
                bool upward = ((right + 1) & 2) == 0;
                size_t y = static_cast<size_t>(upward ? size - 1 - vert : vert);
                if (arr.array[y][x] != 3 && arr.array[y][x] != 4 && i < bitStr.size()) {
                    (bitStr[i] == '0') ? arr.array[y][x] = 0 : arr.array[y][x] = 1;
                    i++;
                }
            }
        }
    }

    //applying mask
    for (size_t y = 0; y < arr.size; y++) {
        for (size_t x = 0; x < arr.size; x++) {
            bool invert;
            switch (maskCode) {
            case 0:  invert = (x + y) % 2 == 0;                    break;
            case 1:  invert = y % 2 == 0;                          break;
            case 2:  invert = x % 3 == 0;                          break;
            case 3:  invert = (x + y) % 3 == 0;                    break;
            case 4:  invert = (x / 3 + y / 2) % 2 == 0;            break;
            case 5:  invert = (x * y % 2) + ((x * y) % 3) == 0;    break;
            case 6:  invert = (x * y % 2 + x * y % 3) % 2 == 0;    break;
            case 7:  invert = ((x * y) % 3 + x + y) % 2 == 0;      break;
            }
            arr.array[y][x] = arr.array[y][x] ^ (invert & (arr.array[y][x] != 3 && arr.array[y][x] != 4));
        }
    }

    //drawing to file
    arr.ToImg(t_color, b_color);

}
int QR::GetCorrectionLevel() {
    return correctionLevel;
}
int QR::GetMaskCode() {
    return maskCode;
}
string QR::GetInfo() {
    return textStr;
}
encoding_mode QR::GetMode() {
    return mode;
}
void QR::Draw(text_colors t_color, background_colors b_color) {
    //assert((t_color == black || t_color == blue || t_color == brown || t_color == green) && "t_color value can only be 'black', 'brown', 'green' or 'blue'");
    //assert((b_color == white || b_color == yellow || b_color == orange) && "b_color value can only be 'white', 'yellow' or 'orange'");
    assert(!(correctionLevel > 4 || correctionLevel < 1) && "Invalid correction level entered. The value must be between 1 and 4");
    assert(!(maskCode > 7 || maskCode < 0) && "Invalid mask code entered. The value must be between 0 and 7");  
    assert(!(textStr.size() == 0) && "Length can't be equal to 0");
    //if (mode == alphanumeric) {
    //    assert(!(textStr.size() > MaximalAmountOfInfo[correctionLevel][40]) && "Length of string is more than allowed");
    //}


    switch (GetMode()) {
    case alphanumeric:
        StrEncoderAlphanumeric();
        break;
    case binary:
        StrEncodeBinary();
        break;
    }

    AddingServiceFields();
    AddingExtraBits();
    DividingToBlocks();
    CreatingCorrectionBytes();
    CombiningBlocks();
    DrawingArray(t_color, b_color);
}



OutPutMatrix::OutPutMatrix(size_t size) : size(size)
{
    array = new int* [size];
    for (size_t count = 0; count < size; count++) {
        array[count] = new int[size];
    }
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            array[i][j] = 0;
        }
    }
}
OutPutMatrix::~OutPutMatrix() {
    for (int i = 0; i < size; i++) {
        delete[] array[i];
    }
    delete[] array;
}
void OutPutMatrix::SetFunctionModule(int x, int y, bool isDark) {
    if (isDark) {
        array[y][x] = 3;
    }
    else {
        array[y][x] = 4;
    }
}
void OutPutMatrix::DrawFormatBits(QR qr) {
    string buffer;
    bool bit = true;

    //1st place
    for (size_t i = 9; i < 15; i++) {
        buffer = MaskCodes[8 * (qr.GetCorrectionLevel() - 1) + qr.GetMaskCode()][i];
        (buffer == "1") ? bit = true : bit = false;
        SetFunctionModule(8, 14 - i, bit);
    }
    buffer = MaskCodes[8 * (qr.GetCorrectionLevel() - 1) + qr.GetMaskCode()][6];
    (buffer == "1") ? bit = true : bit = false;
    SetFunctionModule(7, 8, bit);
    buffer = MaskCodes[8 * (qr.GetCorrectionLevel() - 1) + qr.GetMaskCode()][7];
    (buffer == "1") ? bit = true : bit = false;
    SetFunctionModule(8, 8, bit);
    buffer = MaskCodes[8 * (qr.GetCorrectionLevel() - 1) + qr.GetMaskCode()][8];
    (buffer == "1") ? bit = true : bit = false;
    SetFunctionModule(8, 7, bit);
    for (size_t i = 0; i <= 5; i++) {
        buffer = MaskCodes[8 * (qr.GetCorrectionLevel() - 1) + qr.GetMaskCode()][i];
        (buffer == "1") ? bit = true : bit = false;
        SetFunctionModule(i, 8, bit);
    }

    //2nd place
    for (size_t i = 7; i < 15; i++) {
        buffer = MaskCodes[8 * (qr.GetCorrectionLevel() - 1) + qr.GetMaskCode()][i];
        (buffer == "1") ? bit = true : bit = false;
        SetFunctionModule(size - 15 + i, 8, bit);
    }
    for (size_t i = 0; i < 7; i++) {
        buffer = MaskCodes[8 * (qr.GetCorrectionLevel() - 1) + qr.GetMaskCode()][i];
        (buffer == "1") ? bit = true : bit = false;
        SetFunctionModule(8, size - 1 - i, bit);
    }
    SetFunctionModule(8, size - 8, true);
}
void OutPutMatrix::DrawAlignmentPattern(int x, int y) {
    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++)
            SetFunctionModule(x + dx, y + dy, max(abs(dx), abs(dy)) != 1);
    }
}
void OutPutMatrix::DrawSearchPattern(int x, int y) {
    for (int dy = -4; dy <= 4; dy++) {
        for (int dx = -4; dx <= 4; dx++) {
            int dist = max(abs(dx), abs(dy));
            int xx = x + dx, yy = y + dy;
            if (0 <= xx && xx < size && 0 <= yy && yy < size)
                SetFunctionModule(xx, yy, dist != 2 && dist != 4);
        }
    }
}
void OutPutMatrix::DrawVersion(QR qr) {
    int version = 1;
    switch (qr.GetMode()) {
    case alphanumeric:
        while (qr.GetInfo().size() > MaximalAmountOfInfo[qr.GetCorrectionLevel()][version]) {
            version++;
        }
        break;
    case binary:
        while (qr.GetInfo().size() * 8 > MaxAmountOfInfo[qr.GetCorrectionLevel()][version]) {
            version++;
        }
        break;
    }
    if (version < 7) {
        return;
    }
    bool bit = true;
    string buffer;
    for (size_t i = 0; i < 18; i++) {
        buffer = versionCodes[version - 7][i];
        (buffer == "1") ? bit = true : bit = false;
        int a = size - 11 + (i / 6);
        int b = i % 6;
        SetFunctionModule(a, b, bit);
        SetFunctionModule(b, a, bit);
    }
}
void OutPutMatrix::ToImg(text_colors t_color, background_colors b_color) {
    unsigned int w = size * 10 + 50;
    unsigned int h = size * 10 + 50;
    unsigned char t_col[3];
    unsigned char b_col[3];

    switch (t_color) {
    case black: t_col[0] = 0; t_col[1] = 0; t_col[2] = 0; break;
    case green:  t_col[0] = 0; t_col[1] = 100; t_col[2] = 0; break;
    case blue: t_col[0] = 0; t_col[1] = 0; t_col[2] = 128; break;
    case brown: t_col[0] = 78; t_col[1] = 53; t_col[2] = 36; break;
    default: t_col[0] = 0; t_col[1] = 0; t_col[2] = 0; break;
    }
    switch (b_color) {
    case white: b_col[0] = 255; b_col[1] = 255; b_col[2] = 255; break;
    case yellow: b_col[0] = 255; b_col[1] = 255; b_col[2] = 0; break;
    case orange: b_col[0] = 255; b_col[1] = 165; b_col[2] = 0; break;
    default: b_col[0] = 255; b_col[1] = 255; b_col[2] = 255; break;
    }

    CImg<unsigned char> image(w, h, 1, 3, 255);
    image.draw_rectangle(0, 0, size * 10 + 50, size * 10 + 50, b_col, 1);
    int k, t = 0;
    for (size_t j = 0; j < size; j++) {
        k = 0;
        for (size_t i = 0; i < size; i++) {
            if (array[j][i] == 1 || array[j][i] == 3) {
                image.draw_rectangle(k + 25, t + 24, k + 25 + 9, t + 24 + 9, t_col, 1);
                k += 10;
            }
            else if (array[j][i] == 0 || array[j][i] == 4 || array[j][i] == -1) {
                image.draw_rectangle(k + 25, t + 24, k + 25 + 9, t + 24 + 9, b_col, 1);
                k += 10;
            }
        }
        t += 10;
        k = 0;
        k = 0;
    }

    char filename[15];
    for (size_t i = 0; i < 10; i++) {
        filename[i] = rand() % 26 + 'a';
    }
    filename[10] = '.';
    filename[11] = 'j';
    filename[12] = 'p';
    filename[13] = 'g';
    filename[14] = '\0';

    image.save_bmp(filename);
}