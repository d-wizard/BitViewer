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
#ifndef bitViewerData_h
#define bitViewerData_h

class BitViewerData
{
public:
    BitViewerData():
        m_Input         (""),
        m_Delimiter     (""),
        m_InBase        (10),
        m_InBitsPer     (8),
        m_InBitShift    (0),
        m_OutBase       (10),
        m_OutBitsPer    (8),
        m_OutBitShift   (0),
        m_NumRows       (0),
        m_InBase64      (false),
        m_InAscii       (false),
        m_InSigned      (false),
        m_InByteRev     (false),
        m_InBitRev      (false),
        m_OutBase64     (false),
        m_OutAscii      (false),
        m_OutSigned     (false),
        m_OutByteRev    (false),
        m_OutBitRev     (false),
        m_BitInvert     (false),
        m_cArray        (false),
        m_AutoDelim     (true),
        m_LineEndDelim  (true),
        m_outputText    (""),


        m_inNumValues  (0),
        m_inNumBits    (0),
        m_outNumValues (0),
        m_outNumBits   (0)
    {
        m_ioDataIn.clear();
        m_bitData.clear();
        m_ioDataOut.clear();

    }


    void generateOutputData(bool b_inputChanged);
    void getInputValues(bool b_inputChanged, QStringList& inValues);
    void outputDataToStr();
    void outputAsciiDataToStr();
    void outputBase64DataToStr();

    QString m_Input;
    QString m_Delimiter;
    int m_InBase;
    int m_InBitsPer;
    int m_InBitShift;
    int m_OutBase;
    int m_OutBitsPer;
    int m_OutBitShift;
    int m_NumRows;
    bool m_InBase64;
    bool m_InAscii;
    bool m_InSigned;
    bool m_InByteRev;
    bool m_InBitRev;
    bool m_OutBase64;
    bool m_OutAscii;
    bool m_OutSigned;
    bool m_OutByteRev;
    bool m_OutBitRev;
    bool m_BitInvert;
    bool m_cArray;
    bool m_AutoDelim;
    bool m_LineEndDelim;

    ioData m_ioDataIn;
    bitData m_bitData;
    ioData m_ioDataOut;

    QString m_outputText;

    int m_inNumValues;
    int m_inNumBits;
    int m_outNumValues;
    int m_outNumBits;
private:

};


#endif

