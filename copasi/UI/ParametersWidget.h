// Copyright (C) 2010 - 2013 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2005 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#ifndef PARAMETERSWIDGET_H
#define PARAMETERSWIDGET_H

#include <QVariant>
#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QPushButton>

#include "UI/copasiWidget.h"

class Q3ListView;
class Q3ListViewItem;
class CParameterListItem;

class ParametersWidget : public CopasiWidget
{
  Q_OBJECT

public:
  ParametersWidget(QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0);
  ~ParametersWidget();

  virtual bool update(ListViews::ObjectType objectType, ListViews::Action action, const std::string & key);
  virtual bool leave();
  virtual void setFramework(int framework);

protected:
  virtual bool enterProtected();
  bool loadFromModel();
  bool saveToModel();

  void saveData(std::ofstream& file, const std::string& delim = "\t") const;

  QPushButton* commitButton;
  QPushButton* revertButton;
  QPushButton* saveButton;
  Q3ListView* listView;
  QLabel* labelTitle;

  QGridLayout* ParametersWidgetLayout;
  QSpacerItem* spacer2;
  QHBoxLayout* layoutLeft;
  QSpacerItem* spacer1;

  CParameterListItem* mTimeItem;
  CParameterListItem* mCompItem;
  CParameterListItem* mReacItem;
  CParameterListItem* mMetabItem;
  CParameterListItem* mParamItem;

protected slots:
  virtual void commitPressed();
  virtual void revertPressed();
  virtual void savePressed();

  virtual void editItem(Q3ListViewItem * item, const QPoint & pnt, int c);
  virtual void editItem(Q3ListViewItem * item);
};

#endif // PARAMETERSWIDGET_H
