/* Copyright 2012 - 2018, 2023 Dan Williams. All Rights Reserved.
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
#ifndef BitViewerIni_h
#define BitViewerIni_h

#include "bitViewerData.h"

typedef enum
{
    INI_LINE_TEXT,
    INI_PLAIN_TEXT_EDIT,
    INI_SPIN_BOX,
    INI_CHECK_BOX,
    INI_NANOSEC_MAX_UINT
}eIniType;

typedef struct
{
    std::string t_name;
    eIniType e_type;
    void* p_param;
}tIniParam;


void writeIniInt(const std::string& t_delim, const int i_val, std::string& t_iniText);
void writeIniStr(const std::string& t_delim, const std::string& t_val, std::string& t_iniText);
void writeIniBool(const std::string& t_delim, const bool b_val, std::string& t_iniText);

int readIniInt(const std::string& t_delim, std::string t_iniText);
std::string readIniStr(const std::string& t_delim, std::string t_iniText);
bool readIniBool(const std::string& t_delim, std::string t_iniText);

void writeToIniFile(std::string t_iniPath, std::vector<tIniParam>& t_iniParams);
void readFromIniFile(std::string t_iniPath, std::vector<tIniParam>& t_iniParams);
void addIniParamToVector(std::vector<tIniParam>& t_iniParams, std::string t_name, eIniType e_type, void* p_param);

void writeActiveTabToIni(std::string t_iniPath, UINT_32 i_tabNum);
void writeTabToIni(std::string t_iniPath, UINT_32 i_tabNum, const std::string& t_tabName, BitViewerData& t_bitViewerData);

UINT_32 getActiveTabFromIni(std::string t_iniPath);
UINT_32 getNumGuiTabsFromIni(std::string t_iniPath);
std::string getTabNameFromIni(std::string t_iniPath, UINT_32 i_tabNum);
BitViewerData getTabBitViewerDataFromIni(std::string t_iniPath, UINT_32 i_tabNum);


#endif
