/*******************************************************************
 **  $ CopasiUI/MoietyWidget.cpp                 
 **  $ Author  : Mudita Singhal
 **
 ** This file is used to create the GUI FrontPage for the  information 
 ** obtained from the data model about the Moiety----It is Basically 
 ** the First level of Moieties.
 ********************************************************************/

#include <qlayout.h>
#include <qwidget.h>
#include <qmessagebox.h>
#include "MoietyWidget.h"
#include "model/CMetab.h"
#include "listviews.h"
#include "model/CMoiety.h"
#include <qfont.h>

/**
 *  Constructs a Widget for the Moiety subsection of the tree.
 *  This widget is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'. 
 *  @param model The CModel class which contains the moiety 
 *  to be displayed.
 *  @param parent The widget which this widget is a child of.
 *  @param name The object name is a text that can be used to identify 
 *  this QObject. It's particularly useful in conjunction with the Qt Designer.
 *  You can find an object by name (and type) using child(), and more than one 
 *  using queryList(). 
 *  @param flags Flags for this widget. Redfer Qt::WidgetFlags of Qt documentation 
 *  for more information about these flags.
 */
MoietyWidget::MoietyWidget(QWidget *parent, const char * name, WFlags f)
    : QWidget(parent, name, f)
{
  mModel = NULL;
  table = new MyTable(0, 3, this, "tblMoieties");
  QVBoxLayout *vBoxLayout = new QVBoxLayout(this, 0);
  vBoxLayout->addWidget(table);

  //Setting table headers
  QHeader *tableHeader = table->horizontalHeader();
  tableHeader->setLabel(0, "Name");
  tableHeader->setLabel(1, "Number");
  tableHeader->setLabel(2, "Equation");

  btnOK = new QPushButton("&OK", this);
  btnCancel = new QPushButton("&Cancel", this);

  btnOK->setFont(QFont("Times", 10, QFont::Bold));
  btnCancel->setFont(QFont("Times", 10, QFont::Bold));

  QHBoxLayout *hBoxLayout = new QHBoxLayout(vBoxLayout, 0);

  //To match the Table left Vertical Header Column Width.
  hBoxLayout->addSpacing(32);

  hBoxLayout->addSpacing(50);
  hBoxLayout->addWidget(btnOK);
  hBoxLayout->addSpacing(5);
  hBoxLayout->addWidget(btnCancel);
  hBoxLayout->addSpacing(50);

  table->sortColumn (0, TRUE, TRUE);
  table->setSorting (TRUE);
  table->setFocusPolicy(QWidget::WheelFocus);

  // signals and slots connections
  connect(table, SIGNAL(doubleClicked(int, int, int, const QPoint &)), this, SLOT(slotTableCurrentChanged(int, int, int, const QPoint &)));
  connect(this, SIGNAL(name(QString &)), (ListViews*)parent, SLOT(slotMoietyTableChanged(QString &)));
  connect(table, SIGNAL(selectionChanged ()), this, SLOT(slotTableSelectionChanged ()));
  connect(btnOK, SIGNAL(clicked ()), this, SLOT(slotBtnOKClicked()));
  connect(btnCancel, SIGNAL(clicked ()), this, SLOT(slotBtnCancelClicked()));
}

void MoietyWidget::loadMoieties(CModel *model)
{
  if (model != NULL)
    {
      mModel = model;
      //Emptying the table
      int numberOfRows = table->numRows();

      for (int i = 0; i < numberOfRows; i++)
        {
          table->removeRow(0);
        }

      CCopasiVectorN < CMoiety > &moieties = mModel->getMoieties();

      C_INT32 noOfMoietyRows = moieties.size();
      table->setNumRows (noOfMoietyRows);

      //Now filling the table.
      CMoiety *moiety;
      //CEquation *eqn;

      for (C_INT32 j = 0; j < noOfMoietyRows; j++)
        {
          moiety = moieties[j];
          table->setText(j, 0, moiety->getName().c_str());
          table->setText(j, 1, QString::number(moiety->getNumber()));
          table->setText(j, 2, moiety->getDescription().c_str());
          //table->setText(j, 2,QString::number(moiety->dependentNumber()));
        }
    }
}

void MoietyWidget::slotBtnOKClicked()
{
  QMessageBox::information(this, "Moiety Widget",
                           "Clicked Ok button On Moiety widget.(Inside MoietyWidget::slotBtnOKClicked())");
}

void MoietyWidget::slotBtnCancelClicked()
{
  QMessageBox::information(this, "Moiety Widget",
                           "Clicked Ok button On Moiety widget.(Inside MoietyWidget::slotBtnCancelClicked())");
}

void MoietyWidget::slotTableSelectionChanged()
{
  if (!table->hasFocus())
    {
      table->setFocus();
    }
}

void MoietyWidget::resizeEvent(QResizeEvent * re)
{
  if (isVisible())
    {
      int newWidth = re->size().width();

      newWidth -= 35; //Accounting for the left (vertical) header width.
      float weight0 = 1.0, weight1 = 1.0, weight2 = 6.0;
      float weightSum = weight0 + weight1 + weight2;
      int w0, w1, w2;
      w0 = newWidth * (weight0 / weightSum);
      w1 = newWidth * (weight1 / weightSum);
      w2 = newWidth - w0 - w1 ;
      table->setColumnWidth(0, w0);
      table->setColumnWidth(1, w1);
      table->setColumnWidth(2, w2);
    }
}

void MoietyWidget::slotTableCurrentChanged(int row, int col, int m , const QPoint & n)
{
  QString x = table->text(row, col);
  emit name(x);
  //QMessageBox::information(this, "Compartments Widget",x);
}

/***********ListViews::showMessage(QString caption,QString text)------------------------>
 **
 ** Parameters:- 1. QString :- The Title that needs to be displayed in message box
 **              2. QString :_ The Text that needs to be displayed in the message box
 ** Returns  :-  void(Nothing)
 ** Description:- This method is used to show the message box on the screen
 ****************************************************************************************/

void MoietyWidget::showMessage(QString title, QString text)
{
  QMessageBox::about (this, title, text);
}
