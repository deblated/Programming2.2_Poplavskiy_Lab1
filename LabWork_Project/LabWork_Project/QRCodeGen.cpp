#include "QRCodeGen.h"
#include <cmath>
#include <ctime>
#include <cassert>
#include <bitset>
#include "CImg.h"
using namespace cimg_library;
using std::bitset; using std::to_string; using std::swap; using std::max;

    const int RemBits[40]{
        0,	7,	7,	7,	7,	7,	0,	0,
        0,	0,	0,	0,	0,	3,	3,	3,
        3,	3,	3,	3,	4,	4,	4,	4,
        4,	4,	4,	3,	3,	3,	3,	3,
        3,	3,	0,	0,	0,	0,	0,	0
    };
    const int CodingTable[128]{
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//0-15
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16-31
        36,0,0,0,37,38,0,0,0,0,39,40,0,41,42,43,//32-47
        0,1,2,3,4,5,6,7,8,9,44,0,0,0,0,0,//48-63
        0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,//64-79
        25,26,27,28,29,30,31,32,33,34,35,0,0,0,0,0,//80-95
        0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,//96-111
        25,26,27,28,29,30,31,32,33,34,35,0,0,0,0,0//112-127
    };
    const int MaxAmountOfInfo[5][41]{
       -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
       -1,152,272,440,640,864,1088,1248,1552,1856,2192,2592,2960,3424,3688,4184,4712,5176,5768,6360,6888,7456,8048,8752,9392,10208,10960,11744,12248,13048,13880,14744,15640,16568,17528,18448,19472,20528,21616,22496,23648,
       -1,128,224,352,512,688,864,992,1232,1456,1728,2032,2320,2672,2920,3320,3624,4056,4504,5016,5352,5712,6256,6880,7312,8000,8496,9024,9544,10136,10984,11640,12328,13048,13800,14496,15312,15936,16816,17728,18672,
       -1,104,176,272,384,496,608,704,880,1056,1232,1440,1648,1952,2088,2360,2600,2936,3176,3560,3880,4096,4544,4912,5312,5744,6032,6464,6968,7288,7880,8264,8920,9368,9848,10288,10832,11408,12016,12656,13328,
       -1,72,128,208,288,368,480,528,688,800,976,1120,1264,1440,1576,1784,2024,2264,2504,2728,3080,3248,3536,3712,4112,4304,4768,5024,5288,5608,5960,6344,6760,7208,7688,7888,8432,8768,9136,9776,10208
    };
    const int MaximalAmountOfInfo[5][41]{
       -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
       -1,25,47,77,114,154,195,224,279,335,395,468,535,619,667,758,854,938,1046,1153,1249,1352,1460,1588,1704,1853,1990,2132,2223,2369,2520,2677,2840,3009,3183,3351,3537,3729,3927,4087,4296,
       -1,20,38,61,90,122,154,178,221,262,311,366,419,483,528,600,656,734,816,909,970,1035,1134,1248,1326,1451,1542,1637,1732,1839,1994,2113,2238,2369,2506,2632,2780,2894,3054,3220,3391,
       -1,16,29,47,67,87,108,125,157,189,221,259,296,352,376,426,470,531,574,644,702,742,823,890,963,1041,1094,1172,1263,1322,1429,1499,1618,1700,1787,1867,1966,2071,2181,2298,2420,
       -1,10,20,36,50,64,84,93,122,143,174,200,227,259,283,321,365,408,452,493,557,587,640,672,744,779,864,910,958,1016,1080,1150,1226,1307,1394,1431,1530,1591,1658,1774,1852
    };
    const int AmountOfBlocks[5][41]{
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,1,1,1,1,1,2,2,2,2,4,4,4,4,4,6,6,6,6,7,8,8,9,9,10,12,12,12,13,14,15,16,17,18,19,19,20,21,22,24,25,
        -1,1,1,1,2,2,4,4,4,5,5,5,8,9,9,10,10,11,13,14,16,17,17,18,20,21,23,25,26,28,29,31,33,35,37,38,40,43,45,47,49,
        -1,1,1,2,2,4,4,6,6,8,8,8,10,12,16,12,17,16,18,21,20,23,23,25,27,29,34,34,35,38,40,43,45,48,51,53,56,59,62,65,68,
        -1,1,1,2,4,4,4,5,6,8,8,11,11,16,16,18,16,19,21,25,25,25,34,30,32,35,37,40,42,45,48,51,54,57,60,63,66,70,74,77,81
    };
    const int AmountOfCorrectionBytesPerBlock[5][41]{
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,7,10,15,20,26,18,20,24,30,18,20,24,26,30,22,24,28,30,28,28,28,28,30,30,26,28,30,30,30,30,30,30,30,30,30,30,30,30,30,30,
        -1,10,16,26,18,24,16,18,22,22,26,30,22,22,24,24,28,28,26,26,26,26,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,
        -1,13,22,18,26,18,24,18,22,20,24,28,26,24,20,30,24,28,28,26,30,28,30,30,30,30,28,30,30,30,30,30,30,30,30,30,30,30,30,30,30,
        -1,17,28,22,16,22,28,26,26,24,28,24,28,22,24,24,30,28,28,26,28,30,24,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30
    };
    const int GeneratingPolynomial[13][32]{
        7,-1,87,229,146,149,238,102,21,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        10,-1,251,67,46,61,118,70,64,94,32,45,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        13,-1,74,152,176,100,86,100,106,104,130,218,206,140,78,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        15,-1,8,183,61,91,202,37,51,58,58,237,140,124,5,99,105,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        16,-1,120,104,107,109,102,161,76,3,91,191,147,169,182,194,225,120,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        17,-1,43,139,206,78,43,239,123,206,214,147,24,99,150,39,243,163,136,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        18,-1,215,234,158,94,184,97,118,170,79,187,152,148,252,179,5,98,96,153,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        20,-1,17,60,79,50,61,163,26,187,202,180,221,225,83,239,156,164,212,212,188,190,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        22,-1,210, 171, 247,242,93,230,14,109,221,53,200,74,8,172,98,80,219,134,160,105,165,231,-1,-1,-1,-1,-1,-1,-1,-1,
        24,-1,229,121,135,48,211,117,251,126,159,180,169,152,192,226,228,218,111,0,117,232,87,96,227,21,-1,-1,-1,-1,-1,-1,
        26,-1,173,125,158,2,103,182,118,17,145,201,111,28,165,53,161,21,245,142,13,102,48,227,153,145,218,70,-1,-1,-1,-1,
        28,-1,168,223,200,104,224,234,108,180,110,190,195,147,205,27,232,201,21,43,245,87,42,195,212,119,242,37,9,123,-1,-1,
        30,-1,41,173,145,152,216,31,179,182,50,48,110,86,239,96,222,125,42,173,226,193,224,130,156,37,251,216,238,40,192,180
    };
    const int GF[256]{
        1,2,4,8,16,32,64,128,29,58,116,232,205,135,19,38,
        76,152,45,90,180,117,234,201,143,3,6,12,24,48,96,192,
        157,39,78,156,37,74,148,53,106,212,181,119,238,193,159,35,
        70,140,5,10,20,40,80,160,93,186,105,210,185,111,222,161,
        95,190,97,194,153,47,94,188,101,202,137,15,30,60,120,240,
        253,231,211,187,107,214,177,127,254,225,223,163,91,182,113,226,
        217,175,67,134,17,34,68,136,13,26,52,104,208,189,103,206,
        129,31,62,124,248,237,199,147,59,118,236,197,151,51,102,204,
        133,23,46,92,184,109,218,169,79,158,33,66,132,21,42,84,
        168,77,154,41,82,164,85,170,73,146,57,114,228,213,183,115,
        230,209,191,99,198,145,63,126,252,229,215,179,123,246,241,255,
        227,219,171,75,150,49,98,196,149,55,110,220,165,87,174,65,
        130,25,50,100,200,141,7,14,28,56,112,224,221,167,83,166,
        81,162,89,178,121,242,249,239,195,155,43,86,172,69,138,9,
        18,36,72,144,61,122,244,245,247,243,251,235,203,139,11,22,
        44,88,176,125,250,233,207,131,27,54,108,216,173,71,142,1
    };
    const int IGF[256]{
        -1,0,1,25,2,50,26,198,3,223,51,238,27,104,199,75,
        4,100,224,14,52,141,239,129,28,193,105,248,200,8,76,113,
        5,138,101,47,225,36,15,33,53,147,142,218,240,18,130,69,
        29,181,194,125,106,39,249,185,201,154,9,120,77,228,114,166,
        6,191,139,98,102,221,48,253,226,152,37,179,16,145,34,136,
        54,208,148,206,143,150,219,189,241,210,19,92,131,56,70,64,
        30,66,182,163,195,72,126,110,107,58,40,84,250,133,186,61,
        202,94,155,159,10,21,121,43,78,212,229,172,115,243,167,87,
        7,112,192,247,140,128,99,13,103,74,222,237,49,197,254,24,
        227,165,153,119,38,184,180,124,17,68,146,217,35,32,137,46,
        55,63,209,91,149,188,207,205,144,135,151,178,220,252,190,97,
        242,86,211,171,20,42,93,158,132,60,57,83,71,109,65,162,
        31,45,67,216,183,123,164,118,196,23,73,236,127,12,111,246,
        108,161,59,82,41,157,85,170,251,96,134,177,187,204,62,90,
        203,89,95,176,156,169,160,81,11,245,22,235,122,117,44,215,
        79,174,213,233,230,231,173,232,116,214,244,234,168,80,88,175
    };
    const int AlignmentPatterns[40][7]{
        -1,-1,-1,-1,-1,-1,-1,
        18,-1,-1,-1,-1,-1,-1,
        22,-1,-1,-1,-1,-1,-1,
        26,-1,-1,-1,-1,-1,-1,
        30,-1,-1,-1,-1,-1,-1,
        34,-1,-1,-1,-1,-1,-1,
        6,22,38,-1,-1,-1,-1,
        6,24,42,-1,-1,-1,-1,

        6,26,46,-1,-1,-1,-1,
        6,28,50,-1,-1,-1,-1,
        6,30,54,-1,-1,-1,-1,
        6,32,58,-1,-1,-1,-1,
        6,34,62,-1,-1,-1,-1,
        6,26,46,66,-1,-1,-1,
        6,26,48,70,-1,-1,-1,
        6,26,50,74,-1,-1,-1,

        6,30,54,78,-1,-1,-1,
        6,30,56,82,-1,-1,-1,
        6,30,58,86,-1,-1,-1,
        6,34,62,90,-1,-1,-1,
        6,28,50,72,94,-1,-1,
        6,26,50,74,98,-1,-1,
        6,30,54,78,102,-1,-1,
        6,28,54,80,106,-1,-1,

        6,32,58,84,110,-1,-1,
        6,30,58,86,114,-1,-1,
        6,34,62,90,118,-1,-1,
        6,26,50,74,98,122,-1,
        6,30,54,78,102,126,-1,
        6,26,52,78,104,130,-1,
        6,30,56,82,108,134,-1,
        6,34,60,86,112,138,-1,

        6,30,58,86,114,142,-1,
        6,34,62,90,118,146,-1,
        6,30,54,78,102,126,150,
        6,24,50,76,102,128,154,
        6,28,54,80,106,132,158,
        6,32,58,84,110,136,162,
        6,26,54,82,110,138,166,
        6,30,58,86,114,142,170
    };
    const string versionCodes[34]{
        "000010011110100110",
        "010001011100111000",
        "110111011000000100",
        "101001111110000000",
        "001111111010111100",

        "001101100100011010",
        "101011100000100110",
        "110101000110100010",
        "010011000010011110",
        "011100010001011100",

        "111010010101100000",
        "100100110011100100",
        "000010110111011000",
        "000000101001111110",
        "100110101101000010",

        "111000001011000110",
        "011110001111111010",
        "001101001101100100",
        "101011001001011000",
        "110101101111011100",

        "010011101011100000",
        "010001110101000110",
        "110111110001111010",
        "101001010111111110",
        "001111010011000010",

        "101000011000101101",
        "001110011100010001",
        "010000111010010101",
        "110110111110101001",
        "110100100000001111",

        "010010100100110011",
        "001100000010110111",
        "101010000110001011",
        "111001000100010101"
    };
    const string MaskCodes[32]{
        "111011111000100",
        "111001011110011",
        "111110110101010",
        "111100010011101",
        "110011000101111",
        "110001100011000",
        "110110001000001",
        "110100101110110",

        "101010000010010",
        "101000100100101",
        "101111001111100",
        "101101101001011",
        "100010111111001",
        "100000011001110",
        "100111110010111",
        "100101010100000",

        "011010101011111",
        "011000001101000",
        "011111100110001",
        "011101000000110",
        "010010010110100",
        "010000110000011",
        "010111011011010",
        "010101111101101",

        "001011010001001",
        "001001110111110",
        "001110011100111",
        "001100111010000",
        "000011101100010",
        "000001001010101",
        "000110100001100",
        "000100000111011"
    };
    
    void QR::StrEncodeBit()
    {
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
        int num = std::stoi(str);
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
    void QR::StrEncoder() {
        string result;
        string buffer;
        int* pairs = new int[(textStr.size() / 2) + 1];

        //розбиваємо числа на пари і додаємо в масив
        int j = 0;
        for (size_t i = 0; i <= textStr.size() - 1; i += 2)
        {
            //якщо знаходимо недопустимий символ
            assert(!((textStr[i] && CodingTable[textStr[i]] == 0 && textStr[i] != '0') || (textStr[i + 1] && CodingTable[textStr[i + 1]] == 0 && textStr[i + 1] != '0')) && "Invalid symbol is found. Try to examine the list of valid symbols");

            if (textStr[i] && textStr[i + 1]) {
                pairs[j] = CodingTable[textStr[i]] * 45 + CodingTable[textStr[i + 1]];
            }
            else {
                pairs[j] = CodingTable[textStr[i]];
            }
            j++;
        }
        //робимо двійковий рядок з цих чисел, якщо в рядку нема заборонених символів
        int number;
        for (size_t i = 0; i < (textStr.size() / 2) + (textStr.size() % 2); i++) {

            number = pairs[i];

            //отримуємо двійкове число
            buffer = DecimalToBinary(number);

            //якщо парна кількість символів
            if (textStr.size() % 2 == 0) {
                while (buffer.size() != 11) {
                    buffer = '0' + buffer;
                }
            }
            //якщо непарна кількість символів, то ми маємо зробити довжину останнього 6, а не 11
            if (textStr.size() % 2 != 0) {
                while ((i == textStr.size() / 2) ? buffer.size() != 6 : buffer.size() != 11) {
                    buffer = '0' + buffer;
                }
            }
            //додаємо до результату значення буфера
            result += buffer;
            buffer = "";
        }
        delete[]pairs;
        //повертаємо результат
        bitStr = result;
    }
    void QR::AddingServiceFields() {
        
        string buffer;
        string result;


        //отримуємо двійкове число
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
        //знаходимо підходящу версію кодування
        switch (GetMode()) {
        case alphanumeric:
            while (textStr.size() /** 8*/ > MaximalAmountOfInfo[correctionLevel][version]) {
                version++;
            }
            break;
        case binary:
            while (bitStr.size() /** 8*/ > MaxAmountOfInfo[correctionLevel][version]) {
                version++;
            }
            break;
        }



        //підгоняємо довжину рядку в двійковому представленні під певне число (для різних версій різне)
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

        //додаємо до способу кодування кількість інформації і саму інформацію
        //тут також
        switch (GetMode()) {
        case alphanumeric:
            buffer = "0010" + buffer;
            break;
        case binary:
            buffer = "0100" + buffer;
            break;
        }

        result = buffer + bitStr;

        bitStr = result;
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

        //доповнюємо для кратності 

        while (bitStr.size() % 8 != 0) {
            bitStr = bitStr + '0';
        }

        //знаходимо кількість завершаючих послідовностей 0 та 1, що мають чередуватись
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

        //заповнюємо блоки байтами інформації. певні блоки мають на 1 байт більше,
        //тому спочатку заповнюємо менші, а після них - більші. наприклад: 36 - 36 - 36 - 37 - 37
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
        //ось тут йде заповнення більших
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
        int A, B, C;//буферні значення для вичислення байтів корекції
        int Poly[30];
        int PolyBuf[30];
        int counter = 2;
        vector<string> block;

        int amountOfCorrectionBytes = AmountOfCorrectionBytesPerBlock[correctionLevel][version];
        //проходимось по масиву
        for (size_t i = 0; i < 13; i++) {
            if (GeneratingPolynomial[i][0] == amountOfCorrectionBytes) {
                while (GeneratingPolynomial[i][counter] != -1 && counter != 32) {
                    Poly[counter - 2] = GeneratingPolynomial[i][counter];
                    counter++;
                }
            }
        }
        counter = counter - 2;
        //створюємо стільки ж блоків корекції, скільки є блоків інформації
        for (size_t i = 0; i < informationBlocks.size(); i++) {

            //створюємо та заповнюємо підготовлений масив
            int maximal = max((int)informationBlocks[i].size(), amountOfCorrectionBytes);
            int* arr = new int[maximal] {0};

            for (size_t j = 0; j < informationBlocks[i].size(); j++) {
                arr[j] = BinaryToDecimal(informationBlocks[i][j]);
            }

            //цикл на кількість байтів інформації даного блоку

            for (size_t j = 0; j < informationBlocks[i].size(); j++) {
                //копіюємо в буферний масив значення оригінального т.я. на кожній ітерації циклу потрібні оригінальні значення масиву,
                //але ми їх змінюємо в процесі виконання циклу
                std::copy(std::begin(Poly), std::end(Poly), std::begin(PolyBuf));

                // перший крок
                A = arr[0];
                for (size_t f = 1; f < maximal; f++) {
                    arr[f - 1] = arr[f];
                }
                arr[maximal - 1] = 0;

                //другий крок
                if (A != 0) {
                    //третій крок
                    B = IGF[A];
                    //умновно четвертий крок
                    for (size_t z = 0; z < amountOfCorrectionBytes; z++) {
                        PolyBuf[z] = ((PolyBuf[z] + B) % 255);
                    }
                    //п'ятий крок
                    for (size_t y = 0; y < amountOfCorrectionBytes; y++) {
                        arr[y] = GF[PolyBuf[y]] ^ arr[y];
                    }
                }

            }

            //отут занесемо коректуючі блоки в масив
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
        }
        // ! нульові байти корекції не заносити потім при об'єднанні блоків}
    }
    void QR::CombiningBlocks() {
        bitStr = "";
        int amountOfBlocks = AmountOfBlocks[correctionLevel][version];
        int numberOfAugmentedBlocks = (MaxAmountOfInfo[correctionLevel][version] / 8) % amountOfBlocks;
        //комбінуємо блоки інформації і блоки корекціїї
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
        //вичисляємо кількість вирівнюючих візерунків і розмір потрібного нам масиву
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

        // малюємо горизонтальні і вертикальні полоски синхронизації
        for (size_t i = 0; i < arr.size; i++) {
            arr.SetFunctionModule(6, i, i % 2 == 0);
            arr.SetFunctionModule(i, 6, i % 2 == 0);
        }

        // малюємо три пошукові візерунки
        arr.DrawSearchPart(3, 3);
        arr.DrawSearchPart(arr.size - 4, 3);
        arr.DrawSearchPart(3, arr.size - 4);

        // малюємо вирівнюючі візерунки
        for (size_t i = 0; i < numAlign; i++) {
            for (size_t j = 0; j < numAlign; j++) {
                // не малюємо їх на місці пошукових візерунків
                if (version > 6) {
                    if (!((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0)))
                        arr.DrawAlignmentPattern(AlignmentPatterns[version - 1][i], AlignmentPatterns[version - 1][j]);
                }
                else {
                    arr.DrawAlignmentPattern(AlignmentPatterns[version - 1][i], AlignmentPatterns[version - 1][j]);
                }
            }
        }

        // малюємо номер версії та код маски + рівня корекції
        arr.DrawVersion(*this);
        arr.DrawFormatBits(*this);

        // вставка інформації на полотно
        size_t i = 0;  // індекс для масиву даних
        // вставку бітів робимо зігзагом
        for (int right = size - 1; right >= 1; right -= 2) {  // індекс правої колонки в кожній парі
            if (right == 6)
                right = 5;
            for (int vert = 0; vert < size; vert++) {  // вертикальний лічильник
                for (int j = 0; j < 2; j++) {
                    size_t x = static_cast<size_t>(right - j);  // Actual x coordinate
                    bool upward = ((right + 1) & 2) == 0;
                    size_t y = static_cast<size_t>(upward ? size - 1 - vert : vert);
                    if (arr.array[y][x] != 3 && arr.array[y][x] != 4 && i < bitStr.size()) {
                        (bitStr[i] == '0') ? arr.array[y][x] = 0 : arr.array[y][x] = 1;
                        i++;
                    }
                }
            }
        }

        //приміняємо маску

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
    encoding_mode QR::GetMode(){
        return mode;
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
    OutPutMatrix::~OutPutMatrix()
    {
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
        //малюємо коди маски та рівня корекції в двох місцях

        //1ий
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

        //2ий
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
        //малюємо пошукові візерунки
        for (int dy = -2; dy <= 2; dy++) {
            for (int dx = -2; dx <= 2; dx++)
                SetFunctionModule(x + dx, y + dy, std::max(std::abs(dx), std::abs(dy)) != 1);
        }
    }
    void OutPutMatrix::DrawSearchPart(int x, int y) {
        // малюємо вирівнюючі візерунки
        for (int dy = -4; dy <= 4; dy++) {
            for (int dx = -4; dx <= 4; dx++) {
                int dist = std::max(std::abs(dx), std::abs(dy));
                int xx = x + dx, yy = y + dy;
                if (0 <= xx && xx < size && 0 <= yy && yy < size)
                    SetFunctionModule(xx, yy, dist != 2 && dist != 4);
            }
        }
    }
    void OutPutMatrix::DrawVersion(QR qr) {
        //малюємо код версії (за умови що версія більша за 7)
        int version = 1;
        switch (qr.GetMode()) {
        case alphanumeric: 
            while (qr.GetInfo().size() /** 8*/ > MaximalAmountOfInfo[qr.GetCorrectionLevel()][version]) {
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
        filename[11] = 'b';
        filename[12] = 'm';
        filename[13] = 'p';
        filename[14] = '\0';

        image.save_bmp(filename);
    }
    
    void Draw(QR qrcode, text_colors t_color, background_colors b_color) {
        assert((t_color == black || t_color == blue || t_color == brown || t_color == green) && "t_color value can only be 'black', 'brown', 'green' or 'blue'");
        assert((b_color == white || b_color == yellow || b_color == orange) && "b_color value can only be 'white', 'yellow' or 'orange'");
        assert(!(qrcode.correctionLevel > 4 || qrcode.correctionLevel < 1) && "Invalid correction level entered. The value must be between 1 and 4");
        assert(!(qrcode.maskCode > 7 || qrcode.maskCode < 0) && "Invalid mask code entered. The value must be between 0 and 7");
        assert(!(qrcode.textStr.size() > MaximalAmountOfInfo[qrcode.correctionLevel][40]) && "Length of string is more than allowed");
        assert(!(qrcode.textStr.size() == 0) && "Length can't be equal to 0");
        switch (qrcode.GetMode()) {
        case alphanumeric:
            qrcode.StrEncoder();
            break;
        case binary:
            qrcode.StrEncodeBit();
            break;
        }

        qrcode.AddingServiceFields();
        qrcode.AddingExtraBits();
        qrcode.DividingToBlocks();
        qrcode.CreatingCorrectionBytes();
        qrcode.CombiningBlocks();
        qrcode.DrawingArray(t_color, b_color);
    }