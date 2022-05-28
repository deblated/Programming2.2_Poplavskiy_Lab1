#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cassert>
#include "QRCodeGen.h"
using namespace std;
using namespace qrcodegen;
int main(){
	//допустимі сиволи - цифри, великі та малі латинські літери,
	//пробіл і такі символи як: $	%	*	+	-	.	/	:
	// t_color: black, green, blue and brown
	// b_color: white, yellow and orange
	// maskCode from 0 to 7 and correctionLevel from 1 to 4
	srand(time(0));
	QRCodeGen QR;

	QR.QRCode("QRCODETEST 123 123=", 4, 2,"blue","white");
	return 0;
}