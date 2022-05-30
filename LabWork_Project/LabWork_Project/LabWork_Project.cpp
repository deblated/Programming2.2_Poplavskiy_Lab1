#include <iostream>

#include "QRCodeGen.h"

int main(){
	//допустимі сиволи - цифри, великі та малі латинські літери,
	//пробіл і такі символи як: $	%	*	+	-	.	/	:
	// t_color: black, green, blue and brown
	// b_color: white, yellow and orange
	// maskCode from 0 to 7 and correctionLevel from 1 to 4
	srand(time(0));
	QR qrcode("This is my rewrited version of qrcode function1");
	Draw(qrcode,"black","white");
	Draw(qrcode, "green", "yellow");
	return 0;
}