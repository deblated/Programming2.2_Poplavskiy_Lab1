#include <iostream>
#include "QRCode.h"

/*!
    \brief Main function

	Example

	\code 
	int main(){
	QR qrcode(u8"TEST1", binary);
	qrcode.Draw(qrcode, black, white);
	return 0;
	}
	\endcode

	\warning Writing "u8" before text info is necessary if you want to use binary encoding type
*/

int main(){

	srand(time(0));

	QR qrcode(u8"TEST1", binary);

	qrcode.Draw(qrcode, black, orange);
	qrcode.Draw(qrcode, green, orange);
	qrcode.Draw(qrcode, blue, orange);
	qrcode.Draw(qrcode, brown, orange);

	return 0;
}