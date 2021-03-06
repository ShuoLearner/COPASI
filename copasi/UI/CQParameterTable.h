/* Begin CVS Header
 $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CQParameterTable.h,v $
 $Revision: 1.1 $
 $Name:  $
 $Author: shoops $
 $Date: 2011/09/13 19:21:56 $
 End CVS Header */

// Copyright (C) 2011 - 2010 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#ifndef PARAMETERTABLE_H
#define PARAMETERTABLE_H


#include <QTableWidget>
#include <QTableWidgetItem>

class CReactionInterface;
class CModel;

class ColorTableItem : public QTableWidgetItem
{
public:
  ColorTableItem(QColor color, const QString txt);
  virtual ~ColorTableItem();
  void setColor(QColor color);
};

#ifdef XXXX
class ColorCheckTableItem : public Q3CheckTableItem
{
public:
  ColorCheckTableItem(Q3Table *t, QColor c, const QString txt);
  ~ColorCheckTableItem();
  void setColor(QColor col) {color = col; table()->repaint();}

private:
  void paint(QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected);

  QColor color;
};

class ComboItem : public ColorTableItem
{
public:
  ComboItem(Q3Table *t, EditType et, QColor c, const QStringList & sl);
  QWidget *createEditor() const;
  void setContentFromEditor(QWidget *w);
  void setText(const QString &s);

private:
  QComboBox *cb;
  QStringList mSL;
};

#endif // XXXX

//table used in the reactions widget

class ParameterTable : public QTableWidget
{
  Q_OBJECT

public:
  ParameterTable(QWidget * parent = 0, const char * name = 0);

  void initTable();

public slots:
  void updateTable(const CReactionInterface & ri, const CModel & model);

private slots:
  void handleCurrentCell(int row, int col);
  void slotCellChanged(int row, int col);

signals:
  void signalChanged(int, int, QString);
  void parameterStatusChanged(int, bool);

private:
  //void initTable();

  //convenience function. It gets a List of all metab names in the CMetabNameInterface format
  static const std::vector<std::string> getListOfAllMetabNames(const CModel & model,
      const CReactionInterface & ri);

  //convenience function.
  static QStringList getListOfAllGlobalParameterNames(const CModel & model);
  //convenience function.
  static QStringList getListOfAllCompartmentNames(const CModel & model);

  int mOldRow;

public:
  std::vector<size_t> mLine2Index;
  std::vector<size_t> mIndex2Line;
};

#endif
//********************************************************************************
