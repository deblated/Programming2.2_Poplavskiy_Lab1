#include <iostream>
#include "QRCodeGen.h"

int main(){
	// encoding mode: alphanumeric and binary
	// text color: black, green, blue and brown
	// background color: white, yellow and orange
	// maskCode from 0 to 7 and correctionLevel from 1 to 4

	srand(time(0));

	QR qrcode(u8"TEST1", binary);

	Draw(qrcode, black, orange);
	Draw(qrcode, green, orange);
	Draw(qrcode, blue, orange);
	Draw(qrcode, brown, orange);

	return 0;
}