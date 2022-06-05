#pragma once
#include <string>
#include <vector>

using std::string;  using std::vector; 

///List of text colors in qr code
enum text_colors{ black, green, blue, brown };
///List of background colors in qr code
enum background_colors { white, yellow, orange };
///List of modes of encoding
enum encoding_mode { alphanumeric, binary };

class OutPutMatrix;

///Main Class with the information about qr code
class QR {
    private:
        ///Text to encode
        string textStr;
        ///Bit string
        string bitStr;
        ///Mask code
        int maskCode = -1;
        ///Version
        int version = 1;
        ///Correction level
        int correctionLevel = -1;
        ///Mode of encoding
        encoding_mode mode;
        ///Blocks with information
        vector<vector<string>> informationBlocks;
        ///Blocks with correction data
        vector<vector<string>> correctionBlocks;
        
        void StrEncodeBinary();
        string DecimalToBinary(int number);
        int BinaryToDecimal(string str);
        void StrEncoderAlphanumeric();
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

        /*!
            \brief QR constructor 
            \param[in] textStr - Info to encode
            \param[in] mode - Mode of encoding
            \param[in] maskCode - code of mask (from 0 to 7)
            \param[in] correctionLevel - correction level (from 1 to 4)

            Constructor creates empty qr code with info to encode, mode of encoding, mask code (random by default) and correction level(2 by default)
        */
        QR(string textStr, encoding_mode mode, int maskCode = rand() % 8, int correctionLevel = 2) : textStr(textStr), maskCode(maskCode), correctionLevel(correctionLevel), mode(mode)
        {}
    };