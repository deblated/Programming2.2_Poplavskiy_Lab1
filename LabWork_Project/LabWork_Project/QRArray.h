#pragma once

#include"QRCode.h"

class QR;

///Additional Class in which we put the data
class OutPutMatrix {
private:

    ///QR class uses private fields and methods of OutPutMatrix class
    friend class QR;

    ///Size of the matrix
    size_t size;
    ///2D array (matrix)
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