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
#ifndef bitViewerGuiTab_h
#define bitViewerGuiTab_h

#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include "bitViewerData.h"

typedef enum
{
    GUI_INPUT,
    GUI_DELIMITER,
    GUI_IN_BASE,
    GUI_IN_BITS_PER,
    GUI_IN_BIT_SHIFT,
    GUI_OUT_BASE,
    GUI_OUT_BITS_PER,
    GUI_OUT_BIT_SHIFT,
    GUI_NUM_ROWS,
    GUI_IN_BASE64,
    GUI_IN_ASCII,
    GUI_IN_SIGNED,
    GUI_IN_BYTE_REV,
    GUI_IN_BIT_REV,
    GUI_OUT_BASE64,
    GUI_OUT_ASCII,
    GUI_OUT_SIGNED,
    GUI_OUT_BYTE_REV,
    GUI_OUT_BIT_REV,
    GUI_BIT_INVERT,
    GUI_C_ARRAY,
    GUI_AUTO_DELIM,
    GUI_LINE_END_DELIM,
    GUI_END
}eGuiInputObject;

class GuiObject
{
public:
    GuiObject(void* guiPtr){}

    virtual bool hasGuiChanged() = 0;
    virtual void guiValueChanged() = 0;
    virtual void outputUpdated() = 0;
    virtual void writeValueToGui() = 0;
private:
    GuiObject();
};

class CheckBoxObject: public GuiObject
{
public:
    CheckBoxObject(void* guiPtr, bool* guiVal):
        GuiObject(guiPtr),
        mp_chkBox((QCheckBox*)guiPtr),
        m_lastUpdatedValue(false),
        m_guiValue(guiVal)
    {
        *m_guiValue = false;
    }

    bool hasGuiChanged(){ return (m_lastUpdatedValue & 1) != (*m_guiValue & 1); }
    void guiValueChanged(){ *m_guiValue = (mp_chkBox->isChecked() & 1); }
    void outputUpdated(){ m_lastUpdatedValue = (*m_guiValue & 1); }
    void writeValueToGui(){ mp_chkBox->setChecked((*m_guiValue & 1)); }

    bool getGuiValue(){ return (*m_guiValue & 1); }
private:
    CheckBoxObject();

    QCheckBox* mp_chkBox;
    bool m_lastUpdatedValue;
    bool* m_guiValue;
};


class SpinBoxObject: public GuiObject
{
public:
    SpinBoxObject(void* guiPtr, int* guiVal):
        GuiObject(guiPtr),
        mp_spnBox((QSpinBox*)guiPtr),
        m_lastUpdatedValue(0),
        m_guiValue(guiVal)
    {
        *m_guiValue = 0;
    }

    bool hasGuiChanged(){ return m_lastUpdatedValue != *m_guiValue; }
    void guiValueChanged(){ *m_guiValue = mp_spnBox->value(); }
    void outputUpdated(){ m_lastUpdatedValue = *m_guiValue; }
    void writeValueToGui(){ mp_spnBox->setValue(*m_guiValue); }

    int getGuiValue(){ return *m_guiValue; }
private:
    SpinBoxObject();

    QSpinBox* mp_spnBox;
    int m_lastUpdatedValue;
    int* m_guiValue;
};


class LineEditObject: public GuiObject
{
public:
    LineEditObject(void* guiPtr, QString* guiVal):
        GuiObject(guiPtr),
        mp_lineEdit((QLineEdit*)guiPtr),
        m_lineUpdated(false),
        m_guiValue(guiVal)
    {
        *m_guiValue = "";
    }

    bool hasGuiChanged(){ return m_lineUpdated; }
    void guiValueChanged(){ *m_guiValue = mp_lineEdit->text(); m_lineUpdated = true; }
    void outputUpdated(){ m_lineUpdated = false; }
    void writeValueToGui(){ mp_lineEdit->setText(*m_guiValue); }

    QString getGuiValue(){ return *m_guiValue; }
private:
    LineEditObject();

    QLineEdit* mp_lineEdit;
    bool m_lineUpdated;
    QString* m_guiValue;
};

class GuiTab
{
 public:
    GuiTab(void** p_guiPtrs);
    GuiTab(void** p_guiPtrs, BitViewerData& bitViewOp);

    ~GuiTab();

    void guiValueChanged(eGuiInputObject e_obj);
    void generateOutput(bool b_forceUpdate);
    QString* getOutputText();

    ioData* getOutputData();
    BitViewerData* getBitViewerData();

    void setBitViewerData(BitViewerData& t_bitViewerData){m_bitViewerData = t_bitViewerData;}

    void syncAllWithGui();
    void writeToGui();

private:
    GuiTab();
    bool hasInputValueChanged();
    bool hasInterpretValuesChanged();
    bool syncInterpretWithGui();
    void fillGuiObjArray(void** p_guiPtrs);
    
    GuiObject* m_guiObjs[GUI_END];
    BitViewerData m_bitViewerData;

};


#endif

