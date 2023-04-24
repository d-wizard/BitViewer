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
#include "tabcomparedialog.h"
#include "ui_tabcomparedialog.h"
#include "mainwindow.h"
#include "BitViewerIni.h"
#include "FileSystemOperations.h"

#define INI_FILE_PATH "BitViewer.ini"

TabCompareDialog::TabCompareDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TabCompareDialog)
{
    ui->setupUi(this);
    m_parentUi = NULL;
}

TabCompareDialog::~TabCompareDialog()
{
    delete ui;
}

void TabCompareDialog::showTabCompare(std::vector<BitViewerGuiTab*>& bitViewerTabs, MainWindow* p_parentUi)
{
    // Try to get the previous compare tabs
    std::string iniFile = fso::ReadFile(INI_FILE_PATH);
    QString dstFromIni(readIniStr("TabCompareDst", iniFile).c_str());
    QString srcFromIni(readIniStr("TabCompareSrc", iniFile).c_str());

    int index = 0;
    int dstTabToUse = -1;
    int srcTabToUse = -1;

    m_parentUi = p_parentUi;
    ui->cmbCompareDst->clear();
    ui->cmbCompareSrc->clear();

    index = 0;
    for(std::vector<BitViewerGuiTab*>::iterator iter = bitViewerTabs.begin(); iter != bitViewerTabs.end(); ++iter)
    {
        QString curTabName = (*iter)->getTabName();
        ui->cmbCompareDst->addItem(curTabName);
        if(dstTabToUse < 0 || dstFromIni == curTabName)
        {
            dstTabToUse = index;
        }
        ++index;
    }

    index = 0;
    for(std::vector<BitViewerGuiTab*>::iterator iter = bitViewerTabs.begin(); iter != bitViewerTabs.end(); ++iter)
    {
        QString curTabName = (*iter)->getTabName();
        ui->cmbCompareSrc->addItem(curTabName);

        // No point in comparing the same 2 tabs, so don't auto fill with the same as dst.
        if((srcTabToUse < 0 && dstTabToUse != index) || srcFromIni == curTabName)
        {
            srcTabToUse = index;
        }
        ++index;
    }

    // Got to fill with something, so make it match dst if src was never filled in.
    if(srcTabToUse < 0)
    {
        srcTabToUse = dstTabToUse;
    }

    if(dstTabToUse >= 0)
       ui->cmbCompareDst->setCurrentIndex(dstTabToUse);
    if(srcTabToUse >= 0)
       ui->cmbCompareSrc->setCurrentIndex(srcTabToUse);

    this->show();
}

void TabCompareDialog::on_buttonBox_accepted()
{
    if(m_parentUi != NULL)
    {
        m_parentUi->compareTabs_Finished(ui->cmbCompareDst->currentIndex(), ui->cmbCompareSrc->currentIndex());
    }

    // Write the current tabs to the ini file.
    std::string iniFile = fso::ReadFile(INI_FILE_PATH);
    std::string dst = ui->cmbCompareDst->currentText().toStdString();
    std::string src = ui->cmbCompareSrc->currentText().toStdString();
    writeIniStr("TabCompareDst", dst, iniFile);
    writeIniStr("TabCompareSrc", src, iniFile);
    fso::WriteFile(INI_FILE_PATH, iniFile);

    this->hide();
    m_parentUi = NULL;
}

void TabCompareDialog::on_buttonBox_rejected()
{
    this->hide();
    m_parentUi = NULL;
}
