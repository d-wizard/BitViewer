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
#include <string>
#include <string.h>
#include "DataTypes.h"
#include "IOUtilities.h"

#ifndef COMPILE_FOR_SIZE
static int gai_charToIntLookup[256];
static int gi_intToAsciiBase = -1;

static void initAsciiToIntLookup(int base)
{
   int i_index = 0; // Must be signed
   
   // Set to -1 for all values
   memset(gai_charToIntLookup, 0xFF, sizeof(gai_charToIntLookup));

   // Fill in the numbers
   i_index = 0;
   while(i_index < base && i_index < 10)
   {
      gai_charToIntLookup[i_index + '0'] = i_index;
      ++i_index;
   }

   // Fill in the Letters
   i_index = 0;
   while(i_index < (base-10) && i_index < 26)
   {
      gai_charToIntLookup[i_index + 'A'] = i_index+10;
      gai_charToIntLookup[i_index + 'a'] = i_index+10;
      ++i_index;
   }

}

INT_SMAX asciiToInt(const std::string& asc, int base, bool bSigned)
{
   INT_SMAX retVal = 0;
   int power = 1;
   const unsigned char* cAsc = (const unsigned char*)asc.c_str();
   int i = asc.length() - 1;
   
   if(gi_intToAsciiBase != base)
   {
      initAsciiToIntLookup(base);
      gi_intToAsciiBase = base;
   }
   
   while(i >= 0 && gai_charToIntLookup[cAsc[i]] >= 0)
   {
      retVal += (gai_charToIntLookup[cAsc[i]]*power);
      power *= base;
      --i;
   }
   
   if(bSigned == true && cAsc[i] == '-')
   {
      retVal = -retVal;
   }
   return retVal;
}

#else

INT_SMAX asciiToInt(const std::string& asc, int base, bool bSigned)
{
   INT_SMAX retVal = 0;
   int power = 1;
   const char* cAsc;
   int i = asc.length() - 1;
   int stopValue = 0;
   bool b_neg = false;

   cAsc = asc.c_str();

   while(i >= stopValue)
   {
      if(cAsc[i] >= '0' && cAsc[i] <= '9' && cAsc[i] < ('0' + base))
      {
         retVal += ((cAsc[i] - '0')*power);
      }
      else if( cAsc[i] >= 'A' && cAsc[i] <= ('A' + (base - 11)))
      {
         retVal += ((cAsc[i] - 'A' + 10)*power);
      }
      else if( cAsc[i] >= 'a' && cAsc[i] <= ('a' + (base - 11)))
      {
         retVal += ((cAsc[i] - 'a' + 10)*power);
      }
      else if(bSigned == true && cAsc[i] == '-')
      {
         b_neg = true;
         break;
      }
      else
      {
         break;
      }
      power *= base;
      --i;
   }
   
   
   if(b_neg == true && retVal != 0)
   {
      retVal = -retVal;
   }

   return retVal;
}

#endif



std::string intToAscii(INT_SMAX num, int base, int numBits, bool isSigned)
{
    int i;
    int numChars;
    std::string retVal;
    bool addNeg = false;

    if( (isSigned == true) && (base == 10))
    {
        if(numBits <= 8)
        {
            char temp;
            temp = (char)num;
            if(temp < 0)
            {
                addNeg = true;
                num = -temp;
            }
        }
        else if(numBits <= 16)
        {
            short temp;
            temp = (short)num;
            if(temp < 0)
            {
                addNeg = true;
                num = -temp;
            }
        }
        else if(numBits <= 32)
        {
            int temp;
            temp = (int)num;
            if(temp < 0)
            {
                addNeg = true;
                num = -temp;
            }
        }
        else
        {
            INT_SMAX temp;
            temp = (INT_SMAX)num;
            if(temp < 0)
            {
                addNeg = true;
                num = -temp;
            }
        }
    }

    numChars = determineNumChars(num, base);
    for(i = 0; i < numChars; ++i)
    {
        retVal.insert(0, numToStr(num % base));
        num /= base;
    }
    if(addNeg == true)
        retVal.insert(0,"-");
    return retVal;
}

int intToAscii_cArray(INT_UMAX num, int base, bool isSigned, char* leastSignificantPos)
{
   int numChars;
   
   if( (isSigned == true) && (base == 10) && (num & MAX_INT_NEGATIVE_MASK) )
   {
      INT_SMAX absNum;
      memcpy(&absNum, &num, sizeof(INT_SMAX));
      absNum = -absNum;

      numChars = determineNumChars(absNum, base);
      for(int i = 0; i < numChars; ++i)
      {
         *leastSignificantPos = numToChar(absNum % base);
         --leastSignificantPos;
         absNum /= base;
      }
      *leastSignificantPos = '-';
   }
   else
   {
      numChars = determineNumChars(num, base);
      for(int i = 0; i < numChars; ++i)
      {
         *leastSignificantPos = numToChar(num % base);
         --leastSignificantPos;
         num /= base;
      }
   }
   
   return numChars;
}

int determineNumChars(INT_UMAX value, int base)
{
    int i = 1;
    INT_UMAX divisor = base;
    INT_UMAX prevDivisor = 0;

    if(divisor > 0)
    {
        while( (value >= divisor) && (prevDivisor < divisor) )
        {
            prevDivisor = divisor;
            divisor *= base;
            ++i;
        }
    }
    return i;
}

std::string numToStr(INT_UMAX num)
{
    std::string retVal;
    if(num < 10)
    {
        retVal = num + '0';
    }
    else
    {
        retVal = num - 10 + 'A';
    }
    return retVal;
}

char numToChar(int num)
{
    char retVal;
    if(num < 10)
    {
        retVal = num + '0';
    }
    else
    {
        retVal = num - 10 + 'A';
    }
    return retVal;
}

INT_UMAX fillInMSBs(INT_UMAX value, UINT_32 i_numBitsToNotFillIn, BIT bitValue)
{
    INT_UMAX retVal = value;
    INT_32 i_maxBits = sizeof(INT_UMAX) * BITS_PER_BYTE;
    INT_32 i_bitIndex = i_maxBits - 1;
    INT_32 i_numBitsToFillIn = i_maxBits - i_numBitsToNotFillIn;
    INT_32 i_index = 0;


    if(bitValue == 0)
    {
        for(i_index = 0; i_index < i_numBitsToFillIn; ++i_index)
        {
            retVal = retVal & (~i_bitIndex);
            i_bitIndex >>= 1;
        }
    }
    else
    {
        for(i_index = 0; i_index < i_numBitsToFillIn; ++i_index)
        {
            retVal = retVal | i_bitIndex;
            i_bitIndex >>= 1;
        }
    }
    return retVal;
}
