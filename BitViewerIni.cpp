/* Copyright 2012 - 2018, 2021, 2023 Dan Williams. All Rights Reserved.
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
#include <QMainWindow>
#include <vector>
#include <QModelIndex>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QPlainTextEdit>
#include "DataTypes.h"
#include "IOUtilities.h"
#include <QStringListModel>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "dString.h"
#include "FileSystemOperations.h"
#include "BitViewerIni.h"

static const std::string INI_TAB_ACTIVE_INDEX   = "ActiveTabIndex";

static const std::string INI_TAB_DELIM          = "GuiTab";
static const std::string INI_TAB_NUM            = "TabNumber";
static const std::string INI_TAB_NAME           = "TabName";

static const std::string INI_TAB_INPUT          = "TabInput";
static const std::string INI_TAB_DELIMITER      = "TabDelimiter";
static const std::string INI_TAB_IN_BASE        = "TabInBase";
static const std::string INI_TAB_IN_BITS_PER    = "TabInBitsPer";
static const std::string INI_TAB_IN_BIT_SHIFT   = "TabInBitShift";
static const std::string INI_TAB_OUT_BASE       = "TabOutBase";
static const std::string INI_TAB_OUT_BITS_PER   = "TabOutBitsPer";
static const std::string INI_TAB_OUT_BIT_SHIFT  = "TabOutBitShift";
static const std::string INI_TAB_NUM_ROWS       = "TabNumRows";
static const std::string INI_TAB_IN_BASE64      = "TabInBase64";
static const std::string INI_TAB_IN_ASCII       = "TabInAscii";
static const std::string INI_TAB_IN_SIGNED      = "TabInSigned";
static const std::string INI_TAB_IN_BYTE_REV    = "TabInByteRev";
static const std::string INI_TAB_IN_BIT_REV     = "TabInBitRev";
static const std::string INI_TAB_OUT_BASE64     = "TabOutBase64";
static const std::string INI_TAB_OUT_ASCII      = "TabOutAscii";
static const std::string INI_TAB_OUT_SIGNED     = "TabOutSigned";
static const std::string INI_TAB_OUT_BYTE_REV   = "TabOutByteRev";
static const std::string INI_TAB_OUT_BIT_REV    = "TabOutBitRev";
static const std::string INI_TAB_BIT_INVERT     = "TabBitInvert";
static const std::string INI_TAB_C_ARRAY        = "TabcArray";
static const std::string INI_TAB_AUTO_DELIM     = "TabAutoDelim";
static const std::string INI_TAB_LINE_END_DELIM = "TabLineEndDelim";

static const std::string INI_START_OPEN = "{";
static const std::string INI_START_CLOSE = "}=";
static const std::string INI_END_OPEN = "{/";
static const std::string INI_END_CLOSE = "}\r\n";

static std::string writeIniInt(const std::string& t_delim, const int i_val)
{
    std::stringstream ss("");
    ss << INI_START_OPEN << t_delim << INI_START_CLOSE << i_val << INI_END_OPEN << t_delim << INI_END_CLOSE;
    return ss.str();
}
static std::string writeIniStr(const std::string& t_delim, const std::string& t_val)
{
    std::stringstream ss("");
    ss << INI_START_OPEN << t_delim << INI_START_CLOSE << t_val << INI_END_OPEN << t_delim << INI_END_CLOSE;
    return ss.str();
}
static std::string writeIniBool(const std::string& t_delim, const bool b_val)
{
    std::stringstream ss("");
    int i_val = b_val ? 1 : 0;

    ss << INI_START_OPEN << t_delim << INI_START_CLOSE << i_val << INI_END_OPEN << t_delim << INI_END_CLOSE;
    return ss.str();
}

static void addOrOverrideToIni(std::string& t_iniText, const std::string& curIniText, const std::string& newIniText)
{
    if(dString::InStr(t_iniText, curIniText) >= 0)
    {
        // There is already an entry. Replace it with the new text.
        t_iniText = dString::Replace(t_iniText, curIniText, newIniText);
    }
    else
    {
        // No entry exists, add to the beginning.
        t_iniText = newIniText + t_iniText;
    }
}

void writeIniInt(const std::string& t_delim, const int i_val, std::string& t_iniText)
{
    // Add / override entry in ini.
    auto origVal = readIniInt(t_delim, t_iniText);
    auto curIniText = writeIniInt(t_delim, origVal);
    auto newIniText = writeIniInt(t_delim, i_val);
    addOrOverrideToIni(t_iniText, curIniText, newIniText);
}
void writeIniStr(const std::string& t_delim, const std::string& t_val, std::string& t_iniText)
{
    // Add / override entry in ini.
    auto origVal = readIniStr(t_delim, t_iniText);
    auto curIniText = writeIniStr(t_delim, origVal);
    auto newIniText = writeIniStr(t_delim, t_val);
    addOrOverrideToIni(t_iniText, curIniText, newIniText);
}
void writeIniBool(const std::string& t_delim, const bool b_val, std::string& t_iniText)
{
    // Add / override entry in ini.
    auto origVal = readIniBool(t_delim, t_iniText);
    auto curIniText = writeIniBool(t_delim, origVal);
    auto newIniText = writeIniBool(t_delim, b_val);
    addOrOverrideToIni(t_iniText, curIniText, newIniText);
}

int readIniInt(const std::string& t_delim, std::string t_iniText)
{
    std::string t_searchStart("");
    std::string t_searchEnd("");
    std::string t_strReturn("");

    t_searchStart.append(INI_START_OPEN).append(t_delim).append(INI_START_CLOSE);
    t_searchEnd.append(INI_END_OPEN).append(t_delim).append(INI_END_CLOSE);
    t_strReturn = dString::GetMiddle(&t_iniText, t_searchStart, t_searchEnd);
    return asciiToInt(t_strReturn, 10, true);
}
std::string readIniStr(const std::string& t_delim, std::string t_iniText)
{
    std::string t_searchStart("");
    std::string t_searchEnd("");

    t_searchStart.append(INI_START_OPEN).append(t_delim).append(INI_START_CLOSE);
    t_searchEnd.append(INI_END_OPEN).append(t_delim).append(INI_END_CLOSE);
    return dString::GetMiddle(&t_iniText, t_searchStart, t_searchEnd);
}
bool readIniBool(const std::string& t_delim, std::string t_iniText)
{
    std::string t_searchStart("");
    std::string t_searchEnd("");
    std::string t_strReturn("");

    t_searchStart.append(INI_START_OPEN).append(t_delim).append(INI_START_CLOSE);
    t_searchEnd.append(INI_END_OPEN).append(t_delim).append(INI_END_CLOSE);
    t_strReturn = dString::GetMiddle(&t_iniText, t_searchStart, t_searchEnd);
    return (asciiToInt(t_strReturn, 10, true) != 0);
}

void writeToIniFile(std::string t_iniPath, std::vector<tIniParam>& t_iniParams)
{
    std::vector<tIniParam>::iterator t_iter;
    std::string t_iniFile("");
    std::string t_line("");

    for(t_iter = t_iniParams.begin(); t_iter < t_iniParams.end(); ++t_iter)
    {
        switch(t_iter->e_type)
        {
            case INI_PLAIN_TEXT_EDIT:
                t_line = writeIniStr(t_iter->t_name, ((QPlainTextEdit*)t_iter->p_param)->toPlainText().toStdString());
            break;
            case INI_LINE_TEXT:
                t_line = writeIniStr(t_iter->t_name, ((QLineEdit*)t_iter->p_param)->text().toStdString());
            break;
            case INI_SPIN_BOX:
                t_line = writeIniInt(t_iter->t_name, ((QSpinBox*)t_iter->p_param)->value());
            break;
            case INI_CHECK_BOX:
                t_line = writeIniBool(t_iter->t_name, ((QCheckBox*)t_iter->p_param)->isChecked());
            break;
            case INI_NANOSEC_MAX_UINT:
            {
                char ac_ns[100];
                snprintf(ac_ns, sizeof(ac_ns), "%f",
                        (float)(*((INT_UMAX*)t_iter->p_param))/((float)1000000000.0) );
                t_line = writeIniStr(t_iter->t_name, ac_ns);
            }
            break;
        }
        t_iniFile.append(t_line);
    }
    fso::WriteFile(t_iniPath, t_iniFile);
}

void readFromIniFile(std::string t_iniPath, std::vector<tIniParam>& t_iniParams)
{
    std::vector<tIniParam>::iterator t_iter;
    std::string t_iniFile = fso::ReadFile(t_iniPath);

    if(t_iniFile != "")
    {
        for(t_iter = t_iniParams.begin(); t_iter < t_iniParams.end(); ++t_iter)
        {
            switch(t_iter->e_type)
            {
                case INI_PLAIN_TEXT_EDIT:
                    ((QPlainTextEdit*)t_iter->p_param)->setPlainText(QString::fromStdString(readIniStr(t_iter->t_name, t_iniFile)));
                break;
                case INI_LINE_TEXT:
                    ((QLineEdit*)t_iter->p_param)->setText(QString::fromStdString(readIniStr(t_iter->t_name, t_iniFile)));
                break;
                case INI_SPIN_BOX:
                    ((QSpinBox*)t_iter->p_param)->setValue(readIniInt(t_iter->t_name, t_iniFile));
                break;
                case INI_CHECK_BOX:
                    ((QCheckBox*)t_iter->p_param)->setChecked(readIniBool(t_iter->t_name, t_iniFile));
                break;
                // Do nothing
                default:
                case INI_NANOSEC_MAX_UINT:
                break;
            }

        }
    }

}

void addIniParamToVector(std::vector<tIniParam>& t_iniParams, std::string t_name, eIniType e_type, void* p_param)
{
    tIniParam t_newParam;
    t_newParam.t_name = t_name;
    t_newParam.e_type = e_type;
    t_newParam.p_param = p_param;
    t_iniParams.push_back(t_newParam);
}


void writeActiveTabToIni(std::string t_iniPath, UINT_32 i_tabNum)
{
    std::string t_iniFile = fso::ReadFile(t_iniPath);

    t_iniFile.append("\r\n").append(writeIniInt(INI_TAB_ACTIVE_INDEX, i_tabNum)).append("\r\n");

    fso::WriteFile(t_iniPath, t_iniFile);
}

void writeTabToIni(std::string t_iniPath, UINT_32 i_tabNum, const std::string& t_tabName, BitViewerData& t_bitViewerData)
{
    std::string t_iniFile = fso::ReadFile(t_iniPath);
    std::string t_tabText("\r\n");
      
    t_iniFile.append("\r\n######## TAB ").append(intToAscii(i_tabNum, 10, 32, false)).append(" ########\r\n");

    t_tabText.append(writeIniInt (INI_TAB_NUM           , i_tabNum));
    t_tabText.append(writeIniStr (INI_TAB_NAME          , t_tabName));

    t_tabText.append(writeIniStr (INI_TAB_INPUT         , t_bitViewerData.m_Input.toStdString()));
    t_tabText.append(writeIniStr (INI_TAB_DELIMITER     , t_bitViewerData.m_Delimiter.toStdString()));
    t_tabText.append(writeIniInt (INI_TAB_IN_BASE       , t_bitViewerData.m_InBase      ));
    t_tabText.append(writeIniInt (INI_TAB_IN_BITS_PER   , t_bitViewerData.m_InBitsPer   ));
    t_tabText.append(writeIniInt (INI_TAB_IN_BIT_SHIFT  , t_bitViewerData.m_InBitShift  ));
    t_tabText.append(writeIniInt (INI_TAB_OUT_BASE      , t_bitViewerData.m_OutBase     ));
    t_tabText.append(writeIniInt (INI_TAB_OUT_BITS_PER  , t_bitViewerData.m_OutBitsPer  ));
    t_tabText.append(writeIniInt (INI_TAB_OUT_BIT_SHIFT , t_bitViewerData.m_OutBitShift ));
    t_tabText.append(writeIniInt (INI_TAB_NUM_ROWS      , t_bitViewerData.m_NumRows     ));
    t_tabText.append(writeIniBool(INI_TAB_IN_BASE64     , t_bitViewerData.m_InBase64    ));
    t_tabText.append(writeIniBool(INI_TAB_IN_ASCII      , t_bitViewerData.m_InAscii     ));
    t_tabText.append(writeIniBool(INI_TAB_IN_SIGNED     , t_bitViewerData.m_InSigned    ));
    t_tabText.append(writeIniBool(INI_TAB_IN_BYTE_REV   , t_bitViewerData.m_InByteRev   ));
    t_tabText.append(writeIniBool(INI_TAB_IN_BIT_REV    , t_bitViewerData.m_InBitRev    ));
    t_tabText.append(writeIniBool(INI_TAB_OUT_BASE64    , t_bitViewerData.m_OutBase64    ));
    t_tabText.append(writeIniBool(INI_TAB_OUT_ASCII     , t_bitViewerData.m_OutAscii    ));
    t_tabText.append(writeIniBool(INI_TAB_OUT_SIGNED    , t_bitViewerData.m_OutSigned   ));
    t_tabText.append(writeIniBool(INI_TAB_OUT_BYTE_REV  , t_bitViewerData.m_OutByteRev  ));
    t_tabText.append(writeIniBool(INI_TAB_OUT_BIT_REV   , t_bitViewerData.m_OutBitRev   ));
    t_tabText.append(writeIniBool(INI_TAB_BIT_INVERT    , t_bitViewerData.m_BitInvert   ));
    t_tabText.append(writeIniBool(INI_TAB_C_ARRAY       , t_bitViewerData.m_cArray      ));
    t_tabText.append(writeIniBool(INI_TAB_AUTO_DELIM    , t_bitViewerData.m_AutoDelim   ));
    t_tabText.append(writeIniBool(INI_TAB_LINE_END_DELIM, t_bitViewerData.m_LineEndDelim));

    t_iniFile.append(writeIniStr(INI_TAB_DELIM, t_tabText));

    fso::WriteFile(t_iniPath, t_iniFile);

}

static std::string getTabInfoFromIni(std::string t_iniText, UINT_32 i_tabNum)
{
    std::string t_searchStart("");
    std::string t_searchEnd("");
    std::string t_tabText("");
    std::string t_requestedTabText("");

    UINT_32 i_curTabNum = 0;

    t_searchStart.append(INI_START_OPEN).append(INI_TAB_DELIM).append(INI_START_CLOSE);
    t_searchEnd.append(INI_END_OPEN).append(INI_TAB_DELIM).append(INI_END_CLOSE);

    while(dString::InStr(t_iniText, t_searchStart) >= 0)
    {
        t_tabText = dString::GetMiddle(&t_iniText, t_searchStart, t_searchEnd);
        if(t_tabText != "")
        {
            i_curTabNum = readIniInt(INI_TAB_NUM, t_tabText);
            if(i_curTabNum == i_tabNum)
            {
                t_requestedTabText = t_tabText;
            }
        }
    }

    return t_requestedTabText;
}

UINT_32 getActiveTabFromIni(std::string t_iniPath)
{
    return readIniInt(INI_TAB_ACTIVE_INDEX, fso::ReadFile(t_iniPath));
}


UINT_32 getNumGuiTabsFromIni(std::string t_iniPath)
{
    std::string t_iniFile = fso::ReadFile(t_iniPath);
    UINT_32 i_maxTabNum = 0;

    while(1)
    {
        if(getTabInfoFromIni(t_iniFile, i_maxTabNum) != "")
        {
            ++i_maxTabNum;
        }
        else
        {
            break;
        }
    }

    return i_maxTabNum;
}

std::string getTabNameFromIni(std::string t_iniPath, UINT_32 i_tabNum)
{
    return readIniStr(INI_TAB_NAME, getTabInfoFromIni(fso::ReadFile(t_iniPath), i_tabNum));
}

BitViewerData getTabBitViewerDataFromIni(std::string t_iniPath, UINT_32 i_tabNum)
{
    BitViewerData t_bitViewerData;
    std::string t_iniFile = fso::ReadFile(t_iniPath);
    std::string t_tabText = getTabInfoFromIni(t_iniFile, i_tabNum);

    if(t_tabText != "")
    {
        t_bitViewerData.m_Input        = QString::fromStdString(readIniStr(INI_TAB_INPUT, t_tabText));
        t_bitViewerData.m_Delimiter    = QString::fromStdString(readIniStr(INI_TAB_DELIMITER, t_tabText));
        t_bitViewerData.m_InBase       = readIniInt (INI_TAB_IN_BASE       , t_tabText);
        t_bitViewerData.m_InBitsPer    = readIniInt (INI_TAB_IN_BITS_PER   , t_tabText);
        t_bitViewerData.m_InBitShift   = readIniInt (INI_TAB_IN_BIT_SHIFT  , t_tabText);
        t_bitViewerData.m_OutBase      = readIniInt (INI_TAB_OUT_BASE      , t_tabText);
        t_bitViewerData.m_OutBitsPer   = readIniInt (INI_TAB_OUT_BITS_PER  , t_tabText);
        t_bitViewerData.m_OutBitShift  = readIniInt (INI_TAB_OUT_BIT_SHIFT , t_tabText);
        t_bitViewerData.m_NumRows      = readIniInt (INI_TAB_NUM_ROWS      , t_tabText);
        t_bitViewerData.m_InBase64     = readIniBool(INI_TAB_IN_BASE64     , t_tabText);
        t_bitViewerData.m_InAscii      = readIniBool(INI_TAB_IN_ASCII      , t_tabText);
        t_bitViewerData.m_InSigned     = readIniBool(INI_TAB_IN_SIGNED     , t_tabText);
        t_bitViewerData.m_InByteRev    = readIniBool(INI_TAB_IN_BYTE_REV   , t_tabText);
        t_bitViewerData.m_InBitRev     = readIniBool(INI_TAB_IN_BIT_REV    , t_tabText);
        t_bitViewerData.m_OutBase64    = readIniBool(INI_TAB_OUT_BASE64    , t_tabText);
        t_bitViewerData.m_OutAscii     = readIniBool(INI_TAB_OUT_ASCII     , t_tabText);
        t_bitViewerData.m_OutSigned    = readIniBool(INI_TAB_OUT_SIGNED    , t_tabText);
        t_bitViewerData.m_OutByteRev   = readIniBool(INI_TAB_OUT_BYTE_REV  , t_tabText);
        t_bitViewerData.m_OutBitRev    = readIniBool(INI_TAB_OUT_BIT_REV   , t_tabText);
        t_bitViewerData.m_BitInvert    = readIniBool(INI_TAB_BIT_INVERT    , t_tabText);
        t_bitViewerData.m_cArray       = readIniBool(INI_TAB_C_ARRAY       , t_tabText);
        t_bitViewerData.m_AutoDelim    = readIniBool(INI_TAB_AUTO_DELIM    , t_tabText);
        t_bitViewerData.m_LineEndDelim = readIniBool(INI_TAB_LINE_END_DELIM, t_tabText);

    }
   return t_bitViewerData;
}
