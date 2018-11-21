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
#ifndef bitStreamCompare_h
#define bitStreamCompare_h

class bitStreamCompare
{
public:
    bitStreamCompare(BitViewerData dstBitData, BitViewerData srcBitData);

    BitViewerData* getBitViewerData(){return &m_bitViewerData;}

    UINT_32 getMatchCount(){return m_matchCount;}
    UINT_32 getMatchErrors(){return m_matchErrors;}
    UINT_32 getNumSrcBits(){return m_numSrcBits;}

private:
    bitStreamCompare();

    void findMaxMatch(BitViewerData& dst, BitViewerData& src);
    void compareBitStreams(bitData& dst, bitData& src);
    bool checkForPerfectMatch();

    BitViewerData m_bitViewerData;

    UINT_32 m_numSrcBits;
    INT_32 m_matchBitShift;
    bool m_matchInverted;
    UINT_32 m_matchCount;
    UINT_32 m_matchErrors;
    bool m_matchInSigned;
    bool m_matchInBitRev;
    bool m_matchInByteRev;
    bool m_matchOutSigned;
    bool m_matchOutBitRev;
    bool m_matchOutByteRev;

};

#endif

