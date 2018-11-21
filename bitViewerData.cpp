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
#include <QString>
#include <QStringList>
#include "DataTypes.h"
#include "IOUtilities.h"
#include "dString.h"
#include "bitOperations.h"
#include "AutoDelimiter.h"
#include "bitViewerData.h"

void BitViewerData::generateOutputData(bool b_inputChanged)
{
    int i_inBits = 0;
    int i_inValues = 0;
    int i_outBits = 0;
    int i_outValues = 0;

    UINT_32 i_index;
    UINT_32 i_numInValues;

    if(m_InAscii)
    {
        std::string inText = m_Input.toStdString();
        const char* pc_inText = inText.c_str();
        m_ioDataIn.clear();
        for(i_index = 0; i_index < inText.size(); ++i_index)
        {
            m_ioDataIn.push_back(pc_inText[i_index]);
        }
    }
    else
    {
        QString inText = m_Input;
        if(m_LineEndDelim == true)
        {
           if( m_Delimiter != "\r" &&
               m_Delimiter != "\n" &&
               m_Delimiter != "\r\n" )
           {
               inText = inText.replace("\r\n", m_Delimiter);
               inText = inText.replace("\n", m_Delimiter);
               inText = inText.replace("\r", m_Delimiter);
           }
        }
        if(b_inputChanged)
        {
           inText = QString::fromStdString( removeNonDelimiterChars(
               inText.toStdString(),
               m_Delimiter.toStdString()) );
        }

        QStringList inValues = inText.split(m_Delimiter, QString::SkipEmptyParts);

        // Input
        i_numInValues = inValues.count();
        m_ioDataIn.clear();
        for(i_index = 0; i_index < i_numInValues; ++i_index)
        {
            m_ioDataIn.push_back( asciiToInt(inValues[i_index].toStdString(),
                                             m_InBase,
                                             m_InSigned) );

        }

    }


    ToBitVector(m_bitData, m_ioDataIn, m_InBitsPer);

    m_inNumValues = m_ioDataIn.size();
    m_inNumBits = m_bitData.size();

    i_inValues = m_ioDataIn.size();
    i_inBits = m_bitData.size();


    if(m_InByteRev)
    {
        ByteSwap(m_bitData, m_InBitsPer, m_InSigned);
    }

    if(m_InBitRev)
    {
        BitSwap(m_bitData, m_InBitsPer);
    }
\
    BitShift(m_bitData, m_InBitShift);

    // Middle

    // Output
    BitShift(m_bitData, m_OutBitShift);


    if(m_OutBitRev)
    {
        BitSwap(m_bitData, m_OutBitsPer);
    }

    if(m_OutByteRev)
    {
        ByteSwap(m_bitData, m_OutBitsPer, m_OutSigned);
    }

    // Invert
    if(m_BitInvert)
    {
        bitData::iterator i_bitIter;

        for(i_bitIter = m_bitData.begin(); i_bitIter != m_bitData.end(); ++i_bitIter)
        {
            (*i_bitIter) ^= 1;
        }
    }

    i_outValues = m_ioDataOut.size();
    i_outBits = m_bitData.size();

    m_ioDataOut.clear();
    FromBitVector(m_ioDataOut, m_bitData, m_OutBitsPer, m_OutSigned);

    m_outNumValues = m_ioDataOut.size();
    m_outNumBits = m_bitData.size();

}

