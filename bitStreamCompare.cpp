/* Copyright 2012 - 2018, 2021 Dan Williams. All Rights Reserved.
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
#include "bitStreamCompare.h"

// To check for bit sync between 2 tabs, we toggle some boolean operators
// i.e. the check boxes in the gui. We are trying all the different combinations
// of boolean operators to find bit sync between two differnt tabs. Incrementing
// an unsigned int is used to try all combinations of booleans, each bit in the
// unsigned int represents an individual boolean.
class boolOperatorToggle
{
public:
    boolOperatorToggle(BitViewerData& dst, BitViewerData& src):
        m_curState(0),
        m_prevState(0),
        m_inBitRevValid(true),
        m_outBitRevValid(true)
    {
        // Determine if it is possible to toggle the byte reverse operation
        if( (dst.m_InBitsPer & 7) || (dst.m_InBitsPer <= 8) )
        {
            // The bits per is NOT base 16, 24, ...
            // Cannot check against byte reverse.
            m_inBitRevValid = false;
        }
        // Determine if it is possible to toggle the byte reverse operation
        if( (src.m_OutBitsPer & 7) || (src.m_OutBitsPer <= 8) )
        {
            // The bits per is NOT base 16, 24, ...
            // Cannot check against byte reverse.
            m_outBitRevValid = false;
        }
    }

    bool toggleOperations(BitViewerData& dst, BitViewerData& src)
    {
        ++m_curState;
        while( ((m_inBitRevValid == false) &&  (m_curState & m_inByteRevMask)) ||
               ((m_outBitRevValid == false) &&  (m_curState & m_outByteRevMask)) )
        {
            ++m_curState;
        }

        if(m_curState >= m_invalid)
        {
            m_curState = 0;
            // All combinations of operations have been ran through.
            // Calling setBitViewerValues will reset the values back to their original values.
            setBitViewerValues(dst, src, false);
        }
        else
        {
            setBitViewerValues(dst, src, true);
        }
        return m_curState == 0;
    }

    void setBitViewerValues(BitViewerData& dst, BitViewerData& src, bool genOutput)
    {
        bool dstChanged = setBoolState(dst.m_InBitRev, m_inBitRevMask);
        dstChanged      = setBoolState(dst.m_InSigned, m_inSignedMask)   || dstChanged;
        dstChanged      = setBoolState(dst.m_InByteRev, m_inByteRevMask) || dstChanged;

        bool srcChanged = setBoolState(src.m_OutBitRev, m_outBitRevMask);
        srcChanged      = setBoolState(src.m_OutSigned, m_outSignedMask)   || srcChanged;
        srcChanged      = setBoolState(src.m_OutByteRev, m_outByteRevMask) || srcChanged;

        if(dstChanged == true && genOutput == true)
        {
            dst.generateOutputData(false);
        }
        if(srcChanged == true && genOutput == true)
        {
            src.generateOutputData(false);
        }

        m_prevState = m_curState;
    }

    bool setBoolState(bool& inOutBool, unsigned int mask)
    {
        bool changed = false;
        // if bit in cur state has changed, toggle the inOutBool
        if((m_curState ^ m_prevState) & mask)
        {
            inOutBool = !inOutBool;
            changed = true;
        }
        return changed;
    }

private:
    boolOperatorToggle();

    unsigned int m_curState;
    unsigned int m_prevState;
    bool m_inBitRevValid;
    bool m_outBitRevValid;

    static const unsigned int m_inBitRevMask = 0x00000001;
    static const unsigned int m_inSignedMask = 0x00000002;
    static const unsigned int m_inByteRevMask = 0x00000004;

    static const unsigned int m_outBitRevMask = 0x00000008;
    static const unsigned int m_outSignedMask = 0x00000010;
    static const unsigned int m_outByteRevMask = 0x00000020;

    static const unsigned int m_invalid = 0x00000040;
};



bitStreamCompare::bitStreamCompare(BitViewerData dstBitData, BitViewerData srcBitData):
    m_numSrcBits(0),
    m_matchBitShift(0),
    m_matchInverted(false),
    m_matchCount(0),
    m_matchErrors(0),
    m_matchInSigned(false),
    m_matchInBitRev(false),
    m_matchInByteRev(false),
    m_matchOutSigned(false),
    m_matchOutBitRev(false),
    m_matchOutByteRev(false)

{
    m_numSrcBits = 0;

    m_bitViewerData = dstBitData;

    // Set all output values of the dest to the src,
    // so only the input changes will affect the compare.
    m_bitViewerData.m_OutAscii    = srcBitData.m_OutAscii;
    m_bitViewerData.m_OutBase64   = srcBitData.m_OutBase64;
    m_bitViewerData.m_OutBase     = srcBitData.m_OutBase;
    m_bitViewerData.m_OutBitsPer  = srcBitData.m_OutBitsPer;
    m_bitViewerData.m_OutSigned   = false; // This value will be toggled in srcBitData. Set to false.
    m_bitViewerData.m_OutByteRev  = false; // This value will be toggled in srcBitData. Set to false.
    m_bitViewerData.m_OutBitRev   = false; // This value will be toggled in srcBitData. Set to false.
    m_bitViewerData.m_OutBitShift = srcBitData.m_OutBitShift;

    // Make sure all the input data is used in the compare.
    m_bitViewerData.m_InBitShift = 0;

    findMaxMatch(m_bitViewerData, srcBitData);
    m_bitViewerData.m_InBitShift = m_matchBitShift;
    m_bitViewerData.m_BitInvert = m_matchInverted;
    m_bitViewerData.m_InSigned = m_matchInSigned;
    m_bitViewerData.m_InBitRev = m_matchInBitRev;
    m_bitViewerData.m_InByteRev = m_matchInByteRev;
    m_bitViewerData.m_OutSigned = m_matchOutSigned;
    m_bitViewerData.m_OutBitRev = m_matchOutBitRev;
    m_bitViewerData.m_OutByteRev = m_matchOutByteRev;

    m_bitViewerData.m_cArray = srcBitData.m_cArray;
    m_bitViewerData.m_NumRows = srcBitData.m_NumRows;
}

void bitStreamCompare::findMaxMatch(BitViewerData& dst, BitViewerData& src)
{
    UINT_32 i_prevMatchCount = 0;
    UINT_32 i_timesCheckSource = 1; // Assuming source is not ascii and no need to check for differnt line endings.
    QString possibleLineEndings[] = {"\n", "\r\n", "\r"};

    dst.m_BitInvert = false;

    if(src.m_InAscii)
    {
        i_timesCheckSource = 3;

        // Make sure the line endings are the first member of possibleLineEndings
        src.m_Input = src.m_Input.replace("\r\n", possibleLineEndings[0]);
        src.m_Input = src.m_Input.replace("\r", possibleLineEndings[0]);
        src.m_Input = src.m_Input.replace("\n", possibleLineEndings[0]);
    }

    // Make sure the first time dst regenerates everything.
    dst.generateOutputData(true);

    // starting the change source loop. Source will be changed to look for different line endings.
    for(UINT_32 m = 0; m < i_timesCheckSource && checkForPerfectMatch() == false; ++m)
    {
        src.generateOutputData(true);

        // src and dst should have called generateOutputData by now.
        boolOperatorToggle dstToggler(dst, src);

        do
        {
            i_prevMatchCount = m_matchCount;
            compareBitStreams(dst.m_bitData, src.m_bitData);

            if(m_matchCount > i_prevMatchCount)
            {
                m_matchInSigned = dst.m_InSigned;
                m_matchInBitRev = dst.m_InBitRev;
                m_matchInByteRev = dst.m_InByteRev;
                m_matchOutSigned = src.m_OutSigned;
                m_matchOutBitRev = src.m_OutBitRev;
                m_matchOutByteRev = src.m_OutByteRev;
                m_numSrcBits = src.m_outNumBits;
            }

        }while( checkForPerfectMatch() == false && // If perfect match is found, no need to continue
                dstToggler.toggleOperations(dst, src) == false); // if have toggled bools all the way back to the initial start point, no need to continue

        // If not last time through, replace input line endings with new line ending.
        if(m < (i_timesCheckSource - 1) && checkForPerfectMatch() == false)
        {
            // Not last time through
            src.m_Input = src.m_Input.replace(possibleLineEndings[m], possibleLineEndings[m+1]);

            // Regen dst output data
            dst.generateOutputData(false);
        }
    }

}

void bitStreamCompare::compareBitStreams(bitData& dst, bitData& src)
{
    bitData::iterator dstIterInit = --dst.end();
    bitData::iterator srcIterInit = src.begin();
    bitData::iterator dstIter;
    bitData::iterator srcIter;

    bool b_finished = false;
    bool b_resetIters = false;

    INT_32 i_curBitShift =  1 - (INT_32)dst.size();
    UINT_32 gi_curMatch[2] = {0,0};

    dstIter = dstIterInit;
    srcIter = srcIterInit;

    while(b_finished == false)
    {
        ++gi_curMatch[TO_BIT( ((*dstIter) ^ (*srcIter)) )];
        if(++dstIter == dst.end() || ++srcIter == src.end())
        {
            b_resetIters = true;
            if(dstIterInit == dst.begin())
            {
                if(++srcIterInit == src.end())
                {
                    b_finished = true;
                }
            }
            else
            {
                --dstIterInit;
            }
        }
        if(b_resetIters)
        {
            b_resetIters = false;
            dstIter = dstIterInit;
            srcIter = srcIterInit;
            if(gi_curMatch[0] > m_matchCount)
            {
                m_matchCount = gi_curMatch[0];
                m_matchErrors = gi_curMatch[1];
                m_matchInverted = false;
                m_matchBitShift = i_curBitShift;
            }
            if(gi_curMatch[1] > m_matchCount)
            {
                m_matchCount = gi_curMatch[1];
                m_matchErrors = gi_curMatch[0];
                m_matchInverted = true;
                m_matchBitShift = i_curBitShift;
            }
            gi_curMatch[0] = 0;
            gi_curMatch[1] = 0;
            ++i_curBitShift;

            if(checkForPerfectMatch() == true)
            {
                b_finished = true;
            }
        }
    }

}

bool bitStreamCompare::checkForPerfectMatch()
{
    if(m_matchCount == m_numSrcBits && m_matchErrors == 0 && m_numSrcBits != 0)
    {
        // Perfect match, can do no better.
        return true;
    }
    else
    {
        return false;
    }
}



