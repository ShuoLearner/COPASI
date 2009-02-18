// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/Attic/CQFileDialogBtnGrp.h,v $
//   $Revision: 1.4 $
//   $Name:  $
//   $Author: gauges $
//   $Date: 2009/02/18 20:46:37 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#ifndef CQFILEDIALOGBTNGRP_H
#define CQFILEDIALOGBTNGRP_H

#include <qvariant.h>

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <Qt3Support/Q3ButtonGroup>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <Qt3Support/Q3VBoxLayout>
#include <QtGui/QWidget>
//Added by qt3to4:
#include <QPixmap>
#include <Q3VBoxLayout>

class Ui_CQFileDialogBtnGrp
  {
  public:
    Q3VBoxLayout *vboxLayout;
    QToolButton *mpBtnHome;
    QToolButton *mpBtnExamples;
    QSpacerItem *mpSpacer;

    void setupUi(QWidget *CQFileDialogBtnGrp)
    {
      if (CQFileDialogBtnGrp->objectName().isEmpty())
        CQFileDialogBtnGrp->setObjectName(QString::fromUtf8("CQFileDialogBtnGrp"));
      CQFileDialogBtnGrp->resize(120, 289);
      vboxLayout = new Q3VBoxLayout(CQFileDialogBtnGrp);
      vboxLayout->setSpacing(6);
      vboxLayout->setMargin(11);
      vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
      mpBtnHome = new QToolButton(CQFileDialogBtnGrp);
      mpBtnHome->setObjectName(QString::fromUtf8("mpBtnHome"));
      QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(1), static_cast<QSizePolicy::Policy>(1));
      sizePolicy.setHorizontalStretch(0);
      sizePolicy.setVerticalStretch(0);
      sizePolicy.setHeightForWidth(mpBtnHome->sizePolicy().hasHeightForWidth());
      mpBtnHome->setSizePolicy(sizePolicy);
      const QIcon icon = qt_get_icon(image0_ID);
      mpBtnHome->setIcon(icon);

      vboxLayout->addWidget(mpBtnHome);

      mpBtnExamples = new QToolButton(CQFileDialogBtnGrp);
      mpBtnExamples->setObjectName(QString::fromUtf8("mpBtnExamples"));
      sizePolicy.setHeightForWidth(mpBtnExamples->sizePolicy().hasHeightForWidth());
      mpBtnExamples->setSizePolicy(sizePolicy);
      const QIcon icon1 = qt_get_icon(image1_ID);
      mpBtnExamples->setIcon(icon1);

      vboxLayout->addWidget(mpBtnExamples);

      mpSpacer = new QSpacerItem(10, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

      vboxLayout->addItem(mpSpacer);

      retranslateUi(CQFileDialogBtnGrp);

      QMetaObject::connectSlotsByName(CQFileDialogBtnGrp);
    } // setupUi

    void retranslateUi(QWidget *CQFileDialogBtnGrp)
    {
      CQFileDialogBtnGrp->setWindowTitle(QApplication::translate("CQFileDialogBtnGrp", "Form1", 0, QApplication::UnicodeUTF8));
      mpBtnHome->setText(QString());
      mpBtnHome->setText(QApplication::translate("CQFileDialogBtnGrp", "Home", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
      mpBtnHome->setProperty("toolTip", QVariant(QApplication::translate("CQFileDialogBtnGrp", "Home Directory", 0, QApplication::UnicodeUTF8)));
#endif // QT_NO_TOOLTIP

      mpBtnExamples->setText(QString());
      mpBtnExamples->setText(QApplication::translate("CQFileDialogBtnGrp", "Examples", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
      mpBtnExamples->setProperty("toolTip", QVariant(QApplication::translate("CQFileDialogBtnGrp", "COPASI Examples", 0, QApplication::UnicodeUTF8)));
#endif // QT_NO_TOOLTIP

      Q_UNUSED(CQFileDialogBtnGrp);
    } // retranslateUi

  protected:
    enum IconID
    {
      image0_ID,
      image1_ID,
      unknown_ID
    };
    static QPixmap qt_get_icon(IconID id)
    {
      static const unsigned char image0_data[] =
        {
          0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
          0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
          0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x04,
          0x27, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xad, 0x95, 0x5d, 0x6c, 0x53,
          0x65, 0x18, 0xc7, 0x7f, 0xef, 0xdb, 0xd3, 0x6e, 0xfd, 0x1c, 0xa5, 0x85,
          0xee, 0xc3, 0x02, 0x2b, 0xfb, 0x90, 0x32, 0x1d, 0xd0, 0x64, 0x6e, 0x2b,
          0x43, 0x64, 0xd0, 0x31, 0xe5, 0x63, 0x84, 0x6d, 0x82, 0x20, 0xba, 0x00,
          0x0a, 0x78, 0x67, 0x62, 0x8c, 0x89, 0x89, 0x37, 0x84, 0xe8, 0xa5, 0x09,
          0x22, 0x86, 0x10, 0xa3, 0x31, 0x8b, 0xf1, 0xca, 0xc5, 0x69, 0x10, 0x8d,
          0x89, 0xc1, 0x0d, 0x12, 0x09, 0x9b, 0x6e, 0x8e, 0x10, 0x30, 0x1b, 0x5b,
          0x19, 0x63, 0x85, 0xae, 0x8c, 0x15, 0xba, 0xaf, 0x73, 0xbc, 0xe8, 0x69,
          0xd7, 0x85, 0x71, 0xa1, 0xf1, 0x9f, 0xfc, 0x73, 0xf2, 0x9c, 0xf7, 0xc9,
          0xef, 0x79, 0x3f, 0x9e, 0xbc, 0x2f, 0x9a, 0xa6, 0x91, 0x32, 0x50, 0x6d,
          0xf3, 0xba, 0xbf, 0x01, 0x6a, 0x00, 0x03, 0x20, 0x32, 0xc7, 0xff, 0x8d,
          0x25, 0xba, 0x84, 0x10, 0x02, 0xa8, 0x2b, 0x79, 0x79, 0x23, 0x40, 0x08,
          0xc8, 0x06, 0x14, 0xfd, 0x3f, 0x19, 0x79, 0x75, 0x42, 0x88, 0x2c, 0xa1,
          0x8b, 0x27, 0x48, 0x66, 0x40, 0x0f, 0x05, 0x77, 0xd7, 0xad, 0xca, 0x2b,
          0x5a, 0x01, 0xeb, 0xbc, 0xa5, 0xc0, 0x61, 0xc0, 0x02, 0x48, 0x31, 0xa7,
          0x63, 0x9b, 0xfc, 0xde, 0x73, 0x26, 0x49, 0x02, 0x70, 0x00, 0x06, 0x21,
          0xc4, 0xa7, 0x42, 0x08, 0x4d, 0xf7, 0x19, 0xbd, 0xa8, 0x4c, 0xcd, 0x78,
          0x63, 0x65, 0xc3, 0x96, 0x2d, 0xeb, 0x0f, 0xec, 0x60, 0x72, 0x76, 0x0a,
          0x36, 0x15, 0x43, 0xc0, 0x5b, 0x0d, 0x1c, 0xd4, 0xe1, 0x06, 0xe0, 0xf4,
          0x6b, 0x9b, 0x2b, 0x3e, 0xd9, 0xfb, 0xac, 0x9b, 0x15, 0x76, 0x00, 0x76,
          0x02, 0x56, 0xe0, 0xc8, 0xaf, 0x7b, 0xf6, 0xd0, 0x9a, 0xe4, 0x1c, 0xd2,
          0x0b, 0x1a, 0xa5, 0x10, 0x62, 0xb5, 0x67, 0xcd, 0xca, 0xa3, 0x53, 0x45,
          0x4e, 0x3a, 0x6e, 0xf6, 0x72, 0x69, 0xe4, 0x06, 0x8c, 0x3d, 0x84, 0xb2,
          0x3c, 0xf0, 0xe7, 0x56, 0x02, 0xb5, 0xc0, 0xe9, 0x96, 0x17, 0xd7, 0xbf,
          0xf1, 0x6a, 0xf9, 0x52, 0x2c, 0xf7, 0xfe, 0xa2, 0xb9, 0x08, 0xca, 0x5d,
          0x7c, 0x01, 0x9c, 0x01, 0x78, 0x18, 0x89, 0x30, 0x30, 0xb7, 0x0b, 0x4b,
          0x00, 0x93, 0x04, 0xfa, 0xee, 0x74, 0xff, 0x7d, 0xb2, 0xaf, 0xf5, 0x27,
          0x1e, 0x0d, 0xde, 0xc3, 0x24, 0x0c, 0xf0, 0x68, 0x1a, 0xfe, 0x08, 0xc3,
          0xad, 0x18, 0xc0, 0x81, 0x96, 0xfa, 0xe0, 0xc1, 0xfd, 0x65, 0x2e, 0xae,
          0xff, 0xde, 0xce, 0xe5, 0x91, 0x29, 0xd6, 0x16, 0x15, 0x73, 0xb4, 0xdc,
          0x42, 0x60, 0x09, 0x4d, 0x00, 0xd3, 0xaa, 0x8a, 0x36, 0x07, 0x76, 0xa4,
          0xc0, 0x00, 0xd8, 0x14, 0x33, 0x1b, 0xaa, 0xaa, 0x58, 0x97, 0xeb, 0x03,
          0xb7, 0x15, 0xdc, 0x36, 0x98, 0x98, 0x0a, 0xb4, 0xd4, 0x07, 0x77, 0xed,
          0xf3, 0x3b, 0xe9, 0xba, 0xd0, 0x46, 0xfb, 0x00, 0xb4, 0x0d, 0xc0, 0xad,
          0x48, 0x04, 0xb7, 0xdd, 0x46, 0xe8, 0x29, 0xc8, 0xb3, 0xc0, 0xf9, 0xe1,
          0x61, 0x26, 0xe7, 0xc0, 0x66, 0x40, 0x4a, 0x2d, 0xd9, 0x67, 0xb5, 0xd5,
          0x87, 0x77, 0x32, 0x32, 0x11, 0x05, 0x4d, 0x23, 0xdb, 0x66, 0xc6, 0x70,
          0xf7, 0x51, 0xa0, 0x25, 0x54, 0xe5, 0xdb, 0xe7, 0x77, 0xd2, 0xdd, 0xd9,
          0xce, 0x77, 0x03, 0xa0, 0x16, 0x57, 0x12, 0x8e, 0xc3, 0xd7, 0x7d, 0x31,
          0xae, 0xdd, 0x1e, 0xa5, 0xdc, 0x2d, 0x79, 0x67, 0x8d, 0xa4, 0x7b, 0xfc,
          0x1a, 0x1f, 0xa6, 0xa7, 0x98, 0x6c, 0x88, 0x54, 0x78, 0xf5, 0xb7, 0x4b,
          0x17, 0xf9, 0xa5, 0xbf, 0x87, 0x91, 0x89, 0x28, 0x89, 0xef, 0x7b, 0x03,
          0x07, 0xf2, 0x96, 0xcd, 0x83, 0x5a, 0xfc, 0x95, 0xbc, 0xb4, 0xe5, 0x39,
          0x7e, 0xfe, 0xf6, 0x63, 0x1a, 0x8e, 0x1c, 0xe5, 0x74, 0x1f, 0xdc, 0x18,
          0x07, 0x57, 0x96, 0x4a, 0xa8, 0x00, 0xbc, 0xf6, 0x34, 0x38, 0xd9, 0xa6,
          0x9a, 0xa6, 0x21, 0x84, 0x70, 0x01, 0xaf, 0x14, 0xee, 0x0e, 0xd6, 0x24,
          0x86, 0xc7, 0x02, 0x5b, 0x17, 0x39, 0x1f, 0x83, 0x6e, 0x0b, 0x55, 0x52,
          0x5d, 0xb1, 0x1a, 0xcf, 0x12, 0x27, 0x26, 0xa3, 0x91, 0xf6, 0xf3, 0x9d,
          0x9c, 0x39, 0xf1, 0x11, 0xcd, 0x85, 0x60, 0x54, 0x60, 0x24, 0x0e, 0x3f,
          0x0c, 0x29, 0x5c, 0x19, 0x9d, 0x41, 0x85, 0x62, 0x45, 0xaf, 0xf2, 0x34,
          0xe0, 0x51, 0xc2, 0xe3, 0x81, 0x90, 0xdd, 0xe6, 0xdb, 0x5b, 0x6a, 0x79,
          0x0c, 0x1a, 0xac, 0x28, 0x23, 0x3f, 0xd7, 0x85, 0xc3, 0x6e, 0xc5, 0x20,
          0x05, 0xbb, 0xb7, 0x6d, 0xe0, 0x66, 0x78, 0x94, 0x53, 0x9f, 0x7d, 0x4e,
          0xf3, 0x4a, 0xf0, 0xbb, 0xb2, 0x59, 0x6a, 0x4e, 0x90, 0x98, 0x81, 0xde,
          0x28, 0xfb, 0x53, 0xe0, 0xd0, 0xf3, 0xaf, 0xef, 0xaa, 0xa1, 0xf3, 0xa2,
          0x6f, 0x7b, 0xae, 0x4a, 0xd7, 0xa5, 0x51, 0xce, 0x0d, 0x82, 0xfd, 0x99,
          0x6a, 0xea, 0x6b, 0x2b, 0xd2, 0xd0, 0x1c, 0x87, 0x15, 0xa3, 0xa2, 0x20,
          0x84, 0xc0, 0x22, 0x25, 0x6f, 0x1f, 0x69, 0xa4, 0xd8, 0x57, 0xc0, 0x89,
          0xf7, 0x8e, 0x63, 0x34, 0x24, 0xc8, 0x92, 0x10, 0xcc, 0x85, 0xde, 0x28,
          0xcd, 0x8a, 0x10, 0x62, 0x75, 0x61, 0x45, 0xd9, 0x2a, 0x77, 0x89, 0xf7,
          0xce, 0x85, 0xab, 0x7d, 0xfd, 0x8d, 0x6d, 0xd7, 0x0a, 0x01, 0x9a, 0x1a,
          0x36, 0xb3, 0xf5, 0x85, 0xb5, 0x54, 0xac, 0x59, 0x85, 0x67, 0xa9, 0x93,
          0x1c, 0x87, 0x95, 0xf0, 0x70, 0x84, 0xd8, 0xfd, 0x09, 0x54, 0x55, 0xc5,
          0xb7, 0xbc, 0x00, 0x87, 0xdd, 0xcc, 0xf6, 0x50, 0x15, 0x77, 0x22, 0x6f,
          0x71, 0xea, 0x6c, 0x1b, 0xd7, 0xfb, 0xc3, 0x00, 0x83, 0x40, 0xbb, 0x02,
          0xcc, 0x3c, 0x18, 0x89, 0x52, 0x50, 0xb4, 0x9c, 0xa2, 0xc6, 0x0d, 0x57,
          0x46, 0x4b, 0xed, 0x3d, 0x7c, 0x79, 0x79, 0xc7, 0xf1, 0xf7, 0x8f, 0x61,
          0x92, 0x93, 0x38, 0xac, 0x56, 0x6c, 0xb6, 0x6c, 0x8c, 0x8a, 0x42, 0xec,
          0xfe, 0x04, 0x81, 0xcd, 0x6f, 0xa6, 0x4f, 0x49, 0xbb, 0x7b, 0x01, 0x8b,
          0x94, 0x1c, 0xde, 0xbf, 0x0d, 0x7f, 0xc9, 0x32, 0xea, 0x9a, 0xdf, 0x05,
          0xf8, 0x00, 0xf8, 0x53, 0x02, 0xa1, 0x40, 0xe3, 0x26, 0xfa, 0xc7, 0x6e,
          0xd3, 0x3d, 0x36, 0xa4, 0x61, 0x36, 0xce, 0x00, 0xe4, 0x7b, 0xdc, 0x78,
          0xdc, 0x4e, 0x72, 0x1c, 0x96, 0xf4, 0xf2, 0x55, 0x55, 0x05, 0xe0, 0xab,
          0x45, 0x79, 0x7a, 0x63, 0x99, 0x91, 0x52, 0x62, 0x36, 0x67, 0xe1, 0xb0,
          0x59, 0x53, 0xf5, 0x22, 0x40, 0x54, 0xb1, 0xe6, 0x2f, 0xae, 0xe9, 0xb8,
          0xdf, 0xcf, 0x44, 0xdf, 0x10, 0x3c, 0x98, 0xd4, 0x88, 0x4f, 0xcd, 0x02,
          0xd8, 0xac, 0x16, 0x98, 0x99, 0x65, 0x21, 0xe5, 0xc7, 0x1e, 0xce, 0x8b,
          0xa5, 0x10, 0x28, 0x8a, 0x21, 0x15, 0xc6, 0x81, 0xb8, 0x12, 0x1f, 0x8e,
          0x9e, 0xe4, 0x6c, 0x47, 0x1d, 0x4e, 0x6b, 0x31, 0x85, 0x8b, 0x61, 0x7a,
          0x61, 0x58, 0xa6, 0x9e, 0x78, 0x57, 0x66, 0x48, 0x01, 0x3a, 0x81, 0x2e,
          0xc6, 0xe2, 0x3e, 0xc6, 0xe2, 0xeb, 0x01, 0x13, 0xd0, 0x34, 0x3b, 0x19,
          0xa3, 0xab, 0xe7, 0xfa, 0xbc, 0xe4, 0xd8, 0x78, 0x1c, 0x80, 0x21, 0x92,
          0x5b, 0x92, 0x99, 0x93, 0x1a, 0x03, 0x12, 0x90, 0x4c, 0x10, 0x80, 0x91,
          0xe4, 0x15, 0xe8, 0x04, 0x3c, 0x40, 0x2b, 0xa0, 0xfd, 0x07, 0xff, 0x08,
          0xf8, 0x01, 0xbb, 0xd0, 0x9f, 0xa4, 0xd4, 0x65, 0x2f, 0x75, 0x67, 0x03,
          0x76, 0xc0, 0x05, 0xd8, 0xf4, 0x58, 0x03, 0x66, 0xf5, 0xef, 0x42, 0xbb,
          0x33, 0x0d, 0x8c, 0x93, 0x3c, 0xbc, 0xf1, 0x34, 0x78, 0x5e, 0x96, 0x10,
          0x52, 0x5f, 0x85, 0x49, 0xb7, 0xcc, 0x18, 0x7e, 0x12, 0x58, 0x05, 0xa6,
          0x74, 0x4f, 0x2f, 0x08, 0xfe, 0x3f, 0xf4, 0x0f, 0x78, 0x7e, 0x9d, 0xc5,
          0x17, 0x23, 0x8b, 0xa9, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
          0xae, 0x42, 0x60, 0x82
        };

      static const unsigned char image1_data[] =
        {
          0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
          0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20,
          0x08, 0x06, 0x00, 0x00, 0x00, 0x73, 0x7a, 0x7a, 0xf4, 0x00, 0x00, 0x06,
          0x30, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xc5, 0x97, 0x5d, 0x4c, 0x54,
          0x67, 0x1a, 0xc7, 0x7f, 0x67, 0x18, 0x64, 0xe8, 0x0c, 0xc3, 0x00, 0x0a,
          0x52, 0x0b, 0xc8, 0xc8, 0x88, 0x7c, 0x75, 0xb0, 0x48, 0x0f, 0xc6, 0x82,
          0xa6, 0x04, 0x58, 0xcc, 0x2e, 0xf5, 0x03, 0x4c, 0x76, 0x5d, 0xd9, 0x5d,
          0xbb, 0x17, 0x9b, 0x26, 0xdb, 0xec, 0xba, 0xd1, 0x0d, 0x17, 0xb5, 0xed,
          0x85, 0x98, 0xb5, 0xc6, 0x78, 0x55, 0x4d, 0xba, 0xad, 0x44, 0x7a, 0xd3,
          0xea, 0x2a, 0x09, 0x8d, 0x74, 0xc1, 0x44, 0x50, 0xd7, 0x30, 0x50, 0x98,
          0x61, 0x94, 0x2f, 0x47, 0x25, 0x80, 0x5a, 0x3e, 0x87, 0x61, 0x98, 0x81,
          0x61, 0x66, 0xe0, 0xdd, 0x0b, 0x2d, 0xbb, 0x58, 0xe4, 0xc3, 0xdd, 0xc6,
          0xe7, 0xee, 0x9c, 0x93, 0xf3, 0xfc, 0x7f, 0xef, 0xfb, 0xbc, 0xe7, 0x79,
          0xfe, 0x47, 0xba, 0x75, 0xeb, 0x16, 0x2f, 0x33, 0x14, 0x2f, 0x55, 0x1d,
          0x50, 0xfe, 0xbf, 0x12, 0x65, 0x65, 0x6d, 0x16, 0x1e, 0xcf, 0x3d, 0xbc,
          0xde, 0x91, 0xb9, 0x7b, 0x0a, 0xc5, 0x2a, 0xd4, 0xea, 0x54, 0x4c, 0xa6,
          0x76, 0xe9, 0x27, 0x03, 0x48, 0x4b, 0x9b, 0x14, 0x0e, 0x87, 0x09, 0x9b,
          0xad, 0x9a, 0xba, 0xba, 0x21, 0xcc, 0xe6, 0xe9, 0xb9, 0x67, 0x5a, 0xad,
          0x82, 0xf7, 0xdf, 0xdf, 0x8e, 0x2c, 0x97, 0x08, 0x93, 0xe9, 0xce, 0x82,
          0x10, 0x2f, 0x0c, 0x90, 0x98, 0x38, 0x2e, 0xcc, 0xe6, 0x6f, 0x39, 0x72,
          0x64, 0x84, 0xba, 0xba, 0x08, 0xfc, 0x7e, 0x09, 0x8f, 0x27, 0x14, 0x9f,
          0x2f, 0x00, 0xa3, 0x71, 0x9c, 0xf5, 0xeb, 0x27, 0x71, 0xbb, 0xfd, 0xb4,
          0xb6, 0xb6, 0x10, 0x1d, 0xfd, 0xd6, 0x73, 0xf3, 0xac, 0x18, 0x20, 0x2d,
          0x6d, 0x5a, 0xf4, 0xf5, 0xd5, 0xf2, 0xe1, 0x87, 0x8f, 0xb9, 0x79, 0x33,
          0x84, 0x87, 0x0f, 0xa3, 0x51, 0xa9, 0xfc, 0xbc, 0xf1, 0x86, 0x93, 0xbc,
          0x3c, 0x3b, 0x1a, 0x8d, 0x82, 0xf4, 0x74, 0x23, 0xf1, 0xf1, 0x39, 0x48,
          0x12, 0xb8, 0xa7, 0x14, 0xb4, 0xb6, 0x8e, 0x3e, 0xb7, 0x04, 0xd2, 0x72,
          0xbf, 0x82, 0x8c, 0x8c, 0x28, 0x61, 0xb7, 0x37, 0x72, 0xf2, 0x64, 0x3d,
          0x97, 0x2e, 0xa9, 0x19, 0x19, 0x51, 0xb1, 0x65, 0xcb, 0x18, 0x7b, 0xf7,
          0xce, 0x90, 0x9f, 0x1f, 0x49, 0x44, 0x44, 0x2a, 0x3a, 0xdd, 0x4e, 0x4c,
          0xa6, 0x8e, 0xe7, 0x8a, 0x2d, 0x14, 0x4b, 0xee, 0x80, 0x2c, 0xa7, 0x08,
          0x87, 0xa3, 0x86, 0x9a, 0x9a, 0x4a, 0xaa, 0xaa, 0xa6, 0xa8, 0xae, 0x8e,
          0x44, 0x08, 0x41, 0x5e, 0x9e, 0x83, 0xc3, 0x87, 0x63, 0x30, 0x1a, 0xdf,
          0x41, 0xa5, 0x4a, 0xa0, 0xb1, 0xd1, 0x2c, 0x41, 0xc7, 0x4a, 0xb4, 0x97,
          0x06, 0x90, 0xe5, 0x64, 0xd1, 0xdc, 0xfc, 0x19, 0x27, 0x4e, 0xdc, 0xa3,
          0xbe, 0x3e, 0x04, 0x8f, 0x47, 0x47, 0x6a, 0xaa, 0x9b, 0xc3, 0x87, 0xfd,
          0xbc, 0xfd, 0xf6, 0x0e, 0x74, 0xba, 0xc2, 0xa7, 0x27, 0xdc, 0xbc, 0x62,
          0xe1, 0x25, 0x01, 0x52, 0x52, 0xbd, 0xa2, 0xbd, 0xe3, 0x13, 0xca, 0xcb,
          0xed, 0x5c, 0xbd, 0x1a, 0x8e, 0x4a, 0xe5, 0xa7, 0xb4, 0xb4, 0x9f, 0x92,
          0x92, 0x28, 0x64, 0xf9, 0x5d, 0xac, 0x56, 0xb7, 0x04, 0xed, 0x2f, 0x2c,
          0xbc, 0x28, 0x80, 0x3e, 0x61, 0x52, 0x7c, 0xf4, 0xf1, 0xd7, 0x54, 0x5d,
          0x0e, 0x66, 0x64, 0x38, 0x14, 0x59, 0x1e, 0xa5, 0xb8, 0x58, 0xc9, 0xbe,
          0x7d, 0xfb, 0xb1, 0xd9, 0x42, 0x25, 0xab, 0xd5, 0xfd, 0x3f, 0x0b, 0x3f,
          0x17, 0x20, 0xc1, 0xe0, 0x16, 0xe7, 0x2a, 0xaa, 0xf9, 0xf2, 0x7c, 0x28,
          0xb3, 0xb3, 0xcc, 0xab, 0xb5, 0xc5, 0x32, 0xb1, 0xa2, 0x03, 0xb6, 0x62,
          0x00, 0x83, 0xc1, 0x25, 0xca, 0xcb, 0x2f, 0x50, 0xf9, 0xa5, 0x8e, 0xf5,
          0x71, 0xd3, 0x1c, 0x39, 0xe2, 0x27, 0x37, 0x77, 0x07, 0xdd, 0xdd, 0xb1,
          0x92, 0xc5, 0x32, 0xb1, 0xa2, 0xc4, 0x6f, 0x66, 0xac, 0x11, 0x7e, 0x4f,
          0x2f, 0x08, 0xff, 0x93, 0x1b, 0x92, 0x12, 0xa5, 0x2a, 0x8e, 0xa6, 0x96,
          0xe1, 0x79, 0x8b, 0x98, 0x03, 0x30, 0x18, 0x5c, 0xa2, 0xa2, 0xe2, 0x1b,
          0x2a, 0x2b, 0xc3, 0xd0, 0x68, 0x7c, 0x94, 0x95, 0x05, 0x53, 0x50, 0xf0,
          0xab, 0xa7, 0xb5, 0x5e, 0x5e, 0xa4, 0xc4, 0x54, 0x0b, 0x95, 0x2e, 0x07,
          0x8f, 0xe3, 0x3a, 0xb6, 0x9a, 0x26, 0xee, 0xd5, 0xb6, 0x31, 0x33, 0xed,
          0x03, 0x20, 0x20, 0x28, 0x90, 0x0d, 0xf9, 0x46, 0x8c, 0xd9, 0xfb, 0x45,
          0xb0, 0x6e, 0x37, 0x8d, 0xa6, 0x4e, 0x09, 0x9e, 0xf6, 0x01, 0x83, 0xc1,
          0x25, 0x8e, 0x1d, 0xbb, 0x48, 0x65, 0xa5, 0x8e, 0xf8, 0x78, 0x0f, 0x65,
          0x65, 0xc1, 0x14, 0x15, 0xfd, 0x91, 0xe6, 0xe6, 0xbe, 0x25, 0xc5, 0xb3,
          0xb2, 0x36, 0x0b, 0xbf, 0xa7, 0x93, 0x91, 0xee, 0xcf, 0x19, 0xb4, 0xd4,
          0xd2, 0xd7, 0xe4, 0xa0, 0xab, 0xce, 0xc5, 0xf7, 0xa3, 0x5a, 0x7a, 0xc7,
          0xa3, 0x98, 0x99, 0x0d, 0x00, 0x04, 0x61, 0x0a, 0x07, 0x71, 0xba, 0x41,
          0x36, 0xbe, 0xa5, 0x24, 0x79, 0x57, 0x0c, 0x29, 0x25, 0xa7, 0x69, 0xb9,
          0xad, 0x91, 0xa4, 0xc9, 0xc9, 0xdb, 0xe2, 0xdc, 0xb9, 0xb3, 0x1c, 0x3d,
          0xaa, 0x46, 0xa3, 0xf1, 0x71, 0xea, 0xd4, 0x2a, 0x0a, 0x0a, 0xf6, 0x63,
          0xb5, 0xba, 0x96, 0x14, 0x97, 0x33, 0xa3, 0xc5, 0x78, 0xdf, 0xa7, 0xdc,
          0xad, 0xa9, 0xc3, 0x7a, 0xa9, 0x87, 0xde, 0x36, 0x41, 0xef, 0xe4, 0x5a,
          0xee, 0xfb, 0xe2, 0x91, 0xd6, 0xbc, 0xc2, 0x48, 0x40, 0x24, 0xb3, 0xd2,
          0x13, 0x00, 0x9d, 0x70, 0xb0, 0x6e, 0xa6, 0x9f, 0x8d, 0x0e, 0x2b, 0xeb,
          0x5e, 0x73, 0xf3, 0xcb, 0xcb, 0xef, 0xb2, 0x26, 0xe9, 0x6f, 0x28, 0xbb,
          0xba, 0x2a, 0xb9, 0x78, 0xd1, 0x87, 0x10, 0x70, 0xe8, 0xd0, 0x23, 0x8a,
          0x8a, 0x3e, 0xa1, 0xb9, 0xf9, 0xd1, 0xa2, 0xe2, 0x5b, 0xd2, 0x83, 0xc5,
          0x48, 0xf7, 0xdf, 0x31, 0x7d, 0x5a, 0xc3, 0x95, 0xb3, 0x1e, 0xba, 0xfb,
          0x56, 0xf3, 0xd8, 0x97, 0x85, 0x3d, 0x6c, 0x1d, 0x5b, 0x77, 0xb9, 0xd8,
          0xba, 0x36, 0x88, 0xdf, 0x1d, 0x8c, 0x44, 0xab, 0x0d, 0x41, 0x7a, 0x9a,
          0x49, 0x88, 0x28, 0x26, 0x9c, 0x51, 0x7c, 0xfe, 0x57, 0xb0, 0x5d, 0xb3,
          0xd1, 0x74, 0xb6, 0x9a, 0xdc, 0x8f, 0x0b, 0x50, 0xb6, 0xb5, 0x35, 0x60,
          0xb5, 0xe6, 0x90, 0x9e, 0x3e, 0x4e, 0x76, 0x76, 0xca, 0x92, 0xe2, 0x46,
          0xc3, 0xb0, 0x30, 0x9f, 0x3b, 0x8e, 0xe5, 0x1f, 0x9d, 0xdc, 0x35, 0xaf,
          0xa2, 0xc6, 0xb9, 0x83, 0xf1, 0xd0, 0x28, 0xe4, 0x5c, 0x27, 0x07, 0xdf,
          0x99, 0xa2, 0xb0, 0xf0, 0x17, 0x68, 0x34, 0x7a, 0xd4, 0xea, 0x94, 0x1f,
          0xb5, 0x65, 0x59, 0xde, 0x24, 0x36, 0xed, 0x3c, 0xc0, 0x17, 0x37, 0x65,
          0xf4, 0xd7, 0xeb, 0x49, 0x6a, 0x3d, 0x8d, 0xd2, 0xef, 0xf7, 0xe2, 0xf7,
          0x2b, 0x88, 0x8f, 0x77, 0xa3, 0xd7, 0xe7, 0xd0, 0xb1, 0x48, 0x37, 0x35,
          0x1a, 0x86, 0x45, 0xc3, 0xb1, 0x3f, 0xd1, 0x78, 0x7e, 0x90, 0x06, 0x97,
          0xcc, 0x40, 0x98, 0x9e, 0x6d, 0x07, 0x7c, 0x14, 0xed, 0x9a, 0x21, 0x3b,
          0xfb, 0x67, 0xcf, 0xcc, 0x82, 0x85, 0x12, 0x29, 0xf0, 0xa2, 0x64, 0x80,
          0x48, 0xec, 0x8f, 0xc0, 0xf9, 0xf0, 0xbb, 0xe5, 0x4f, 0x43, 0xa3, 0x61,
          0x58, 0x98, 0x2b, 0x8e, 0xd1, 0x78, 0x7e, 0x90, 0x66, 0x67, 0x2a, 0xab,
          0xf3, 0x5f, 0xe5, 0x37, 0x7b, 0x27, 0x29, 0xdc, 0xf9, 0x73, 0xc2, 0xc3,
          0xb3, 0x68, 0x69, 0x19, 0x5c, 0x74, 0x16, 0xc8, 0x72, 0x8a, 0xe8, 0xed,
          0xbd, 0x40, 0xfd, 0x15, 0x35, 0xb1, 0x9e, 0x07, 0xac, 0xcd, 0x08, 0x22,
          0xca, 0xb8, 0x67, 0x79, 0x00, 0x59, 0x72, 0x92, 0x30, 0x57, 0xfc, 0x9e,
          0xfa, 0x93, 0xf7, 0xf9, 0x97, 0x67, 0x2b, 0xab, 0x77, 0xc6, 0x71, 0xfc,
          0x74, 0x36, 0x71, 0x71, 0xc5, 0x98, 0x4c, 0x1d, 0x52, 0x4f, 0xcf, 0xe0,
          0xa2, 0xef, 0xcb, 0x72, 0xb2, 0xf8, 0xae, 0xf9, 0x33, 0x4e, 0x95, 0xdf,
          0xc6, 0x59, 0xdb, 0xcb, 0x06, 0xad, 0x9d, 0x88, 0xe2, 0x7c, 0xc2, 0x37,
          0xfd, 0x65, 0x79, 0x00, 0x53, 0x8e, 0xcb, 0x74, 0x56, 0xf5, 0x33, 0x30,
          0x16, 0x82, 0x76, 0x5b, 0x24, 0x7f, 0x2e, 0x4b, 0x20, 0x2e, 0xae, 0x64,
          0x51, 0xab, 0xf5, 0x43, 0x64, 0xa4, 0xb9, 0xc4, 0xe3, 0xce, 0x0f, 0x38,
          0x53, 0x3e, 0xc4, 0xf0, 0xd5, 0x61, 0x62, 0xb5, 0x76, 0x52, 0x4a, 0xf5,
          0xe4, 0xef, 0x2b, 0xc5, 0x6c, 0x99, 0x90, 0x94, 0x4a, 0xe5, 0x2a, 0x94,
          0xca, 0x59, 0x7a, 0x7a, 0xd4, 0x3c, 0x78, 0x70, 0x1d, 0xc8, 0xfb, 0x51,
          0x12, 0x9f, 0xfb, 0x0e, 0x7d, 0x96, 0x69, 0x2c, 0xde, 0x4c, 0x12, 0x62,
          0x02, 0xd9, 0x90, 0xb0, 0xb8, 0xcf, 0xcb, 0x92, 0x93, 0x84, 0xd7, 0x6d,
          0xc1, 0xeb, 0xea, 0xe2, 0xda, 0x47, 0x27, 0xb8, 0x73, 0xa9, 0x9f, 0xd9,
          0xfe, 0x18, 0x22, 0xb4, 0xe1, 0x6c, 0x2b, 0xcb, 0x64, 0xf7, 0x6f, 0xf7,
          0x73, 0xd7, 0x16, 0x22, 0x01, 0x28, 0x8d, 0xc6, 0xed, 0xbc, 0xfe, 0xba,
          0x03, 0x8b, 0x45, 0xc7, 0x8d, 0x1b, 0xed, 0xbc, 0xf7, 0xde, 0x01, 0xd1,
          0xdc, 0x3c, 0x30, 0x2f, 0xb9, 0x22, 0x20, 0x04, 0xb5, 0x56, 0x22, 0x56,
          0xd1, 0x47, 0xbf, 0x39, 0x92, 0x96, 0x6b, 0x5f, 0x91, 0x25, 0xbf, 0x2a,
          0x58, 0x08, 0x41, 0xc0, 0x70, 0xc7, 0x28, 0xad, 0x5f, 0xd4, 0xe2, 0x1c,
          0x98, 0xc4, 0xfa, 0x4f, 0xc1, 0xd8, 0x58, 0x18, 0xf7, 0xc2, 0xd2, 0xd8,
          0x5e, 0x1a, 0x3a, 0x4f, 0x1c, 0xe0, 0x99, 0x46, 0xe4, 0xe5, 0xd0, 0xa1,
          0xc7, 0xe4, 0xe4, 0xa4, 0x92, 0x98, 0xb8, 0x7b, 0xde, 0xf0, 0x89, 0x18,
          0xfd, 0x40, 0x74, 0x5d, 0xb9, 0x49, 0xff, 0x5d, 0x25, 0x53, 0xbd, 0x3e,
          0x02, 0x25, 0x1f, 0x33, 0x1e, 0x10, 0xcf, 0xe8, 0xcf, 0x08, 0x05, 0x93,
          0x33, 0xc1, 0xb4, 0x0f, 0xad, 0xc7, 0x25, 0xd4, 0xf4, 0x84, 0x26, 0x93,
          0xb8, 0x79, 0x9a, 0xc2, 0xbd, 0xa1, 0x14, 0xef, 0xdb, 0x8d, 0xcd, 0xa6,
          0x99, 0x87, 0x3d, 0xd7, 0x8a, 0x8f, 0x1f, 0xbf, 0x40, 0x55, 0xd5, 0x2b,
          0x4c, 0x4c, 0x04, 0x92, 0x96, 0xe6, 0x64, 0xcf, 0x9e, 0x59, 0xf2, 0xf2,
          0xd6, 0x10, 0x1c, 0x1c, 0x04, 0x80, 0x7f, 0xfa, 0x06, 0xfe, 0x29, 0x0b,
          0x63, 0x3d, 0x3e, 0x06, 0xdb, 0xe0, 0x51, 0x13, 0xdc, 0xaf, 0x03, 0xff,
          0xf4, 0x7c, 0x80, 0x01, 0xd6, 0xd2, 0xc5, 0x26, 0xbe, 0x57, 0xc7, 0x11,
          0x1d, 0xeb, 0xe5, 0xe0, 0x1f, 0xec, 0x64, 0xe7, 0xa4, 0x92, 0x98, 0xb8,
          0x67, 0xc1, 0x69, 0x3a, 0xe7, 0x09, 0x7f, 0x30, 0x9b, 0x67, 0xce, 0xf4,
          0xd2, 0xd0, 0xa0, 0xa5, 0xa7, 0x47, 0x87, 0x56, 0x3b, 0x4d, 0x40, 0xc0,
          0x7f, 0xf6, 0x56, 0xfc, 0xd7, 0x72, 0xbd, 0x6e, 0x98, 0x1e, 0x07, 0x31,
          0xfb, 0xe4, 0x3a, 0x30, 0x70, 0x86, 0x60, 0x95, 0x9f, 0xd7, 0xf4, 0x3e,
          0x12, 0x8d, 0x1e, 0x32, 0xdf, 0x0c, 0xa2, 0xb0, 0x50, 0x4f, 0x6c, 0xec,
          0xaf, 0x17, 0x6d, 0x6e, 0xf3, 0x4c, 0xa9, 0x2c, 0x27, 0x8b, 0xb1, 0xb1,
          0x93, 0x34, 0x34, 0xb4, 0x70, 0xf4, 0xe8, 0x06, 0x9c, 0x4e, 0x25, 0x2c,
          0x58, 0xe8, 0x67, 0x43, 0x90, 0x9e, 0xee, 0x24, 0x3f, 0xdf, 0x4e, 0x52,
          0x52, 0x2c, 0x46, 0x63, 0x2e, 0x6a, 0xb5, 0x7e, 0x59, 0xfe, 0x61, 0x41,
          0x57, 0x9c, 0x99, 0xb9, 0x4e, 0x0c, 0x0d, 0x7d, 0x8d, 0xdf, 0xef, 0x9e,
          0xeb, 0xe5, 0x8b, 0xca, 0x0b, 0xd0, 0x68, 0x36, 0xbe, 0x90, 0x2b, 0x5e,
          0xb6, 0x2d, 0xff, 0xa9, 0xe2, 0xa5, 0xff, 0x9c, 0xbe, 0x74, 0x80, 0x7f,
          0x03, 0x2f, 0xc7, 0xb8, 0x4f, 0x36, 0xc7, 0xa5, 0xce, 0x00, 0x00, 0x00,
          0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
        };

      switch (id)
        {
        case image0_ID: {QImage img; img.loadFromData(image0_data, sizeof(image0_data), "PNG"); return QPixmap::fromImage(img);}
        case image1_ID: {QImage img; img.loadFromData(image1_data, sizeof(image1_data), "PNG"); return QPixmap::fromImage(img);}
        default: return QPixmap();
        } // switch
    } // icon
  };

namespace Ui
  {
  class CQFileDialogBtnGrp: public Ui_CQFileDialogBtnGrp {};
} // namespace Ui

class CQFileDialogBtnGrp : public QWidget, public Ui::CQFileDialogBtnGrp
  {
    Q_OBJECT

  public:
    CQFileDialogBtnGrp(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = 0);
    ~CQFileDialogBtnGrp();

  protected slots:
    virtual void languageChange();
  };

#endif // CQFILEDIALOGBTNGRP_H
