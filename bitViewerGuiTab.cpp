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
#include "dString.h"
#include "bitOperations.h"
#include "bitViewerGuiTab.h"

GuiTab::GuiTab(void** p_guiPtrs)
{
    fillGuiObjArray(p_guiPtrs);
    syncAllWithGui();
}

GuiTab::GuiTab(void** p_guiPtrs, BitViewerData& bitViewOp)
{
    fillGuiObjArray(p_guiPtrs);
    m_bitViewerData = bitViewOp;
}

GuiTab::~GuiTab()
{
   for(int i = 0; i < GUI_END; ++i)
   {
      delete m_guiObjs[i];
   }
}


void GuiTab::fillGuiObjArray(void** p_guiPtrs)
{
    m_guiObjs[GUI_INPUT         ] = new LineEditObject(p_guiPtrs[GUI_INPUT         ], &m_bitViewerData.m_Input       );
    m_guiObjs[GUI_DELIMITER     ] = new LineEditObject(p_guiPtrs[GUI_DELIMITER     ], &m_bitViewerData.m_Delimiter   );
    m_guiObjs[GUI_IN_BASE       ] = new SpinBoxObject (p_guiPtrs[GUI_IN_BASE       ], &m_bitViewerData.m_InBase      );
    m_guiObjs[GUI_IN_BITS_PER   ] = new SpinBoxObject (p_guiPtrs[GUI_IN_BITS_PER   ], &m_bitViewerData.m_InBitsPer   );
    m_guiObjs[GUI_IN_BIT_SHIFT  ] = new SpinBoxObject (p_guiPtrs[GUI_IN_BIT_SHIFT  ], &m_bitViewerData.m_InBitShift  );
    m_guiObjs[GUI_OUT_BASE      ] = new SpinBoxObject (p_guiPtrs[GUI_OUT_BASE      ], &m_bitViewerData.m_OutBase     );
    m_guiObjs[GUI_OUT_BITS_PER  ] = new SpinBoxObject (p_guiPtrs[GUI_OUT_BITS_PER  ], &m_bitViewerData.m_OutBitsPer  );
    m_guiObjs[GUI_OUT_BIT_SHIFT ] = new SpinBoxObject (p_guiPtrs[GUI_OUT_BIT_SHIFT ], &m_bitViewerData.m_OutBitShift );
    m_guiObjs[GUI_NUM_ROWS      ] = new SpinBoxObject (p_guiPtrs[GUI_NUM_ROWS      ], &m_bitViewerData.m_NumRows     );
    m_guiObjs[GUI_IN_ASCII      ] = new CheckBoxObject(p_guiPtrs[GUI_IN_ASCII      ], &m_bitViewerData.m_InAscii     );
    m_guiObjs[GUI_IN_SIGNED     ] = new CheckBoxObject(p_guiPtrs[GUI_IN_SIGNED     ], &m_bitViewerData.m_InSigned    );
    m_guiObjs[GUI_IN_BYTE_REV   ] = new CheckBoxObject(p_guiPtrs[GUI_IN_BYTE_REV   ], &m_bitViewerData.m_InByteRev   );
    m_guiObjs[GUI_IN_BIT_REV    ] = new CheckBoxObject(p_guiPtrs[GUI_IN_BIT_REV    ], &m_bitViewerData.m_InBitRev    );
    m_guiObjs[GUI_OUT_ASCII     ] = new CheckBoxObject(p_guiPtrs[GUI_OUT_ASCII     ], &m_bitViewerData.m_OutAscii    );
    m_guiObjs[GUI_OUT_SIGNED    ] = new CheckBoxObject(p_guiPtrs[GUI_OUT_SIGNED    ], &m_bitViewerData.m_OutSigned   );
    m_guiObjs[GUI_OUT_BYTE_REV  ] = new CheckBoxObject(p_guiPtrs[GUI_OUT_BYTE_REV  ], &m_bitViewerData.m_OutByteRev  );
    m_guiObjs[GUI_OUT_BIT_REV   ] = new CheckBoxObject(p_guiPtrs[GUI_OUT_BIT_REV   ], &m_bitViewerData.m_OutBitRev   );
    m_guiObjs[GUI_BIT_INVERT    ] = new CheckBoxObject(p_guiPtrs[GUI_BIT_INVERT    ], &m_bitViewerData.m_BitInvert   );
    m_guiObjs[GUI_C_ARRAY       ] = new CheckBoxObject(p_guiPtrs[GUI_C_ARRAY       ], &m_bitViewerData.m_cArray      );
    m_guiObjs[GUI_AUTO_DELIM    ] = new CheckBoxObject(p_guiPtrs[GUI_AUTO_DELIM    ], &m_bitViewerData.m_AutoDelim   );
    m_guiObjs[GUI_LINE_END_DELIM] = new CheckBoxObject(p_guiPtrs[GUI_LINE_END_DELIM], &m_bitViewerData.m_LineEndDelim);

}

void GuiTab::guiValueChanged(eGuiInputObject e_obj)
{
   m_guiObjs[e_obj]->guiValueChanged();
}

bool GuiTab::hasInputValueChanged()
{
   return
   (
      m_guiObjs[GUI_INPUT]->hasGuiChanged() ||
      m_guiObjs[GUI_DELIMITER]->hasGuiChanged()
   );
}

bool GuiTab::hasInterpretValuesChanged()
{
   bool b_changed = false;
   for(int i = GUI_IN_BASE; i < GUI_END && b_changed == false; ++i)
   {
      b_changed = m_guiObjs[i]->hasGuiChanged();
   }
   return b_changed;
}

QString* GuiTab::getOutputText()
{
    return &m_bitViewerData.m_outputText;
}

ioData* GuiTab::getOutputData()
{
    return &m_bitViewerData.m_ioDataOut;
}
BitViewerData* GuiTab::getBitViewerData()
{
    return &m_bitViewerData;
}

bool GuiTab::syncInterpretWithGui()
{
    for(int i = GUI_IN_BASE; i < GUI_END; ++i)
    {
        m_guiObjs[i]->guiValueChanged();
    }
    return false;
}

void GuiTab::syncAllWithGui()
{
    for(int i = 0; i < GUI_END; ++i)
    {
        m_guiObjs[i]->guiValueChanged();
    }
}

void GuiTab::writeToGui()
{
    for(int i = 0; i < GUI_END; ++i)
    {
        m_guiObjs[i]->writeValueToGui();
    }
}

void GuiTab::generateOutput(bool b_forceUpdate)
{
    bool b_notSynced = syncInterpretWithGui();
    if(b_notSynced)
    {
        b_notSynced = false;
    }
    if( (m_bitViewerData.m_Delimiter.length() != 0 || m_bitViewerData.m_InAscii == true) &&
        m_bitViewerData.m_Input.length() != 0 &&
        (b_forceUpdate || hasInputValueChanged() || hasInterpretValuesChanged()) )
    {
        m_bitViewerData.generateOutputData(hasInputValueChanged() || b_forceUpdate);
        if(m_bitViewerData.m_OutAscii)
        {
            m_bitViewerData.outputAsciiDataToStr();
        }
        else
        {
            m_bitViewerData.outputDataToStr();
        }

        for(int i = 0; i < GUI_END; ++i)
        {
            m_guiObjs[i]->outputUpdated();
        }
    }
}
