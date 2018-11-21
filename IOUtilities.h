/* Copyright 2012 - 2018 Dan Williams. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef IOUTILITIES_H
#define IOUTILITIES_H

INT_SMAX asciiToInt(const std::string& asc, int base, bool bSigned);
std::string intToAscii(INT_SMAX num, int base, int numBits, bool isSigned);
int intToAscii_cArray(INT_UMAX num, int base, bool isSigned, char* leastSignificantPos);
int determineNumChars(INT_UMAX value, int base);
std::string numToStr(INT_UMAX num);
char numToChar(int num);

INT_UMAX fillInMSBs(INT_UMAX value, UINT_32 i_numBitsToNotFillIn, BIT bitValue);

#endif // IOUTILITIES_H
