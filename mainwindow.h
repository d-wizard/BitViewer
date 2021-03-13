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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tabcomparedialog.h"

#include <QMainWindow>
#include <vector>
#include "DataTypes.h"
#include <new>
#include <QModelIndex>
#include <QTime>
#include <QElapsedTimer>
#include "BitViewerIni.h"
#include "bitViewerGuiTab.h"


//#define TIME_PROFILE
//#define FAILURE_INI

#ifdef TIME_PROFILE
   #define TIME_START m_timer.start();
   #define TIME_END(x) m_diffTimeArray[x] = m_timer.nsecsElapsed();
#else
   #define TIME_START
   #define TIME_END(x)
#endif

class TabCompareDialog;

class BitViewerGuiTab
{
public:
    typedef struct
    {
        int i_bitsPer;
        bool b_signed;
    } tAsciiSave;

    BitViewerGuiTab( QTabWidget* p_tabWidget,
                     void** p_guiTabPtrs,
                     const QString& tabName):
        m_tab(p_tabWidget),
        m_guiTab(p_guiTabPtrs),
        m_tabName(tabName)
    {
        p_tabWidget->addTab(&m_tab, tabName);
        initSaveParam(p_guiTabPtrs);
    }
    BitViewerGuiTab( QTabWidget* p_tabWidget,
                     void** p_guiTabPtrs,
                     BitViewerData& bitViewOp,
                     const QString& tabName):
        m_tab(p_tabWidget),
        m_guiTab(p_guiTabPtrs, bitViewOp),
        m_tabName(tabName)
    {
        p_tabWidget->addTab(&m_tab, tabName);
        initSaveParam(p_guiTabPtrs);
    }
    GuiTab* getIoGuiTab(){ return &m_guiTab; }

    void setBitViewerData(BitViewerData& t_bitViewerData)
    {
        m_guiTab.setBitViewerData(t_bitViewerData);
    }

    QString getTabName(){ return m_tabName; }
    void setTabName(QString newName){ m_tabName = newName; }

    tAsciiSave m_asciiSaveIn;
    tAsciiSave m_asciiSaveOut;
    tAsciiSave m_base64SaveIn;
    tAsciiSave m_base64SaveOut;

    int i_bitsPerInSave;
    int i_bitsPerOutSave;

private:
    BitViewerGuiTab();
    void initSaveParam(void** p_guiTabPtrs)
    {
       m_asciiSaveIn.i_bitsPer = ((QSpinBox*)p_guiTabPtrs[GUI_IN_BITS_PER])->value();
       m_asciiSaveIn.b_signed = ((QCheckBox*)p_guiTabPtrs[GUI_IN_SIGNED])->isChecked();

       m_asciiSaveOut.i_bitsPer = ((QSpinBox*)p_guiTabPtrs[GUI_OUT_BITS_PER])->value();
       m_asciiSaveOut.b_signed = ((QCheckBox*)p_guiTabPtrs[GUI_OUT_SIGNED])->isChecked();

       m_base64SaveIn.i_bitsPer = ((QSpinBox*)p_guiTabPtrs[GUI_IN_BITS_PER])->value();
       m_base64SaveIn.b_signed = ((QCheckBox*)p_guiTabPtrs[GUI_IN_SIGNED])->isChecked();

       m_base64SaveOut.i_bitsPer = ((QSpinBox*)p_guiTabPtrs[GUI_OUT_BITS_PER])->value();
       m_base64SaveOut.b_signed = ((QCheckBox*)p_guiTabPtrs[GUI_OUT_SIGNED])->isChecked();

        i_bitsPerInSave = m_asciiSaveIn.i_bitsPer;
        i_bitsPerOutSave = m_asciiSaveOut.i_bitsPer;
    }

    QWidget m_tab;
    GuiTab m_guiTab;
    QString m_tabName;
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    typedef enum
    {
        BYTE_REVERSE,
        BIT_REVERSE,
        BIT_SHIFT,
        MOD_INVALID
    }e_modType;



public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
#if 0
    void on_cmdAddByteReverseIn_clicked();

