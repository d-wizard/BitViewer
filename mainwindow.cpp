/* Copyright 2012 - 2018, 2021, 2023 - 2025 Dan Williams. All Rights Reserved.
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
#include "tabcomparedialog.h"
#include "ui_tabcomparedialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "DataTypes.h"
#include "IOUtilities.h"
#include <QStringListModel>
#include "dString.h"
#include "bitOperations.h"
#include "FileSystemOperations.h"
#include "AutoDelimiter.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include "bitStreamCompare.h"
#include <QClipboard>

#define INI_FILE_PATH "BitViewer.ini"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_readyToPrint(1),
    m_ignorGuiChange(1),
    m_ignorTabChange(1),
    mp_curGuiTab(NULL)
{
    setWindowIcon(QIcon(":/bitviewer.png")); // Linux way to set the icon

    // Sub GUIs
    m_tabCompare.hide();

    ui->setupUi(this);

    m_guiPtrs[GUI_INPUT         ] = ui->txtInput;
    m_guiPtrs[GUI_DELIMITER     ] = ui->txtDelimiter;
    m_guiPtrs[GUI_IN_BASE       ] = ui->spnBaseIn;
    m_guiPtrs[GUI_IN_BITS_PER   ] = ui->spnBitsPerIn;
    m_guiPtrs[GUI_IN_BIT_SHIFT  ] = ui->spnBitShiftIn;
    m_guiPtrs[GUI_OUT_BASE      ] = ui->spnBaseOut;
    m_guiPtrs[GUI_OUT_BITS_PER  ] = ui->spnBitsPerOut;
    m_guiPtrs[GUI_OUT_BIT_SHIFT ] = ui->spnBitShiftOut;
    m_guiPtrs[GUI_NUM_ROWS      ] = ui->spnNumRows;
    m_guiPtrs[GUI_IN_BASE64     ] = ui->chkBase64In;
    m_guiPtrs[GUI_IN_ASCII      ] = ui->chkAsciiIn;
    m_guiPtrs[GUI_IN_SIGNED     ] = ui->chkSignedIn;
    m_guiPtrs[GUI_IN_BYTE_REV   ] = ui->chkByteReverseIn;
    m_guiPtrs[GUI_IN_BIT_REV    ] = ui->chkBitReverseIn;
    m_guiPtrs[GUI_OUT_BASE64    ] = ui->chkBase64Out;
    m_guiPtrs[GUI_OUT_ASCII     ] = ui->chkAsciiOut;
    m_guiPtrs[GUI_OUT_SIGNED    ] = ui->chkSignedOut;
    m_guiPtrs[GUI_OUT_BYTE_REV  ] = ui->chkByteReverseOut;
    m_guiPtrs[GUI_OUT_BIT_REV   ] = ui->chkBitReverseOut;
    m_guiPtrs[GUI_BIT_INVERT    ] = ui->chkInvert;
    m_guiPtrs[GUI_C_ARRAY       ] = ui->chkCArray;
    m_guiPtrs[GUI_AUTO_DELIM    ] = ui->chkDelimAuto;
    m_guiPtrs[GUI_LINE_END_DELIM] = ui->chkLineEndDelim;
    m_guiPtrs[GUI_AUTO_IN_STDINT] = ui->chkAutoStdInt;

    ui->chkAutoStdInt->setToolTip("If checked, only standard integer sizes will be automatically detected (8, 16, 32, 64 bits).");

    m_guiTabs.clear();

#if 0
    ui->listModIn->setModel(new QStringListModel());
    // Constant Strings
    mt_byteReverseStr = "Byte Reverse";
    mt_bitReverseStr = "Bit Reverse";
    mt_bitShiftStr = "Bit Shift ";
#endif

    // For now do not use the middle section or out shift
    ui->middleGroupBox->setHidden(true);
    ui->lblBitShiftOut->setHidden(true);
    ui->spnBitShiftOut->setHidden(true);

    ui->mainToolBar->setHidden(true);

    m_iniParams.clear();
    
#ifdef TIME_PROFILE
    addIniParamToVector(m_iniParams, "StringPrintTime", INI_NANOSEC_MAX_UINT, &m_stringPrintTimer_ns);
    addIniParamToVector(m_iniParams, "nonStringPrintTime", INI_NANOSEC_MAX_UINT, &m_nonStringPrintTimer_ns);
    
    addIniParamToVector(m_iniParams, "TimerVal_00", INI_NANOSEC_MAX_UINT, &m_diffTimeArray[ 0]);
    addIniParamToVector(m_iniParams, "TimerVal_01", INI_NANOSEC_MAX_UINT, &m_diffTimeArray[ 1]);
    addIniParamToVector(m_iniParams, "TimerVal_02", INI_NANOSEC_MAX_UINT, &m_diffTimeArray[ 2]);
    addIniParamToVector(m_iniParams, "TimerVal_03", INI_NANOSEC_MAX_UINT, &m_diffTimeArray[ 3]);
    addIniParamToVector(m_iniParams, "TimerVal_04", INI_NANOSEC_MAX_UINT, &m_diffTimeArray[ 4]);
    addIniParamToVector(m_iniParams, "TimerVal_05", INI_NANOSEC_MAX_UINT, &m_diffTimeArray[ 5]);
    addIniParamToVector(m_iniParams, "TimerVal_06", INI_NANOSEC_MAX_UINT, &m_diffTimeArray[ 6]);
    addIniParamToVector(m_iniParams, "TimerVal_07", INI_NANOSEC_MAX_UINT, &m_diffTimeArray[ 7]);
    addIniParamToVector(m_iniParams, "TimerVal_08", INI_NANOSEC_MAX_UINT, &m_diffTimeArray[ 8]);
    addIniParamToVector(m_iniParams, "TimerVal_09", INI_NANOSEC_MAX_UINT, &m_diffTimeArray[ 9]);
    addIniParamToVector(m_iniParams, "TimerVal_10", INI_NANOSEC_MAX_UINT, &m_diffTimeArray[10]);
    addIniParamToVector(m_iniParams, "TimerVal_11", INI_NANOSEC_MAX_UINT, &m_diffTimeArray[11]);
#endif

    addIniParamToVector(m_iniParams, "Input"       , INI_LINE_TEXT, ui->txtInput);
    addIniParamToVector(m_iniParams, "Delimiter"   , INI_LINE_TEXT, ui->txtDelimiter);
    addIniParamToVector(m_iniParams, "InBase"      , INI_SPIN_BOX , ui->spnBaseIn);
    addIniParamToVector(m_iniParams, "InBitsPer"   , INI_SPIN_BOX , ui->spnBitsPerIn);
    addIniParamToVector(m_iniParams, "InBitShift"  , INI_SPIN_BOX , ui->spnBitShiftIn);
    addIniParamToVector(m_iniParams, "OutBase"     , INI_SPIN_BOX , ui->spnBaseOut);
    addIniParamToVector(m_iniParams, "OutBitsPer"  , INI_SPIN_BOX , ui->spnBitsPerOut);
    addIniParamToVector(m_iniParams, "OutBitShift" , INI_SPIN_BOX , ui->spnBitShiftOut);
    addIniParamToVector(m_iniParams, "NumRows"     , INI_SPIN_BOX , ui->spnNumRows);
    addIniParamToVector(m_iniParams, "InBase64"    , INI_CHECK_BOX, ui->chkBase64In);
    addIniParamToVector(m_iniParams, "InAscii"     , INI_CHECK_BOX, ui->chkAsciiIn);
    addIniParamToVector(m_iniParams, "InSigned"    , INI_CHECK_BOX, ui->chkSignedIn);
    addIniParamToVector(m_iniParams, "InByteRev"   , INI_CHECK_BOX, ui->chkByteReverseIn);
    addIniParamToVector(m_iniParams, "InBitRev"    , INI_CHECK_BOX, ui->chkBitReverseIn);
    addIniParamToVector(m_iniParams, "OutBase64"   , INI_CHECK_BOX, ui->chkBase64Out);
    addIniParamToVector(m_iniParams, "OutAscii"    , INI_CHECK_BOX, ui->chkAsciiOut);
    addIniParamToVector(m_iniParams, "OutSigned"   , INI_CHECK_BOX, ui->chkSignedOut);
    addIniParamToVector(m_iniParams, "OutByteRev"  , INI_CHECK_BOX, ui->chkByteReverseOut);
    addIniParamToVector(m_iniParams, "OutBitRev"   , INI_CHECK_BOX, ui->chkBitReverseOut);
    addIniParamToVector(m_iniParams, "BitInvert"   , INI_CHECK_BOX, ui->chkInvert);
    addIniParamToVector(m_iniParams, "cArray"      , INI_CHECK_BOX, ui->chkCArray);
    addIniParamToVector(m_iniParams, "AutoDelim"   , INI_CHECK_BOX, ui->chkDelimAuto);
    addIniParamToVector(m_iniParams, "LineEndDelim", INI_CHECK_BOX, ui->chkLineEndDelim);
//    addIniParamToVector(m_iniParams, "InAutoStdInt", INI_CHECK_BOX, ui->chkAutoStdInt); Adding this in causes a crash. But the values seems to store and restore just fine without this line.

    readFromIniFile(INI_FILE_PATH, m_iniParams);

    connect(ui->actionAdd_Tab, SIGNAL(triggered(bool)), this, SLOT(AddBitViewerTab()));
    connect(ui->actionCompare_Tabs, SIGNAL(triggered(bool)), this, SLOT(CompareTabs()));
    connect(ui->actionRename_Tab, SIGNAL(triggered(bool)), this, SLOT(RenameTab()));

    connect(ui->actionOpen_Binary_File, SIGNAL(triggered(bool)), this, SLOT(ReadInputFromBinaryFile()));
    connect(ui->actionOpen_ASCII_File, SIGNAL(triggered(bool)), this, SLOT(ReadInputFromAsciiFile()));
    connect(ui->actionWrite_Binary_File, SIGNAL(triggered(bool)), this, SLOT(WriteOutputToBinaryFile()));
    connect(ui->actionWrite_ASCII_File, SIGNAL(triggered(bool)), this, SLOT(WriteOutputToAsciiFile()));

    connect(ui->actionCopy_Output_to_Excel, SIGNAL(triggered(bool)), this, SLOT(CopyOutputForExcel()));

    connect(ui->cmdUpdateInput, SIGNAL(clicked()), this, SLOT(on_cmdUpdateInput_clicked));

    //connect(ui->actionWrite_CSV_File, SIGNAL(triggered(bool)), this, SLOT(WriteOutputToCsvFile()));


    syncGuiTab();

    UINT_32 i_numTabsFromIni = getNumGuiTabsFromIni(INI_FILE_PATH);
    for(UINT_32 i = 0; i < i_numTabsFromIni; ++i)
    {
        QString t_tabName = QString::fromStdString(getTabNameFromIni(INI_FILE_PATH, i));
        BitViewerData t_bitViewerData = getTabBitViewerDataFromIni(INI_FILE_PATH, i);
        if( (i + 1) > m_guiTabs.size() )
        {
            m_guiTabs.push_back( new BitViewerGuiTab( ui->tabWidget,
                                                      m_guiPtrs,
                                                      t_bitViewerData,
                                                      t_tabName) );
        }
        else
        {
            ui->tabWidget->setTabText(i, t_tabName);
            m_guiTabs[i]->setBitViewerData(t_bitViewerData);
        }
    }

    // Set the current GUI tab.
    UINT_32 i_curTab = 0;
    if(i_numTabsFromIni == 0)
    {
        BitViewerData t_bitViewerData;
        m_guiTabs.push_back( new BitViewerGuiTab( ui->tabWidget,
                                                  m_guiPtrs,
                                                  t_bitViewerData,
                                                  "Start Tab") );
    }
    else
    {
        i_curTab = getActiveTabFromIni(INI_FILE_PATH);
    }

    if(i_curTab < m_guiTabs.size())
    {
        ui->tabWidget->setCurrentIndex(i_curTab);
    }

    mp_curGuiTab = m_guiTabs[ui->tabWidget->currentIndex()];
    mp_curGuiTab->getIoGuiTab()->writeToGui();

    m_readyToPrint = 0;
    m_ignorGuiChange = 0;
    m_ignorTabChange = 0;

    syncGuiTab();
    updateForce();

}

MainWindow::~MainWindow()
{
    mp_curGuiTab = NULL;
    int i_numTabs = m_guiTabs.size();

    // Get values from ini before they get overwritten.
    auto lastOpenSaveDir = getLastOpenSaveDir();

    // Write GUI values to the ini (this will create a brand new ini file, so save everything off before this)
    writeToIniFile(INI_FILE_PATH, m_iniParams);

    writeActiveTabToIni(INI_FILE_PATH, ui->tabWidget->currentIndex());
    for(int i = 0; i < i_numTabs; ++i)
    {
        writeTabToIni(INI_FILE_PATH, i, ui->tabWidget->tabText(i).toStdString(), *m_guiTabs[i]->getIoGuiTab()->getBitViewerData());
    }

    // Write the rest of the ini values.
    setLastOpenSaveDir(lastOpenSaveDir);

    for(int i = 0; i < i_numTabs; ++i)
    {
        delete m_guiTabs[i];
    }

    delete ui;
}


std::string MainWindow::getLastOpenSaveDir()
{
    std::string t_iniFile = fso::ReadFile(INI_FILE_PATH);
    return readIniStr("LastOpenSaveDir", t_iniFile);
}

void MainWindow::setLastOpenSaveDir(const std::string& directory)
{
    std::string t_iniFile = fso::ReadFile(INI_FILE_PATH);
    if(fso::DirExists(directory))
    {
        // Valid directory. Save it.
        writeIniStr("LastOpenSaveDir", directory, t_iniFile);
    }
    else
    {
        // This is a file. Save the directory this file is in.
        std::string upDir = fso::GetDir(directory);
        writeIniStr("LastOpenSaveDir", upDir, t_iniFile);
    }
    fso::WriteFile(INI_FILE_PATH, t_iniFile);
}

void MainWindow::printOutputToGui()
{
    if(mp_curGuiTab != NULL)
    {
        ui->txtOutput->setPlainText(*mp_curGuiTab->getIoGuiTab()->getOutputText());
        std::stringstream infoText;
        infoText << "In Values: " << mp_curGuiTab->getIoGuiTab()->getBitViewerData()->m_inNumValues <<
                    " | In Bits: " << mp_curGuiTab->getIoGuiTab()->getBitViewerData()->m_inNumBits <<
                    " | Out Values: " << mp_curGuiTab->getIoGuiTab()->getBitViewerData()->m_outNumValues <<
                    " | Out Bits: " << mp_curGuiTab->getIoGuiTab()->getBitViewerData()->m_outNumBits;
        ui->lblInfo->setText(QString::fromStdString(infoText.str()));
    }
}

void MainWindow::updateOutputOnChange(eGuiInputObject e_obj)
{
    if(mp_curGuiTab != NULL && m_ignorTabChange == 0)
    {
        if(m_ignorGuiChange == 0)
        {
            mp_curGuiTab->getIoGuiTab()->guiValueChanged(e_obj);
        }
        if(m_readyToPrint == 0)
        {
            mp_curGuiTab->getIoGuiTab()->generateOutput(false);
            printOutputToGui();
        }
    }
}

void MainWindow::updateForce()
{
    if(mp_curGuiTab != NULL && m_ignorTabChange == 0)
    {
        mp_curGuiTab->getIoGuiTab()->generateOutput(true);
        printOutputToGui();
    }
}

void MainWindow::updateInput()
{
    if(m_ignorTabChange == 0)
    {
        if(ui->chkDelimAuto->isChecked())
        {
            std::string t_autoDelim = getDelimiter(ui->txtInput->text().toStdString());
            ui->txtDelimiter->setText(QString::fromStdString(t_autoDelim));
        }
    }
    updateOutputOnChange(GUI_INPUT);
}

// Tabs
void MainWindow::syncGuiTab()
{
    if(mp_curGuiTab != NULL && m_guiTabs.size() > 0)
    {
        mp_curGuiTab = m_guiTabs[ui->tabWidget->currentIndex()];
        ui->tabWidget->setVisible(true); // Always show the tab widget (in the past it was only shown if there were more than 1 tab)
    }
    else
    {
        mp_curGuiTab = NULL;
    }
}

void MainWindow::AddBitViewerTab()
{
    if(mp_curGuiTab != NULL)
    {
        char tabname[20];
        memset(tabname, '\0', sizeof(tabname));
        snprintf(tabname, sizeof(tabname) - 1, "Tab %d", (int)m_guiTabs.size());

        bool ok;
        QInputDialog* inputDialog = new QInputDialog();
        inputDialog->setOptions(QInputDialog::NoButtons);
        QString text =  inputDialog->getText( NULL ,
                                              "Create New Tab",
                                              "Enter New Tab Name:",
                                              QLineEdit::Normal,
                                              QString::fromStdString(tabname),
                                              &ok);


        memset(tabname, '\0', sizeof(tabname));
        snprintf(tabname, sizeof(tabname) - 1, "%s", text.toStdString().c_str());

        if(ok)
        {
            ++m_ignorGuiChange;
            //++m_ignorTabChange;
            m_guiTabs.push_back( new BitViewerGuiTab( ui->tabWidget,
                                                      m_guiPtrs,
                                                      *mp_curGuiTab->getIoGuiTab()->getBitViewerData(),
                                                      QString::fromStdString(tabname) ) );
            //--m_ignorTabChange;
            ui->tabWidget->setCurrentIndex(m_guiTabs.size()-1); // Set to the newly created tab.
            syncGuiTab();
            updateForce();
            --m_ignorGuiChange;
        }

    }
}

void MainWindow::CompareTabs()
{
    m_tabCompare.showTabCompare(m_guiTabs, this);
}

void MainWindow::RenameTab()
{
    if(mp_curGuiTab != NULL)
    {
        bool ok;
        QInputDialog* inputDialog = new QInputDialog();
        inputDialog->setOptions(QInputDialog::NoButtons);
        QString text =  inputDialog->getText( NULL ,
                                              "Rename Current Tab",
                                              "Enter New Tab Name:",
                                              QLineEdit::Normal,
                                              ui->tabWidget->tabText(ui->tabWidget->currentIndex()),
                                              &ok);


        if(ok)
        {
            ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), text);
            m_guiTabs[ui->tabWidget->currentIndex()]->setTabName(text);
        }

    }
}

void MainWindow::on_cmdUpdateInput_clicked()
{
    updateInput();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    (void)index;
    if(mp_curGuiTab != NULL)
    {
        ++m_ignorGuiChange;
        ++m_ignorTabChange;
        mp_curGuiTab->getIoGuiTab()->syncAllWithGui();
        syncGuiTab();
        mp_curGuiTab->getIoGuiTab()->writeToGui();
        --m_ignorTabChange;
        --m_ignorGuiChange;
        updateForce();
    }
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if(mp_curGuiTab != NULL)
    {
        if(m_guiTabs.size() > 1)
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Delete Tab");
            msgBox.setText("Do you want to delete this tab?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            switch( msgBox.exec() )
            {
                case QMessageBox::Yes:
                    ++m_ignorGuiChange;
                    ui->tabWidget->removeTab(index);
                    delete m_guiTabs[index];
                    m_guiTabs.erase(m_guiTabs.begin() + index);

                    syncGuiTab();

                    ++m_ignorTabChange;
                    mp_curGuiTab->getIoGuiTab()->writeToGui();
                    --m_ignorTabChange;

                    updateForce();
                    --m_ignorGuiChange;
                break;
                case QMessageBox::No:

                break;
            }
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Delete Tab");
            msgBox.setText("Can't delete the last tab. There needs to be at least 1 tab.");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
    }
}

void MainWindow::CopyOutputForExcel()
{
    QClipboard* clipboard = QApplication::clipboard();
    QString clipText;

    if(ui->chkAsciiOut->isChecked() == false && ui->chkBase64Out->isChecked() == false && ui->chkCArray->isChecked() == false)
    {
        ++m_ignorGuiChange;
        ui->chkCArray->setChecked(true);
        mp_curGuiTab->getIoGuiTab()->generateOutput(true);
        clipText = mp_curGuiTab->getIoGuiTab()->getOutputText()->replace(",", "\t");
        ui->chkCArray->setChecked(false);
        mp_curGuiTab->getIoGuiTab()->generateOutput(true);
        --m_ignorGuiChange;
    }
    else
    {
        clipText = mp_curGuiTab->getIoGuiTab()->getOutputText()->replace(",", "\t");
    }
    clipText.append("\r\n");
    clipText = clipText.replace("\t\r\n","\r\n");
    clipboard->setText(clipText);
}

void MainWindow::ReadInputFromBinaryFile()
{
    std::string oldDir = getLastOpenSaveDir();
    QString startDir = fso::DirExists(oldDir) ? oldDir.c_str() : QDir::currentPath();

    QString filename = QFileDialog::getOpenFileName(
        this,
        tr("Open Binary File"),
        startDir,
        tr("All files (*.*)") );
    if( filename.isNull() )
    {
       return;
    }

    setLastOpenSaveDir(filename.toStdString()); // Save the path the user specified.

    int length;
    unsigned char* buffer;

    std::ifstream is;
    is.open(filename.toStdString().c_str(), std::ios::binary );

    // get length of file:
    is.seekg (0, std::ios::end);
    length = is.tellg();
    is.seekg (0, std::ios::beg);

    // allocate memory:
    buffer = new unsigned char [length];

    // read data as a block:
    is.read ((char*)buffer,length);
    is.close();

    m_guiTabs.push_back( new BitViewerGuiTab( ui->tabWidget,
                                              m_guiPtrs,
                                              *mp_curGuiTab->getIoGuiTab()->getBitViewerData(),
                                              QString::fromStdString(fso::GetFileNameNoExt(filename.toStdString())) ) );
    ui->tabWidget->setCurrentIndex(m_guiTabs.size()-1); // Set to the newly created tab.

    ++m_readyToPrint;
    {
        int i_index = 0;
        std::string inputTxt = "";
        for(i_index = 0; i_index < length; ++i_index)
        {
            inputTxt.append(intToAscii(buffer[i_index], 16, 8, false));
            inputTxt.append(" ");
        }
        ui->spnBaseIn->setValue(16);
        ui->spnBitsPerIn->setValue(8);
        ui->chkSignedIn->setChecked(false);
        ui->spnBaseOut->setValue(16);
        ui->spnBitsPerOut->setValue(8);
        ui->chkSignedOut->setChecked(false);
        ui->txtInput->setText(QString::fromStdString(inputTxt));
    }
    --m_readyToPrint;
    updateInput();

    delete[] buffer;

}
void MainWindow::ReadInputFromAsciiFile()
{
    std::string oldDir = getLastOpenSaveDir();
    QString startDir = fso::DirExists(oldDir) ? oldDir.c_str() : QDir::currentPath();

    QString filename = QFileDialog::getOpenFileName(
        this,
        tr("Open ACSII File"),
        startDir,
        tr("All files (*.*)") );
    if( filename.isNull() )
    {
       return;
    }

    setLastOpenSaveDir(filename.toStdString()); // Save the path the user specified.

    m_guiTabs.push_back( new BitViewerGuiTab( ui->tabWidget,
                                              m_guiPtrs,
                                              *mp_curGuiTab->getIoGuiTab()->getBitViewerData(),
                                              QString::fromStdString(fso::GetFileNameNoExt(filename.toStdString())) ) );
    ui->tabWidget->setCurrentIndex(m_guiTabs.size()-1); // Set to the newly created tab.

    std::string winFormatPath = QString::fromStdString(filename.toStdString()).replace("/", "\\").toStdString();
    ++m_readyToPrint;
    ui->txtInput->setText(QString::fromStdString(fso::ReadFile(winFormatPath)));
    ui->chkAsciiIn->setChecked(true);
    --m_readyToPrint;
    updateInput();
}

void MainWindow::WriteOutputToBinaryFile()
{
    if(mp_curGuiTab != NULL)
    {
        std::string oldDir = getLastOpenSaveDir();
        QString startDir = fso::DirExists(oldDir) ? oldDir.c_str() : QDir::currentPath();

        QString filename = QFileDialog::getSaveFileName(
            this,
            tr("Save Binary Path"),
            startDir,
            tr("All files (*.*)") );
        if( filename.isNull() )
        {
           return;
        }

        setLastOpenSaveDir(filename.toStdString()); // Save the path the user specified.

        ioData* t_outputData = mp_curGuiTab->getIoGuiTab()->getOutputData();
        std::string t_outFilePath = filename.toStdString();
        UINT_32 i_bytesPerOut = (ui->spnBitsPerOut->value() + 7) >> 3;
        UINT_32 i_outFileSizeBytes = i_bytesPerOut*(*t_outputData).size();
        char* pc_outFile = new char[i_bytesPerOut*(*t_outputData).size()];
        UINT_32 i_outIndex = 0;

        for( ioData::iterator i_ioOutIter = (*t_outputData).begin(); i_ioOutIter != (*t_outputData).end(); ++i_ioOutIter)
        {
           UINT_32 i_ioOutShift = 0;
           for( UINT_32 i_byteIndex = 0; i_byteIndex < i_bytesPerOut; ++i_byteIndex)
           {
              pc_outFile[i_outIndex++] = (*i_ioOutIter) >> i_ioOutShift;
              i_ioOutShift += BITS_PER_BYTE;
           }
        }
        fso::WriteFile(t_outFilePath, pc_outFile, i_outFileSizeBytes);
        delete[] pc_outFile;
    }
}

void MainWindow::WriteOutputToAsciiFile()
{
    std::string oldDir = getLastOpenSaveDir();
    QString startDir = fso::DirExists(oldDir) ? oldDir.c_str() : QDir::currentPath();

    QString filename = QFileDialog::getSaveFileName(
        this,
        tr("Save ASCII Path"),
        startDir,
        tr("All files (*.*)") );
    if( filename.isNull() )
    {
       return;
    }

    setLastOpenSaveDir(filename.toStdString()); // Save the path the user specified.

    fso::WriteFile(filename.toStdString(), ui->txtOutput->toPlainText().toStdString());

}

void MainWindow::WriteOutputToCsvFile()
{
}

void MainWindow::compareTabs_Finished(UINT_32 dst, UINT_32 src)
{
    if( (dst < m_guiTabs.size()) &&
        (src < m_guiTabs.size()) )
    {
        bitStreamCompare comp(*m_guiTabs[dst]->getIoGuiTab()->getBitViewerData(),
                              *m_guiTabs[src]->getIoGuiTab()->getBitViewerData());

        UINT_32 i_matchCount = comp.getMatchCount();
        UINT_32 i_matchErrors = comp.getMatchErrors();

        std::stringstream newTabName;

        newTabName << m_guiTabs[dst]->getTabName().toStdString() << " vs " <<
                      m_guiTabs[src]->getTabName().toStdString();

        ++m_ignorGuiChange;
        //++m_ignorTabChange;
        m_guiTabs.push_back( new BitViewerGuiTab( ui->tabWidget,
                                                  m_guiPtrs,
                                                  *comp.getBitViewerData(),
                                                  QString::fromStdString(newTabName.str()) ) );
        //--m_ignorTabChange;

        // Set the current tab to the newly created compare tab.
        ui->tabWidget->setCurrentIndex(m_guiTabs.size()-1);

        syncGuiTab();
        updateForce();

        std::stringstream infoText;
        infoText << "Overlapping Bits: " << (i_matchErrors + i_matchCount) <<
                    "\nSource Bits: " << comp.getNumSrcBits() <<
                    "\nError Bits: " << i_matchErrors <<
                    "\nMatch Percent: " << ((float)i_matchCount / (float)(i_matchErrors + i_matchCount) * 100.0);

        QMessageBox msgBox;
        msgBox.setWindowTitle("Compare Results");
        msgBox.setText(QString::fromStdString(infoText.str()));
        msgBox.exec();

        --m_ignorGuiChange;
    }
}



void MainWindow::on_txtInput_textChanged(const QString &arg1)
{
    (void)arg1;
}
void MainWindow::on_txtDelimiter_textChanged(const QString &arg1)
{
    (void)arg1;
    updateOutputOnChange(GUI_DELIMITER);
}


void MainWindow::on_chkAsciiIn_stateChanged(int arg1)
{
    (void)arg1;
    if(mp_curGuiTab != NULL && m_ignorTabChange == 0)
    {
        ++m_readyToPrint;
        if(ui->chkAsciiIn->isChecked() == true)
        {
            mp_curGuiTab->m_asciiSaveIn.i_bitsPer = ui->spnBitsPerIn->value();
            ui->spnBitsPerIn->setValue(8);

            mp_curGuiTab->m_asciiSaveIn.b_signed = ui->chkSignedIn->isChecked();
            ui->chkSignedIn->setChecked(false);
        }
        else
        {
            ui->spnBitsPerIn->setValue(mp_curGuiTab->m_asciiSaveIn.i_bitsPer);
            ui->chkSignedIn->setChecked(mp_curGuiTab->m_asciiSaveIn.b_signed);
        }
        --m_readyToPrint;
        updateOutputOnChange(GUI_IN_ASCII);
    }
    if(ui->chkAsciiIn->isChecked())
    {
        ui->chkByteReverseIn->setHidden(true);
        ui->spnBaseIn->setHidden(true);
        ui->lblBaseIn->setHidden(true);
        ui->spnBitsPerIn->setHidden(true);
        ui->lblBitsPerIn->setHidden(true);
        ui->chkSignedIn->setHidden(true);
        ui->chkBase64In->setHidden(true);
        ui->chkBase64In->setChecked(false); // Uncheck when hiding. This way the "stateChange" function will trigger when this is visible and checked.
    }
    else if(!ui->chkBase64In->isChecked())
    {
        ui->chkByteReverseIn->setHidden(false);
        ui->spnBaseIn->setHidden(false);
        ui->lblBaseIn->setHidden(false);
        ui->spnBitsPerIn->setHidden(false);
        ui->lblBitsPerIn->setHidden(false);
        ui->chkSignedIn->setHidden(false);
        ui->chkBase64In->setHidden(false);
    }
    else
    {
        ui->chkBase64In->setHidden(false);
    }
}

void MainWindow::on_chkAsciiOut_stateChanged(int arg1)
{
    (void)arg1;
    if(mp_curGuiTab != NULL && m_ignorTabChange == 0)
    {
        ++m_readyToPrint;
        if(ui->chkAsciiOut->isChecked() == true)
        {
            mp_curGuiTab->m_asciiSaveOut.i_bitsPer = ui->spnBitsPerOut->value();
            ui->spnBitsPerOut->setValue(8);

            mp_curGuiTab->m_asciiSaveOut.b_signed = ui->chkSignedOut->isChecked();
            ui->chkSignedOut->setChecked(false);
        }
        else
        {
            ui->spnBitsPerOut->setValue(mp_curGuiTab->m_asciiSaveOut.i_bitsPer);
            ui->chkSignedOut->setChecked(mp_curGuiTab->m_asciiSaveOut.b_signed);
        }
        --m_readyToPrint;
        updateOutputOnChange(GUI_OUT_ASCII);
    }
    if(ui->chkAsciiOut->isChecked())
    {
        ui->chkByteReverseOut->setHidden(true);
        ui->spnBaseOut->setHidden(true);
        ui->lblBaseOut->setHidden(true);
        ui->spnBitsPerOut->setHidden(true);
        ui->lblBitsPerOut->setHidden(true);
        ui->chkSignedOut->setHidden(true);
        ui->chkBase64Out->setHidden(true);
        ui->chkBase64Out->setChecked(false); // Uncheck when hiding. This way the "stateChange" function will trigger when this is visible and checked.
    }
    else if(!ui->chkBase64Out->isChecked())
    {
        ui->chkByteReverseOut->setHidden(false);
        ui->spnBaseOut->setHidden(false);
        ui->lblBaseOut->setHidden(false);
        ui->spnBitsPerOut->setHidden(false);
        ui->lblBitsPerOut->setHidden(false);
        ui->chkSignedOut->setHidden(false);
        ui->chkBase64Out->setHidden(false);
    }
    else
    {
        ui->chkBase64Out->setHidden(false);
    }
}

void MainWindow::on_chkBase64In_stateChanged(int arg1)
{
   (void)arg1;
   if(mp_curGuiTab != NULL && m_ignorTabChange == 0)
   {
      ++m_readyToPrint;
      if(ui->chkBase64In->isChecked() == true)
      {
         mp_curGuiTab->m_base64SaveIn.i_bitsPer = ui->spnBitsPerIn->value();
         ui->spnBitsPerIn->setValue(6);

         mp_curGuiTab->m_base64SaveIn.b_signed = ui->chkSignedIn->isChecked();
         ui->chkSignedIn->setChecked(false);
      }
      else
      {
         ui->spnBitsPerIn->setValue(mp_curGuiTab->m_base64SaveIn.i_bitsPer);
         ui->chkSignedIn->setChecked(mp_curGuiTab->m_base64SaveIn.b_signed);
      }
      --m_readyToPrint;
      updateOutputOnChange(GUI_IN_BASE64);
   }
   if(ui->chkBase64In->isChecked())
   {
      ui->chkByteReverseIn->setHidden(true);
      ui->spnBaseIn->setHidden(true);
      ui->lblBaseIn->setHidden(true);
      ui->spnBitsPerIn->setHidden(true);
      ui->lblBitsPerIn->setHidden(true);
      ui->chkSignedIn->setHidden(true);
      ui->chkAsciiIn->setHidden(true);
      ui->chkAsciiIn->setChecked(false); // Uncheck when hiding. This way the "stateChange" function will trigger when this is visible and checked.
   }
   else if(!ui->chkAsciiIn->isChecked())
   {
      ui->chkByteReverseIn->setHidden(false);
      ui->spnBaseIn->setHidden(false);
      ui->lblBaseIn->setHidden(false);
      ui->spnBitsPerIn->setHidden(false);
      ui->lblBitsPerIn->setHidden(false);
      ui->chkSignedIn->setHidden(false);
      ui->chkAsciiIn->setHidden(false);
   }
   else
   {
      ui->chkAsciiIn->setHidden(false);
   }
}

void MainWindow::on_chkBase64Out_stateChanged(int arg1)
{
   (void)arg1;
   if(mp_curGuiTab != NULL && m_ignorTabChange == 0)
   {
      ++m_readyToPrint;
      if(ui->chkBase64Out->isChecked() == true)
      {
         mp_curGuiTab->m_base64SaveOut.i_bitsPer = ui->spnBitsPerOut->value();
         ui->spnBitsPerOut->setValue(6);

         mp_curGuiTab->m_base64SaveOut.b_signed = ui->chkSignedOut->isChecked();
         ui->chkSignedOut->setChecked(false);
      }
      else
      {
         ui->spnBitsPerOut->setValue(mp_curGuiTab->m_base64SaveOut.i_bitsPer);
         ui->chkSignedOut->setChecked(mp_curGuiTab->m_base64SaveOut.b_signed);
      }
      --m_readyToPrint;
      updateOutputOnChange(GUI_OUT_BASE64);
   }
   if(ui->chkBase64Out->isChecked())
   {
      ui->chkByteReverseOut->setHidden(true);
      ui->spnBaseOut->setHidden(true);
      ui->lblBaseOut->setHidden(true);
      ui->spnBitsPerOut->setHidden(true);
      ui->lblBitsPerOut->setHidden(true);
      ui->chkSignedOut->setHidden(true);
      ui->chkAsciiOut->setHidden(true);
      ui->chkAsciiOut->setChecked(false); // Uncheck when hiding. This way the "stateChange" function will trigger when this is visible and checked.
   }
   else if(!ui->chkAsciiOut->isChecked())
   {
      ui->chkByteReverseOut->setHidden(false);
      ui->spnBaseOut->setHidden(false);
      ui->lblBaseOut->setHidden(false);
      ui->spnBitsPerOut->setHidden(false);
      ui->lblBitsPerOut->setHidden(false);
      ui->chkSignedOut->setHidden(false);
      ui->chkAsciiOut->setHidden(false);
   }
   else
   {
      ui->chkAsciiOut->setHidden(false);
   }
}


void MainWindow::on_spnBitsPerIn_valueChanged(int arg1)
{
    (void)arg1;
    if(mp_curGuiTab != NULL && m_ignorTabChange == 0)
    {
        int curVal = ui->spnBitsPerIn->value();

        if(ui->chkByteReverseIn->isChecked())
        {
            if(curVal < mp_curGuiTab->i_bitsPerInSave)
            {
                curVal &= (7 ^ -1);
            }
            else if(curVal > mp_curGuiTab->i_bitsPerInSave)
            {
                curVal = (mp_curGuiTab->i_bitsPerInSave + 8) & (7 ^ -1);
            }
            if(curVal < 8)
            {
                curVal = 8;
            }
            ++m_readyToPrint;
            ui->spnBitsPerIn->setValue(curVal);
            --m_readyToPrint;
        }
    }

    if(mp_curGuiTab != NULL)
    {
        mp_curGuiTab->i_bitsPerInSave = ui->spnBitsPerIn->value();
    }
    updateOutputOnChange(GUI_IN_BITS_PER);
}


void MainWindow::on_spnBitsPerOut_valueChanged(int arg1)
{
    (void)arg1;
    if(mp_curGuiTab != NULL && m_ignorTabChange == 0)
    {
        int curVal = ui->spnBitsPerOut->value();

        if(ui->chkByteReverseOut->isChecked())
        {
            if(curVal < mp_curGuiTab->i_bitsPerOutSave)
            {
                curVal &= (7 ^ -1);
            }
            else if(curVal > mp_curGuiTab->i_bitsPerOutSave)
            {
                curVal = (mp_curGuiTab->i_bitsPerOutSave + 8) & (7 ^ -1);
            }
            if(curVal < 8)
            {
                curVal = 8;
            }
            ++m_readyToPrint;
            ui->spnBitsPerOut->setValue(curVal);
            --m_readyToPrint;
        }
    }

    if(mp_curGuiTab != NULL)
    {
        mp_curGuiTab->i_bitsPerOutSave = ui->spnBitsPerOut->value();
    }
    updateOutputOnChange(GUI_OUT_BITS_PER);
}


void MainWindow::on_txtInput_editingFinished()
{
}

void MainWindow::on_chkDelimAuto_stateChanged(int arg1)
{
    (void)arg1;
    if(m_ignorTabChange == 0)
    {
        if(ui->chkDelimAuto->isChecked())
        {
            std::string t_autoDelim = getDelimiter(ui->txtInput->text().toStdString());
            ui->txtDelimiter->setText(QString::fromStdString(t_autoDelim));
        }
    }
    updateOutputOnChange(GUI_AUTO_DELIM);
}

void MainWindow::on_txtDelimiter_editingFinished()
{
    updateOutputOnChange(GUI_DELIMITER);
}

void MainWindow::on_spnBaseIn_editingFinished()
{
    updateOutputOnChange(GUI_IN_BASE);
}

void MainWindow::on_spnBitsPerIn_editingFinished()
{
    updateOutputOnChange(GUI_IN_BITS_PER);
}

void MainWindow::on_chkSignedIn_clicked()
{
    updateOutputOnChange(GUI_IN_SIGNED);
}

void MainWindow::on_chkByteReverseIn_clicked()
{
    if(m_ignorTabChange == 0)
    {
        if(ui->chkByteReverseIn->isChecked())
        {
            ++m_readyToPrint;
            // Round up to nearest integer number of bytes.
            ui->spnBitsPerIn->setValue((ui->spnBitsPerIn->value() + BITS_PER_BYTE_MASK) & -8);
            --m_readyToPrint;
        }
    }
    updateOutputOnChange(GUI_IN_BYTE_REV);
}

void MainWindow::on_chkBitReverseIn_clicked()
{
    updateOutputOnChange(GUI_IN_BIT_REV);
}

void MainWindow::on_spnBitShiftIn_editingFinished()
{
    updateOutputOnChange(GUI_IN_BIT_SHIFT);
}

void MainWindow::on_spnBaseOut_editingFinished()
{
    updateOutputOnChange(GUI_OUT_BASE);
}

void MainWindow::on_spnBitsPerOut_editingFinished()
{
    updateOutputOnChange(GUI_OUT_BITS_PER);
}

void MainWindow::on_chkSignedOut_clicked()
{
    updateOutputOnChange(GUI_OUT_SIGNED);
}

void MainWindow::on_chkByteReverseOut_clicked()
{
    if(m_ignorTabChange == 0)
    {
        if(ui->chkByteReverseOut->isChecked())
        {
            ++m_readyToPrint;
            // Round up to nearest integer number of bytes.
            ui->spnBitsPerOut->setValue((ui->spnBitsPerOut->value() + BITS_PER_BYTE_MASK) & -8);
            --m_readyToPrint;
        }
    }
    updateOutputOnChange(GUI_OUT_BYTE_REV);
}

void MainWindow::on_chkBitReverseOut_clicked()
{
    updateOutputOnChange(GUI_OUT_BIT_REV);
}

void MainWindow::on_spnBitShiftOut_editingFinished()
{
    updateOutputOnChange(GUI_OUT_BIT_SHIFT);
}

void MainWindow::on_chkInvert_clicked()
{
    updateOutputOnChange(GUI_BIT_INVERT);
}

void MainWindow::on_chkCArray_clicked()
{
    updateOutputOnChange(GUI_C_ARRAY);
}

void MainWindow::on_spnNumRows_editingFinished()
{
    updateOutputOnChange(GUI_NUM_ROWS);
}

void MainWindow::on_spnBaseIn_valueChanged(int arg1)
{
    (void)arg1;
    updateOutputOnChange(GUI_IN_BASE);
}

void MainWindow::on_spnBitShiftIn_valueChanged(int arg1)
{
    (void)arg1;
    updateOutputOnChange(GUI_IN_BIT_SHIFT);
}

void MainWindow::on_spnBaseOut_valueChanged(int arg1)
{
    (void)arg1;
    updateOutputOnChange(GUI_OUT_BASE);
}

void MainWindow::on_spnBitShiftOut_valueChanged(int arg1)
{
    (void)arg1;
    updateOutputOnChange(GUI_OUT_BIT_SHIFT);
}

void MainWindow::on_spnNumRows_valueChanged(int arg1)
{
    (void)arg1;
    updateOutputOnChange(GUI_NUM_ROWS);
}

void MainWindow::on_chkLineEndDelim_stateChanged(int arg1)
{
    (void)arg1;
    updateOutputOnChange(GUI_LINE_END_DELIM);
}


#if 0
// Future Middle interpretation
void MainWindow::on_cmdAddByteReverseIn_clicked()
{
    m_modifiersIn.append(mt_byteReverseStr);
    ((QStringListModel*)ui->listModIn->model())->setStringList(m_modifiersIn);
}

void MainWindow::on_cmdAddBitReverseIn_clicked()
{
    m_modifiersIn.append(mt_bitReverseStr);
    ((QStringListModel*)ui->listModIn->model())->setStringList(m_modifiersIn);
}

void MainWindow::on_cmdAddBitShiftIn_clicked()
{
    QString temp(mt_bitShiftStr);
    temp.append(intToAscii(ui->spnBitShiftModIn->value(), 10, 32, true).c_str());
    m_modifiersIn.append(temp);
    ((QStringListModel*)ui->listModIn->model())->setStringList(m_modifiersIn);
}

void MainWindow::on_cmdDeleteIn_clicked()
{
    m_modifiersIn.clear();
    ((QStringListModel*)ui->listModIn->model())->setStringList(m_modifiersIn);

}

void MainWindow::readModType(const QString& t_modStr, e_modType& e_mod, INT_32& i_shiftValue)
{
    if(dString::InStr(t_modStr.toStdString(), mt_byteReverseStr.toStdString()) >= 0)
    {
        e_mod = BYTE_REVERSE;
    }
    else if(dString::InStr(t_modStr.toStdString(), mt_bitReverseStr.toStdString()) >= 0)
    {
        e_mod = BIT_REVERSE;
    }
    else if(dString::InStr(t_modStr.toStdString(), mt_bitShiftStr.toStdString()) >= 0)
    {
        e_mod = BIT_SHIFT;
        i_shiftValue = asciiToInt(t_modStr.toStdString(), 10, true);
    }
    else
    {
        e_mod = MOD_INVALID;
    }
}
#endif

void MainWindow::DetermineInputType()
{
    updateInput();

    // Get Input Info.
    std::string inputStr = ui->txtInput->text().toStdString();
    QStringList inQstrList;
    mp_curGuiTab->getIoGuiTab()->getInput(true, inQstrList);
    auto numInValues = inQstrList.count();

    // Determine how many character are in the input text.
    int64_t maxNumCharsPerInput = 0;
    int64_t totalNumChars = 0;
    for(int i = 0; i < numInValues; ++i)
    {
        // Remove common characters that don't contribute to the number of bits per value.
        auto inVal = inQstrList[i];
        inVal = inVal.replace("0x", "");
        inVal = inVal.replace("0X", "");
        inVal = inVal.replace("-", "");

        int64_t charSize = inVal.size();
        maxNumCharsPerInput = std::max(maxNumCharsPerInput, charSize);
        totalNumChars += charSize;
    }

    // Determine Base
    bool hex_0x = (inputStr.find("0x") != std::string::npos) || (inputStr.find("0X") != std::string::npos);
    bool hex_chars = false;
    bool dec_chars = false;
    bool bin_chars = false;
    bool neg_chars = false;
    const char* inPtr = inputStr.c_str();
    const size_t inSize = inputStr.size();
    for(size_t i = 0; i < inSize; ++i)
    {
        char inChar = inPtr[i];
        if((inChar >= 'a') && (inChar <= 'f'))
            hex_chars = true;
        else if((inChar >= 'A') && (inChar <= 'F'))
            hex_chars = true;
        else if((inChar >= '2') && (inChar <= '9'))
            dec_chars = true;
        else if((inChar >= '0') && (inChar <= '1'))
            bin_chars = true;
        else if(inChar == '-')
            neg_chars = true;
    }

    int base = 0; // Init to invalid.
    if(hex_0x || hex_chars)
    {
        base = 16;
    }
    else if(bin_chars && !dec_chars && (totalNumChars > 10 || numInValues > 3)) // Don't auto set to base 2 unless there are plenty of only 1:0 characters. If not assume base 10
    {
        base = 2;
    }
    else if(dec_chars || bin_chars)
    {
        base = 10;
        ui->chkSignedIn->setChecked(neg_chars);
    }
    else
    {
        // Do Nothing
    }

    if(base != 0)
    {
        ui->spnBaseIn->setValue(base);

        double maxValPos = 0;
        double maxValNeg = 0;

        for(int i = 0; i < numInValues; ++i)
        {
            try
            {
                bool isNeg = false;
                double val = (double)asciiToUint(inQstrList[i].toStdString(), base, isNeg);
                if(isNeg)
                    maxValNeg = std::max(val, maxValNeg);
                else
                    maxValPos = std::max(val, maxValPos);
            }catch (...) {}
        }

        // Determine max number of bits needed
        int maxBitsPos = (maxValPos > 0) ? (floor(log2(maxValPos)) + 1) : 1; 
        int maxBitsNeg = (maxValNeg > 0) ? (ceil(log2(maxValNeg)) + 1) : 1;
        int numBitsPer = std::max(maxBitsPos, maxBitsNeg);
        if(numBitsPer > 64)
            numBitsPer = 64;

        // Next Determine Bits Per
        if(ui->chkAutoStdInt->isChecked())
        {
           if(numBitsPer > 32)
               numBitsPer = 64;
           else if(numBitsPer > 16)
               numBitsPer = 32;
           else if(numBitsPer > 8)
               numBitsPer = 16;
           else
               numBitsPer = 8;
        }
        if(numBitsPer > 0)
            ui->spnBitsPerIn->setValue(numBitsPer);
    }

}

void MainWindow::on_cmdDetectInputFormat_clicked()
{
    DetermineInputType();
}


void MainWindow::on_chkAutoStdInt_stateChanged(int arg1)
{
   (void)arg1;
   updateOutputOnChange(GUI_AUTO_IN_STDINT);
   DetermineInputType();
}

void MainWindow::on_cmdMatchIn_clicked()
{
   ui->spnBaseOut->setValue(ui->spnBaseIn->value());
   ui->spnBitsPerOut->setValue(ui->spnBitsPerIn->value());
   ui->chkBase64Out->setChecked(ui->chkBase64In->isChecked());
   ui->chkAsciiOut->setChecked(ui->chkAsciiIn->isChecked());
   ui->chkSignedOut->setChecked(ui->chkSignedIn->isChecked());
   updateForce();
}
