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
#include "tabcomparedialog.h"
#include "ui_tabcomparedialog.h"

#include "mainwindow.h"

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
    m_parentUi = p_parentUi;
    ui->cmbCompareDst->clear();
    for(std::vector<BitViewerGuiTab*>::iterator iter = bitViewerTabs.begin(); iter != bitViewerTabs.end(); ++iter)
    {
        ui->cmbCompareDst->addItem((*iter)->getTabName());
    }

    ui->cmbCompareSrc->clear();
    for(std::vector<BitViewerGuiTab*>::iterator iter = bitViewerTabs.begin(); iter != bitViewerTabs.end(); ++iter)
    {
        ui->cmbCompareSrc->addItem((*iter)->getTabName());
    }

    this->show();
}

void TabCompareDialog::on_buttonBox_accepted()
{
    if(m_parentUi != NULL)
    {
        m_parentUi->compareTabs_Finished(ui->cmbCompareDst->currentIndex(), ui->cmbCompareSrc->currentIndex());
    }
    this->hide();
    m_parentUi = NULL;
}

void TabCompareDialog::on_buttonBox_rejected()
{
    this->hide();
    m_parentUi = NULL;
}
