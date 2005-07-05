/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/CopasiUI/Attic/CQProgressItemText.h,v $
   $Revision: 1.3 $
   $Name:  $
   $Author: shoops $ 
   $Date: 2005/07/05 21:05:42 $
   End CVS Header */

/****************************************************************************
 ** Form interface generated from reading ui file 'CQProgressItemText.ui'
 **
 ** Created: Tue Jul 5 17:04:25 2005
 **      by: The User Interface Compiler ($Id: CQProgressItemText.h,v 1.3 2005/07/05 21:05:42 shoops Exp $)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/

#ifndef CQPROGRESSITEMTEXT_H
#define CQPROGRESSITEMTEXT_H

#include <qvariant.h>
#include <qpixmap.h>
#include "CQProgressItem.h"
#include "utilities/CVector.h"
#include "utilities/CProcessReport.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QLineEdit;

class CQProgressItemText : public CQProgressItem
  {
    Q_OBJECT

  public:
    CQProgressItemText(QWidget* parent = 0, const char* name = 0);
    ~CQProgressItemText();

    QLabel* mItemName;
    QLineEdit* mValue;

    virtual bool initFromProcessReportItem(CProcessReportItem * pItem);
    virtual bool process();
    virtual bool reset();

  protected:
    QHBoxLayout* mLabel;
    QSpacerItem* mSpacer;

  protected slots:
    virtual void languageChange();

  private:
    void (CQProgressItemText::*mpSetValue)();
    CCopasiParameter::Value mParameterValue;

    QPixmap image0;

    virtual void setValueFromDOUBLE();
    virtual void setValueFromINT();
    virtual void setValueFromUINT();
    virtual void setValueFromSTRING();
  };

#endif // CQPROGRESSITEMTEXT_H
