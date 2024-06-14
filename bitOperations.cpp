/* Copyright 2012 - 2018, 2024 Dan Williams. All Rights Reserved.
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
#include "DataTypes.h"
#include "bitOperations.h"


void ToBitVector(bitData& t_dest, ioData& t_src, UINT_32 i_bitsPer)
{
    ioData::iterator srcItr;
    INT_UMAX l_srcData = 0;

    t_dest.clear();

    for(srcItr = t_src.begin(); srcItr != t_src.end(); srcItr++)
    {
        l_srcData = *srcItr;
        for(UINT_32 i_bitIndex = 0; i_bitIndex < i_bitsPer; ++i_bitIndex)
        {
            t_dest.push_back(TO_BIT(l_srcData));
            l_srcData >>= 1;
        }
    }
}

void FromBitVector(ioData& t_dest, bitData& t_src, UINT_32 i_bitsPer, bool b_signed)
{
    bitData::iterator srcItr;
    ioData::iterator destItr;
    UINT_32 i_bitIndex = 0;
    UINT_32 i_outIndex = 0;
    const INT_UMAX signedMask = ((((INT_UMAX)1 << ((sizeof(INT_UMAX)*BITS_PER_BYTE) - i_bitsPer)) - 1) << i_bitsPer); // This is used to fill MSBs to 1s for non-standard 'i_bitsPer' values (... I think)
    const INT_UMAX destMask = (i_bitsPer >= 64) ? (INT_UMAX)-1 : ((INT_UMAX)1 << (i_bitsPer - 1));
    const INT_UMAX ONE_UMAX = 1;

    t_dest.clear();

    for(srcItr = t_src.begin(); srcItr != t_src.end(); srcItr++)
    {
        if(i_bitIndex == 0)
        {
            t_dest.push_back(0);
            if(i_outIndex++ == 0)
            {
                destItr = t_dest.begin();
            }
            else
            {
                ++destItr;
            }
        }
        if(*srcItr)
        {
            *destItr |= (ONE_UMAX << i_bitIndex);
        }
        else
        {
            *destItr &= ~(ONE_UMAX << i_bitIndex);
        }
        if(++i_bitIndex == i_bitsPer)
        {
            i_bitIndex = 0;
            if( b_signed && (*destItr & destMask) )
            {
                *destItr |= signedMask;
            }
        }
    }
}

UINT_32 FillInToByteBoundary(bitData& t_dest, UINT_32 i_bitsPer, bool b_signed)
{
    UINT_32 i_newBitsPer = (i_bitsPer + BITS_PER_BYTE_MASK) & 0xFFFFFFF8;
    bitData::iterator itr = t_dest.begin();

    UINT_32 i_numZerosToInsert = i_newBitsPer - i_bitsPer;
    UINT_32 i_numReadBits = i_bitsPer;
    UINT_32 i_numRemainingBits = t_dest.size();

    BIT fillInData = 0;

    UINT_32 i_index = 0;

    if(i_newBitsPer != i_bitsPer)
    {
        while(i_numRemainingBits > 0)
        {
            if( i_numRemainingBits >= i_bitsPer)
            {
                i_numRemainingBits -= i_bitsPer;
            }
            else
            {
                i_numZerosToInsert = i_newBitsPer - i_numRemainingBits;
                i_numReadBits = i_numRemainingBits;
                i_numRemainingBits = 0;
            }
            for(i_index = 0; i_index < (i_numReadBits-1); ++i_index)
            {
                ++itr;
            }
            if(*itr && b_signed)
            {
                fillInData = 1;
            }
            else
            {
                fillInData = 0;
            }
            ++itr;
            for(i_index = 0; i_index < i_numZerosToInsert; ++i_index)
            {
                t_dest.insert(itr, fillInData);
            }
        }
    }
    else if( (t_dest.size() & BITS_PER_BYTE_MASK) != 0)
    {
        itr = t_dest.end();
        itr--;
        if(*itr && b_signed)
        {
            fillInData = 1;
        }
        else
        {
            fillInData = 0;
        }
        ++itr;
        i_numZerosToInsert = BITS_PER_BYTE - (t_dest.size() & BITS_PER_BYTE_MASK);
        for(i_index = 0; i_index < i_numZerosToInsert; ++i_index)
        {
            t_dest.insert(itr, fillInData);
        }

    }

    return i_newBitsPer;
}


UINT_32 ByteSwap(bitData& bitData, UINT_32 i_bitsPer, bool b_signed)
{
    i_bitsPer = FillInToByteBoundary(bitData, i_bitsPer, b_signed);
    UINT_32 i_bytesPer = (i_bitsPer + BITS_PER_BYTE_MASK) / BITS_PER_BYTE; //Round Up
    UINT_32 i_numBytesToSwapPer = i_bytesPer >> 1; // Round Down
    UINT_32 i_numUnits = bitData.size() / i_bitsPer;

    UINT_32 i_indexSwaps = 0;
    UINT_32 i_indexByte = 0;
    UINT_32 i_indexBit = 0;

    BIT swapBit = 0;
    UINT_32 i_swapBitIndex1 = 0;
    UINT_32 i_swapBitIndex2 = 0;

    bitData::iterator* itrArray = new bitData::iterator[i_bitsPer];
    bitData::iterator tempItr = bitData.begin();
    for(i_indexSwaps = 0; i_indexSwaps < i_numUnits; ++ i_indexSwaps)
    {
        for(i_indexBit = 0; i_indexBit < i_bitsPer; ++i_indexBit)
        {
            itrArray[i_indexBit] = tempItr;
            tempItr++;
        }
        for(i_indexByte = 0; i_indexByte < i_numBytesToSwapPer; ++i_indexByte)
        {
            i_swapBitIndex1 = i_indexByte * BITS_PER_BYTE;
            i_swapBitIndex2 = i_bitsPer - (1+i_indexByte)*BITS_PER_BYTE;
            for(i_indexBit = 0; i_indexBit < BITS_PER_BYTE; ++i_indexBit)
            {
                swapBit = *itrArray[i_swapBitIndex1];
                *itrArray[i_swapBitIndex1] = *itrArray[i_swapBitIndex2];
                *itrArray[i_swapBitIndex2] = swapBit;
                ++i_swapBitIndex1;
                ++i_swapBitIndex2;
            }
        }
    }
    delete [] itrArray;
    return i_bitsPer;
}

void BitSwap(bitData& bitData, UINT_32 i_bitsPer)
{
    UINT_32 i_numUnits = bitData.size() / i_bitsPer;
    UINT_32 i_indexBit = 0;
    UINT_32 i_indexSwaps = 0;

    BIT swapBit = 0;
    UINT_32 i_swapBitIndex1 = 0;
    UINT_32 i_swapBitIndex2 = 0;

    bitData::iterator* itrArray = new bitData::iterator[i_bitsPer];
    bitData::iterator tempItr = bitData.begin();
    for(i_indexSwaps = 0; i_indexSwaps < i_numUnits; ++ i_indexSwaps)
    {
        for(i_indexBit = 0; i_indexBit < i_bitsPer; ++i_indexBit)
        {
            itrArray[i_indexBit] = tempItr;
            tempItr++;
        }
        for(i_indexBit = 0; i_indexBit < (i_bitsPer>>1); ++i_indexBit)
        {
            i_swapBitIndex1 = i_indexBit;
            i_swapBitIndex2 = i_bitsPer - (1+i_indexBit);
            swapBit = *itrArray[i_swapBitIndex1];
            *itrArray[i_swapBitIndex1] = *itrArray[i_swapBitIndex2];
            *itrArray[i_swapBitIndex2] = swapBit;
        }
    }
    delete [] itrArray;
}

void BitShift(bitData& bitData, INT_32 i_shiftAmount)
{
    INT_32 i_index = 0;
    if(i_shiftAmount > 0)
    {
        for(i_index = 0; i_index < i_shiftAmount; i_index++)
        {
            bitData.push_front(0);
        }
    }
    else if(i_shiftAmount < 0)
    {
        i_shiftAmount = -i_shiftAmount;
        for(i_index = 0; i_index < i_shiftAmount; i_index++)
        {
            bitData.pop_front();
        }
    }
}

