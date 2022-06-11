﻿#include "pch.h"
#include "CppUnitTest.h"
#include "QRCodeGen.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(QRTestClass)
    {
    public:

        TEST_METHOD(TestCostructor)
        {
            QR qrcode("123321test", alphanumeric, 2, 3);
            Assert::IsTrue(qrcode.GetMaskCode() == 2);
            Assert::IsTrue(qrcode.GetCorrectionLevel() == 3);
            Assert::IsTrue(qrcode.GetMode() == alphanumeric);
            Assert::IsTrue(qrcode.GetInfo() == "123321test");
        }
        TEST_METHOD(TestConstructorWithDefault)
        {
            QR qrcode("123321test", binary);
            Assert::IsTrue(qrcode.GetCorrectionLevel() == 2);
            Assert::IsTrue(qrcode.GetMaskCode() <= 7 && qrcode.GetMaskCode() >= 0);
        }
        TEST_METHOD(TestEncodingAlphanumeric)
        {
            QR qrcode("hello world my dear friend1", alphanumeric);
            qrcode.StrEncoderAlphanumeric();
            Assert::IsTrue(qrcode.bitStr == "01100001011011110001101000101110010110111000100110101000100110110110000000000110011000010101000000010011100011010101111100110011100010000011000000001");
        }
        TEST_METHOD(TestEncodingBinary)
        {
            QR qrcode(u8"すべての人間は、生まれながらにして自由であり、かつ、尊厳.", binary);
            qrcode.StrEncodeBinary();
            Assert::IsTrue(qrcode.bitStr == "11100011100000011001100111100011100000011011100111100011100000011010011011100011100000011010111011100100101110101011101011101001100101101001001111100011100000011010111111100011100000001000000111100111100101001001111111100011100000011011111011100011100000101000110011100011100000011010101011100011100000011000110011100011100000101000100111100011100000011010101111100011100000011001011111100011100000011010011011101000100001111010101011100111100101001011000111100011100000011010011111100011100000011000001011100011100000101000101011100011100000001000000111100011100000011000101111100011100000011010010011100011100000001000000111100101101100001000101011100101100011101011001100101110");
        }

	};