    void on_cmdAddBitReverseIn_clicked();

    void on_cmdAddBitShiftIn_clicked();

    void on_cmdDeleteIn_clicked();
#endif

    void on_txtInput_textChanged(const QString &arg1);

    void on_txtInput_editingFinished();

    void on_spnBitsPerIn_valueChanged(int arg1);
    
    void on_txtDelimiter_textChanged(const QString &arg1);

    void on_txtDelimiter_editingFinished();

    void on_spnBaseIn_editingFinished();

    void on_spnBitsPerIn_editingFinished();

    void on_chkSignedIn_clicked();

    void on_chkByteReverseIn_clicked();

    void on_chkBitReverseIn_clicked();

    void on_spnBitShiftIn_editingFinished();

    void on_spnBaseOut_editingFinished();

    void on_spnBitsPerOut_editingFinished();

    void on_chkSignedOut_clicked();

    void on_chkByteReverseOut_clicked();

    void on_chkBitReverseOut_clicked();

    void on_spnBitShiftOut_editingFinished();

    void on_chkInvert_clicked();

    void on_chkCArray_clicked();

    void on_spnNumRows_editingFinished();

    void on_spnBaseIn_valueChanged(int arg1);

    void on_spnBitShiftIn_valueChanged(int arg1);

    void on_spnBaseOut_valueChanged(int arg1);

    void on_spnBitsPerOut_valueChanged(int arg1);

    void on_spnBitShiftOut_valueChanged(int arg1);

    void on_spnNumRows_valueChanged(int arg1);

private slots:
    void ReadInputFromBinaryFile();
    void ReadInputFromAsciiFile();

    void AddBitViewerTab();
    void CompareTabs();
    void RenameTab();

    void WriteOutputToBinaryFile();
    void WriteOutputToAsciiFile();
    void WriteOutputToCsvFile();

    void CopyOutputForExcel();


    void on_chkDelimAuto_stateChanged(int arg1);

    void on_chkAsciiIn_stateChanged(int arg1);

    void on_chkAsciiOut_stateChanged(int arg1);

    void on_chkLineEndDelim_stateChanged(int arg1);

    void on_tabWidget_tabCloseRequested(int index);

    void on_tabWidget_currentChanged(int index);

    void on_cmdUpdateInput_clicked();

    void on_chkBase64In_stateChanged(int arg1);

    void on_chkBase64Out_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;

    // Sub GUI's
    TabCompareDialog m_tabCompare;

#if 0
    // Constants Strings
    QString mt_byteReverseStr;
    QString mt_bitReverseStr;
    QString mt_bitShiftStr;

    // On GUI Functions
    void readModType(const QString& t_modStr, e_modType& e_mod, INT_32& i_shiftValue);
#endif


    std::vector<tIniParam> m_iniParams;
    int m_readyToPrint;
    int m_ignorGuiChange;
    int m_ignorTabChange;

    QStringList  m_modifiersIn;

    void* m_guiPtrs[GUI_END];
    std::vector<BitViewerGuiTab*> m_guiTabs;

    BitViewerGuiTab* mp_curGuiTab;

    void printOutputToGui();

    void syncGuiTab();

    void updateOutputOnChange(eGuiInputObject e_obj);
    void updateForce();
    void updateInput();


#ifdef TIME_PROFILE
    QElapsedTimer m_stringPrintTimer;
    QElapsedTimer m_nonStringPrintTimer;
    INT_UMAX m_stringPrintTimer_ns;
    INT_UMAX m_nonStringPrintTimer_ns;

    QElapsedTimer m_timer;
    INT_UMAX m_diffTimeArray[50];
#endif
      

public:
    void compareTabs_Finished(UINT_32 dst, UINT_32 src);

};

#endif // MAINWINDOW_H
