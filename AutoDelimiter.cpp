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
#include "hist.h"
#include "dString.h"

typedef DataHistogram<std::string> StringHistogram;
static const int VALID_INPUT_CHAR[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

std::string getDelimiter(const std::string& t_dataToParse)
{
   StringHistogram mt_delimitHist;
   StringHistogram mt_delimitHistOneChar;
   const char* pc_data = t_dataToParse.c_str();
   char c_curChar = '\0';
   char c_prevChar = '\0';
   std::string t_curDelimit = "";
   int i_numChr = t_dataToParse.length();
   int i;
   int numBytesInDelimiter = 0;

   for(i = 0; i < i_numChr; ++i)
   {
      c_curChar = pc_data[i];
      if( ( (c_curChar >= '0') && (c_curChar <= '9') ) ||
          ( (c_curChar >= 'a') && (c_curChar <= 'z') ) ||
          ( (c_curChar >= 'A') && (c_curChar <= 'Z') ) )
      {
         if(numBytesInDelimiter)
         {
            if(numBytesInDelimiter > 1 && c_prevChar == '-')
            {
               t_curDelimit = dString::Left(t_curDelimit, numBytesInDelimiter - 1);
            }
            mt_delimitHist.addNewEntry(t_curDelimit);
            mt_delimitHistOneChar.addNewEntry(dString::Left(t_curDelimit, 1));
            numBytesInDelimiter = 0;
            t_curDelimit = "";
         }
      }
      else
      {
         char ac_curChar[2] = {c_curChar, '\0'};
         t_curDelimit.append(ac_curChar);
         ++numBytesInDelimiter;
         c_prevChar = c_curChar;
      }
   }

   if(numBytesInDelimiter)
   {
      mt_delimitHist.addNewEntry(t_curDelimit);
      mt_delimitHistOneChar.addNewEntry(dString::Left(t_curDelimit, 1));
      numBytesInDelimiter = 0;
      t_curDelimit = "";
   }
   
   if(mt_delimitHistOneChar.getMaxEntryValue() == dString::Left(mt_delimitHist.getMaxEntryValue(), 1))
   {
      return mt_delimitHistOneChar.getMaxEntryValue();
   }
   else
   {
      return mt_delimitHist.getMaxEntryValue();
   }

    return "";
}

std::string removeNonDelimiterChars(const std::string& t_dataToParse, const std::string& t_delimit)
{
   std::string retVal = t_dataToParse;
   int writeIndex = 0;
   
   const char* pc_data = t_dataToParse.c_str();
   int i_numChr = t_dataToParse.length();
   const char* pc_delim = t_delimit.c_str();
   int i_numDelimChr = t_delimit.length();

   if(i_numDelimChr > 1)
   {
      for(int i = 0; i <= (i_numChr-i_numDelimChr); ++i)
      {
         unsigned char c_curChar = pc_data[i];
         bool isDelim = (memcmp(pc_data+i, pc_delim, i_numDelimChr) == 0);

         if(isDelim)
         {
            for(int j = 0; j < i_numDelimChr; ++j)
            {
               retVal[writeIndex++] = pc_data[i+j];
            }
            i += (i_numDelimChr-1); // Skip past all other delimiter characters.
         }
         else if( VALID_INPUT_CHAR[c_curChar] )
         {
            retVal[writeIndex++] = c_curChar;
         }
      }
   }
   else if(i_numDelimChr == 1)
   {
      for(int i = 0; i <= (i_numChr-i_numDelimChr); ++i)
      {
         unsigned char c_curChar = pc_data[i];
         bool isDelim = (pc_data[i] == pc_delim[0]);

         if( VALID_INPUT_CHAR[c_curChar] || isDelim )
         {
            retVal[writeIndex++] = c_curChar;
         }
      }
   }
   else if(i_numDelimChr == 0)
   {
      for(int i = 0; i <= (i_numChr-i_numDelimChr); ++i)
      {
         unsigned char c_curChar = pc_data[i];
         if( VALID_INPUT_CHAR[c_curChar] )
         {
            retVal[writeIndex++] = c_curChar;
         }
      }
   }

   // Write any left over characters.
   for(int i = (i_numChr-i_numDelimChr+1); i < i_numChr; ++i)
   {
      unsigned char c_curChar = pc_data[i];
      if( VALID_INPUT_CHAR[c_curChar] )
      {
         retVal[writeIndex++] = c_curChar;
      }
   }


   retVal.resize(writeIndex);
   return retVal;
}


