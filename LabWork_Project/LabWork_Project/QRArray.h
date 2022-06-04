#pragma once

#include"QRCode.h"

class QR;

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
    void DrawSearchPart(int x, int y);
    void DrawVersion(QR qr);
    void ToImg(text_colors t_color, background_colors b_color);
};