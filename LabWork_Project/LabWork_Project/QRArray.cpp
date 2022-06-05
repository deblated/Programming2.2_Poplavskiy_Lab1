#include "QRArray.h"
#include "Constants.h"
#include "CImg.h"
using namespace cimg_library;
using std::max;

/*!
    \brief OutPutMatrix constructor that creates dynamic 2d array (matrix)
    \param[in] size - Sets the matrix size
*/

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

/*!
    \brief OutPutMatrix Destructor that frees allocated memory
*/

OutPutMatrix::~OutPutMatrix(){
    for (int i = 0; i < size; i++) {
        delete[] array[i];
    }
    delete[] array;
}

/*!
    \brief Sets matrix modules as functional
    \param[in] x - The x-coordinate of the bit in the matrix
    \param[in] y - The y-coordinate of the bit in the matrix
    \param[in] isDark - Bit color boolean (black or white)

    There ara 4 types of modules:
    0 - white non-functional
    1 - black non-functional
    3 - black functional
    4 - white functional
*/

void OutPutMatrix::SetFunctionModule(int x, int y, bool isDark) {
    if (isDark) {
        array[y][x] = 3;
    }
    else {
        array[y][x] = 4;
    }
}

/*!
    \brief Draws a correction level and a mask code in 2 places of the qr code 
    \param[in] qr - qr code from which we will take the information
*/

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

/*!
    \brief Draws an alignment pattern in the qr code 
    \param[in] x - Center of the alignment pattern by x-coordinate
    \param[in] y - Center of the alignment pattern by y-coordinate
*/

void OutPutMatrix::DrawAlignmentPattern(int x, int y) {
    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++)
            SetFunctionModule(x + dx, y + dy, max(abs(dx), abs(dy)) != 1);
    }
}

/*!
    \brief Draws a search pattern in the qr code 
    \param[in] x - Center of the search pattern by x-coordinate
    \param[in] y - Center of the search pattern by y-coordinate
*/

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

/*!
    \brief Draws a version of the QR code 
    \param[in] qr - QR code from which we will take the information
*/

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

/*!
    \brief Converts matrix to .jpg file
    \param[in] t_color - Text color of the qr code (usually it is black)
    \param[in] b_color - Background color of the qr code (usually it is white)
*/

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