void BitViewerData::outputAsciiDataToStr()
{
    int outSize = m_ioDataOut.size();
    char* outText = new char[outSize + 1];
    bool nonNullByteWritten = false;
    outText[outSize] = '\0';
    int i_index = 0;
    for(ioData::iterator outValues = m_ioDataOut.begin(); outValues != m_ioDataOut.end(); ++outValues)
    {
        if(nonNullByteWritten || (*outValues & 0xFF))
        {
            outText[i_index++] = (*outValues & 0xFF);
            nonNullByteWritten = true;
        }
    }
    outText[i_index] = '\0';
    m_outputText = QString::fromStdString(outText);

    delete [] outText;
}
void BitViewerData::outputDataToStr()
{
   static const char LINE_END[] = {'\r', '\n'};
   bool b_useSpaceSep = true; // the space between values
   int i_sepSize = 1;

   int i_numOutValues = m_ioDataOut.size();

   if(i_numOutValues <= 0)
   {
      return;
   }

   INT_UMAX l_bitMask = (INT_UMAX)(((INT_UMAX)1 << (INT_UMAX)m_OutBitsPer) - (INT_UMAX)1);
   UINT_32 maxChars = determineNumChars( l_bitMask, m_OutBase);

   if(m_OutBase == 10 && m_OutSigned)
   {
      maxChars = determineNumChars( (l_bitMask>>1)+1, m_OutBase) + 1;
      l_bitMask = -1;
   }

   int i_charsPerValue = maxChars;
   int i_numReturns = 0;
   int i_numSepChars = 0;
   int i_totalOutputSize = 0;
   int i_cArrayCommaSize = 0; // Default to zero before checking if it is a c array.

   if(i_numOutValues > m_NumRows)
   {
      i_numReturns = ((i_numOutValues - m_NumRows - 1) / m_NumRows) + 1;
   }

   if(m_OutBase == 16 && m_cArray)
   {
      i_charsPerValue += 3; // 2 for 0x and 1 for comma at end
   }
   else if(m_cArray)
   {
      i_charsPerValue += 1; // 1 for comma
   }

   if(b_useSpaceSep)
   {
      i_numSepChars = (i_numOutValues - i_numReturns)*i_sepSize; // No Extra spaces for values that are at the beginning of a line
   }
   else
   {
      i_sepSize = 0;
      i_numSepChars = 0;
   }

   i_totalOutputSize = i_charsPerValue * i_numOutValues + i_numReturns*sizeof(LINE_END) + i_numSepChars;

   char* pc_outText = new char[i_totalOutputSize];

   // Fill in the unused characters.
   if(m_OutBase == 10)
   {
      memset(pc_outText, ' ', i_totalOutputSize);
   }
   else
   {
      memset(pc_outText, '0', i_totalOutputSize);
   }

   int i_lineEndPos = m_NumRows*i_charsPerValue + (m_NumRows - 1)*i_sepSize;
   int i_lineEndIncrement = i_lineEndPos + sizeof(LINE_END);
   // Fill in line ends (assuming 2 chars per line end for now)
   for(int i = 0; i < i_numReturns; ++i)
   {
      pc_outText[i_lineEndPos] = LINE_END[0];
      pc_outText[i_lineEndPos+1] = LINE_END[1];
      i_lineEndPos += i_lineEndIncrement;
   }

   // Fill in the commas for the c array
   if(m_cArray)
   {
      i_cArrayCommaSize = 1;

      int i_lineEndPos = m_NumRows*i_charsPerValue + (m_NumRows - 1)*i_sepSize;
      int i_lineEndIncrement = i_lineEndPos + sizeof(LINE_END);
      int i_charDiffFromEndLineToSep = sizeof(LINE_END) - i_sepSize;
      int i_commaPos = i_charsPerValue - 1;
      int i_commaIncrement = i_charsPerValue + i_sepSize;
      while(i_commaPos < i_totalOutputSize)
      {
         pc_outText[i_commaPos] = ',';
         i_commaPos += i_commaIncrement;
         if(i_commaPos >= i_lineEndPos)
         {
            i_commaPos += i_charDiffFromEndLineToSep;
            i_lineEndPos += i_lineEndIncrement;
         }
      }
   }

   // Fill in the 0x for the c array
   if(m_OutBase == 16 && m_cArray)
   {
      int i_lineEndPos = m_NumRows*i_charsPerValue + (m_NumRows - 1)*i_sepSize;
      int i_lineEndIncrement = i_lineEndPos + sizeof(LINE_END);
      int i_charDiffFromEndLineToSep = sizeof(LINE_END) - i_sepSize;
      int i_0xPos = 0;
      int i_0xIncrement = i_charsPerValue + i_sepSize;
      while(i_0xPos < i_totalOutputSize)
      {
         pc_outText[i_0xPos] = '0';
         pc_outText[i_0xPos+1] = 'x';
         i_0xPos += i_0xIncrement;
         if(i_0xPos >= i_lineEndPos)
         {
            i_0xPos += i_charDiffFromEndLineToSep;
            i_lineEndPos += i_lineEndIncrement;
         }
      }
   }

   // Fill in extra spaces
   if(m_OutBase != 10 && b_useSpaceSep && m_NumRows > 1)
   {
      int i_lineEndPos = m_NumRows*i_charsPerValue + (m_NumRows - 1)*i_sepSize;
      int i_lineEndIncrement = i_lineEndPos + sizeof(LINE_END);
      int i_charDiffFromEndLineToSep = sizeof(LINE_END) - i_sepSize;
      int i_sepPos = i_charsPerValue;
      int i_sepIncrement = i_charsPerValue + i_sepSize;
      while(i_sepPos < i_totalOutputSize)
      {
         pc_outText[i_sepPos] = ' ';
         i_sepPos += i_sepIncrement;
         if(i_sepPos >= i_lineEndPos)
         {
            i_sepPos += (i_sepIncrement + i_charDiffFromEndLineToSep);
            i_lineEndPos += i_lineEndIncrement;
         }
      }
   }

   // Fill in the output data
   {
      int i_lineEndPos = m_NumRows*i_charsPerValue + (m_NumRows - 1)*i_sepSize;
      int i_lineEndIncrement = i_lineEndPos + sizeof(LINE_END);
      int i_charDiffFromEndLineToSep = sizeof(LINE_END) - i_sepSize;
      int i_outValPos = i_charsPerValue - 1 - i_cArrayCommaSize;
      int i_outValIncrement = i_charsPerValue + i_sepSize;
      for(ioData::iterator outValues = m_ioDataOut.begin(); outValues != m_ioDataOut.end(); ++outValues)
      {
         intToAscii_cArray( *outValues & l_bitMask,
                             m_OutBase,
                             m_OutSigned,
                             pc_outText + i_outValPos );

         i_outValPos += i_outValIncrement;
         if(i_outValPos >= i_lineEndPos)
         {
            i_outValPos += i_charDiffFromEndLineToSep;
            i_lineEndPos += i_lineEndIncrement;
         }
      }
   }

   pc_outText[i_totalOutputSize-1] = '\0';
   m_outputText = QString::fromAscii(pc_outText, i_totalOutputSize-1);
   delete[] pc_outText;

}
