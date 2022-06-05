#include "QRCode.h"
#include "Constants.h"
#include "QRArray.h"
#include <cmath>
#include <ctime>
#include <cassert>
#include <bitset>

using std::bitset; using std::to_string; using std::swap; using std::max; using std::abs; 
using std::begin; using std::end; using std::copy; using std::stoi;
    

    /*!
        \brief Converts text to binary string in binary encoding mode
    */

    void QR::StrEncodeBinary(){
        string result = "";
        for (char& _char : textStr) {
            result += bitset<8>(_char).to_string();
        }
        bitStr = result;
    }

    /*!
        \brief Converts decimal number to binary
        \param[in] number - Value to convert
    */

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

    /*!
        \brief Converts binary number to decimal
        \param[in] str - Value to convert
    */

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

    /*!
        \brief Converts text to binary string in alphanumeric encoding mode
    */

    void QR::StrEncoderAlphanumeric() {
        string result;
        string buffer;
        int* pairs = new int[(textStr.size() / 2) + 1];

        //splitting numbers into pairs
        int j = 0;
        for (size_t i = 0; i <= textStr.size() - 1; i += 2){
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

    /*!
    \brief Adds service fields such as: size of the info to encode, version to be used and encoding mode    
    */

    void QR::AddingServiceFields() {
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
        switch (GetMode()) {
        case alphanumeric:
            while (textStr.size() > MaximalAmountOfInfo[correctionLevel][version]) {
                version++;
            }
            break;
        case binary:
            while (bitStr.size() > MaxAmountOfInfo[correctionLevel][version]) {
                version++;
            }
            break;
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

        bitStr = result;
    }

    /*!
    \brief Adds extra bits to make a bit string of the desired length
    */

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

    /*!
    \brief Divides the data received from AddingServiceFields() to blocks
    */

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

    /*!
    \brief Creates blocks with correction bytes
    */

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

    /*!
    \brief Combines blocks to make a bit string of the desired length
    */

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

    /*!
    \brief Puts all information into the matrix
    \param[in] t_color - Text color of the qr code (usually it is black)
    \param[in] b_color - Background color of the qr code (usually it is white)
    */

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

    /*!
    \brief Correction level getter
    */

    int QR::GetCorrectionLevel() {
        return correctionLevel;
    }

    /*!
    \brief Mask code getter
    */

    int QR::GetMaskCode() {
        return maskCode;
    }

    /*!
    \brief Text getter
    */

    string QR::GetInfo() {
        return textStr;
    }

    /*!
    \brief Encoding mode getter
    */

    encoding_mode QR::GetMode(){
        return mode;
    }

    /*!
    \brief Calls all necessary functions to draw qr code
    \param[in] t_color - Text color of the qr code (usually it is black)
    \param[in] b_color - Background color of the qr code (usually it is white)
    */

    void QR::Draw(QR qrcode, text_colors t_color, background_colors b_color) {
        assert((t_color == black || t_color == blue || t_color == brown || t_color == green) && "t_color value can only be 'black', 'brown', 'green' or 'blue'");
        assert((b_color == white || b_color == yellow || b_color == orange) && "b_color value can only be 'white', 'yellow' or 'orange'");
        assert(!(qrcode.correctionLevel > 4 || qrcode.correctionLevel < 1) && "Invalid correction level entered. The value must be between 1 and 4");
        assert(!(qrcode.maskCode > 7 || qrcode.maskCode < 0) && "Invalid mask code entered. The value must be between 0 and 7");
        assert(!(qrcode.textStr.size() > MaximalAmountOfInfo[qrcode.correctionLevel][40]) && "Length of string is more than allowed");
        assert(!(qrcode.textStr.size() == 0) && "Length can't be equal to 0");
        switch (qrcode.GetMode()) {
        case alphanumeric:
            qrcode.StrEncoderAlphanumeric();
            break;
        case binary:
            qrcode.StrEncodeBinary();
            break;
        }

        qrcode.AddingServiceFields();
        qrcode.AddingExtraBits();
        qrcode.DividingToBlocks();
        qrcode.CreatingCorrectionBytes();
        qrcode.CombiningBlocks();
        qrcode.DrawingArray(t_color, b_color);
    }