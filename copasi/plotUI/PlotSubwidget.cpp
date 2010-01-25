// Begin CVS Header
//   $Source: /fs/turing/cvs/copasi_dev/cvs_admin/addHeader,v $
//   $Revision: 1.11 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2009/07/14 11:09:51 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include "PlotSubwidget.h"

#include "curve2dwidget.h"
#include "HistoWidget.h"
#include "plotwindow.h"
#include "plot/CPlotSpecification.h"
#include "plot/COutputDefinitionVector.h"
#include "report/CKeyFactory.h"
#include "utilities/CAnnotatedMatrix.h"
#include "UI/CCopasiPlotSelectionDialog.h"
#include "model/CMetabNameInterface.h"
#include "CopasiDataModel/CCopasiDataModel.h"
#include "UI/DataModelGUI.h"
#include "copasi/UI/qtUtilities.h"
#include "report/CCopasiRootContainer.h"

//temporary
#include "mathematics.h"

#include "UI/CCopasiSelectionDialog.h"

//-----------------------------------------------------------------------------

/*
 *  Constructs a PlotWidget1 as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
PlotSubwidget::PlotSubwidget(QWidget* parent, const char* name, Qt::WFlags fl)
    : CopasiWidget(parent, name, fl)
{
  setupUi(this);
}

//-----------------------------------------------------------------------------
/*
 *  Destroys the object and frees any allocated resources
 */
PlotSubwidget::~PlotSubwidget()
{}

//-----------------------------------------------------------------------------

//the slot...
void PlotSubwidget::addCurveSlot()
{
  if (mType == CPlotItem::plot2d)
    addCurve2D();
}

void PlotSubwidget::addHistoSlot()
{
  if (mType == CPlotItem::plot2d)
    addHisto1D();
}

void PlotSubwidget::addCurveTab(const std::string & title,
                                const CPlotDataChannelSpec & x,
                                const CPlotDataChannelSpec & y)
{
  CPlotItem* item = new CPlotItem(title, NULL, CPlotItem::curve2d);
  item->addChannel(x);
  item->addChannel(y);

  Curve2DWidget * curveWidget = new Curve2DWidget(tabs);
  assert(CCopasiRootContainer::getDatamodelList()->size() > 0);
  curveWidget->setModel((*CCopasiRootContainer::getDatamodelList())[0]->getModel());
  curveWidget->LoadFromCurveSpec(item);
  tabs->addTab(curveWidget, item->getTitle().c_str());

  delete item;
}

void PlotSubwidget::addCurve2D()
{
  CCopasiPlotSelectionDialog* pBrowser = new CCopasiPlotSelectionDialog();
  std::vector< const CCopasiObject * > vector1;
  std::vector< const CCopasiObject * > vector2;
  pBrowser->setOutputVectors(&vector1, &vector2);
  assert(CCopasiRootContainer::getDatamodelList()->size() > 0);
  CCopasiDataModel* pDataModel = (*CCopasiRootContainer::getDatamodelList())[0];
  assert(pDataModel != NULL);
  pBrowser->setModel(pDataModel->getModel(), CCopasiSimpleSelectionTree::NumericValues);

  if (pBrowser->exec() == QDialog::Rejected)
    {
      return;
    }

  //this assumes that the vector is empty if nothing was chosen
  if (vector1.size() == 0 || vector2.size() == 0)
    {
      return;
    }

  std::vector<CCopasiObjectName> objects1, objects2;
  unsigned C_INT32 i;
  std::vector<CCopasiObjectName>::const_iterator sit;
  const CArrayAnnotation *pArray;

  // 1. enable user to choose either a cell, an entire row/column, or even the objects themselves, if they are arrays.
  // 2. translate to CNs and remove duplicates

  // x-axis is set for single cell selection
  std::string cn;

  for (i = 0; i < vector1.size(); i++)
    {
      if (vector1[i])  // the object is not empty
        {
          // is it an array annotation?
          if ((pArray = dynamic_cast< const CArrayAnnotation * >(vector1[i])))
            {
              // second argument is true as only single cell here is allowed. In this case we
              //can assume that the size of the return vector is 1.
              const CCopasiObject * pObject = CCopasiSelectionDialog::chooseCellMatrix(pArray, true, true, "X axis: ")[0];

              if (!pObject) continue;

              cn = pObject->getCN();
            }
          else
            cn = vector1[i]->getCN();

          // check whether cn is already on objects1
          for (sit = objects1.begin(); sit != objects1.end(); ++sit)
            {
              if (*sit == cn) break;
            }

          // if not exist, input cn into objects1
          if (sit == objects1.end())
            {
              objects1.push_back(cn);
            }
        }
    }

  for (i = 0; i < vector2.size(); i++)
    {
      if (vector2[i])
        {
          // is it an array annotation?
          if ((pArray = dynamic_cast< const CArrayAnnotation * >(vector2[i])))
            {
              // second argument is set false for multi selection
              std::vector<const CCopasiObject*> vvv = CCopasiSelectionDialog::chooseCellMatrix(pArray, false, true, "Y axis: ");
              std::vector<const CCopasiObject*>::const_iterator it;

              for (it = vvv.begin(); it != vvv.end(); ++it)
                {
                  if (!*it) continue;

                  cn = (*it)->getCN();

                  //check if the CN already is in the list, if not add it.
                  for (sit = objects2.begin(); sit != objects2.end(); ++sit)
                    if (*sit == cn) break;

                  if (sit == objects2.end())
                    objects2.push_back(cn);
                }
            }
          else
            {
              cn = vector2[i]->getCN();

              //check if the CN already is in the list, if not add it.
              for (sit = objects2.begin(); sit != objects2.end(); ++sit)
                if (*sit == cn) break;

              if (sit == objects2.end())
                objects2.push_back(cn);
            }
        }
    }

  C_INT32 storeTab = tabs->count();

  if (objects1.size() == 1)
    {
      for (i = 0; i < objects2.size(); ++i)
        {
          addCurveTab(pDataModel->getObject(objects2[i])->getObjectDisplayName()
                      + "|"
                      + pDataModel->getObject(objects1[0])->getObjectDisplayName(),
                      objects1[0], objects2[i]);
        }
    }
  else if (objects2.size() == 1)
    {
      for (i = 0; i < objects1.size(); ++i)
        {
          addCurveTab(pDataModel->getObject(objects2[0])->getObjectDisplayName()
                      + "|"
                      + pDataModel->getObject(objects1[i])->getObjectDisplayName(),
                      objects1[i], objects2[0]);
        }
    }
  else
    {
      unsigned C_INT32 imax;

      if (objects1.size() > objects2.size())
        imax = objects2.size();
      else
        imax = objects1.size();

      for (i = 0; i < imax; ++i)
        {
          addCurveTab(pDataModel->getObject(objects2[i])->getObjectDisplayName()
                      + "|"
                      + pDataModel->getObject(objects1[i])->getObjectDisplayName() ,
                      objects1[i], objects2[i]);
        }
    }

  tabs->setCurrentPage(storeTab);
}

void PlotSubwidget::addHisto1DTab(const std::string & title,
                                  const CPlotDataChannelSpec & x, const C_FLOAT64 & incr)
{
  CPlotItem* item = new CPlotItem(title, NULL, CPlotItem::histoItem1d);
  item->addChannel(x);
  item->setValue("increment", incr);

  HistoWidget * curveWidget = new HistoWidget(tabs);
  assert(CCopasiRootContainer::getDatamodelList()->size() > 0);
  curveWidget->setModel((*CCopasiRootContainer::getDatamodelList())[0]->getModel());
  curveWidget->LoadFromCurveSpec(item);
  tabs->addTab(curveWidget, item->getTitle().c_str());

  delete item;
}

void PlotSubwidget::addHisto1D()
{
  C_INT32 storeTab = tabs->count();
  addHisto1DTab("Histogram", CPlotDataChannelSpec(CCopasiObjectName("")), 1.0);
  tabs->setCurrentPage(storeTab);
}

void PlotSubwidget::createHistograms(std::vector<const CCopasiObject* >objects, const C_FLOAT64 & incr)
{
  C_INT32 storeTab = tabs->count();

  unsigned C_INT32 i;

  for (i = 1; i < objects.size(); ++i)
    {
      if (objects[i])
        addHisto1DTab("Histogram: " + objects[i]->getObjectDisplayName(),
                      CPlotDataChannelSpec(objects[i]->getCN()), incr);

      //         lineEditTitle->setText("Histogram: " + FROM_UTF8(mpObjectX->getObjectDisplayName()));
    }

  tabs->setCurrentPage(storeTab);
}

//-----------------------------------------------------------------------------

void PlotSubwidget::removeCurve()
{
  delete tabs->currentPage();
}

//-----------------------------------------------------------------------------

void PlotSubwidget::commitPlot()
{
  saveToPlotSpec();

  loadFromPlotSpec(dynamic_cast<CPlotSpecification*>(CCopasiRootContainer::getKeyFactory()->get(mKey)));
}

//-----------------------------------------------------------------------------

void PlotSubwidget::deletePlot()
{
  unsigned C_INT32 Index, Size;

  assert(CCopasiRootContainer::getDatamodelList()->size() > 0);
  CCopasiDataModel* pDataModel = (*CCopasiRootContainer::getDatamodelList())[0];
  assert(pDataModel != NULL);

  if (!pDataModel->getModel())
    return;

  CPlotSpecification * pspec = dynamic_cast< CPlotSpecification * >(CCopasiRootContainer::getKeyFactory()->get(mKey));

  if (!pspec) return;

  Index =
    pDataModel->getPlotDefinitionList()->CCopasiVector<CPlotSpecification>::getIndex(pspec);
  pDataModel->getPlotDefinitionList()->removePlotSpec(mKey);

  Size = pDataModel->getPlotDefinitionList()->size();

  if (Size > 0)
    enter((*pDataModel->getPlotDefinitionList())[std::min(Index, Size - 1)]->CCopasiParameter::getKey());
  else
    enter("");

  //ListViews::
  protectedNotify(ListViews::PLOT, ListViews::DELETE, mKey);
}

//-----------------------------------------------------------------------------

void PlotSubwidget::addPlot()
{
  leave();

  std::string name = "plot_";
  int i = 0;
  CPlotSpecification* pPl = NULL;
  name += TO_UTF8(QString::number(i));
  assert(CCopasiRootContainer::getDatamodelList()->size() > 0);
  CCopasiDataModel* pDataModel = (*CCopasiRootContainer::getDatamodelList())[0];
  assert(pDataModel != NULL);

  while (!(pPl = pDataModel->getPlotDefinitionList()->createPlotSpec(name, CPlotItem::plot2d)))
    {
      i++;
      name = "plot_";
      name += TO_UTF8(QString::number(i));
    }

  protectedNotify(ListViews::PLOT, ListViews::ADD);

  enter(pPl->CCopasiParameter::getKey());
}

//-----------------------------------------------------------------------------

void PlotSubwidget::resetPlot()
{
  loadFromPlotSpec(dynamic_cast<CPlotSpecification*>(CCopasiRootContainer::getKeyFactory()->get(mKey)));
}

void PlotSubwidget::typeChanged()
{
  CPlotItem::Type newType;

  switch (comboType->currentItem())
    {
      case 0:
        newType = CPlotItem::plot2d;
        break;
      default :
        fatalError();
    }

  if (mType == newType)
    return; //do nothing

  mType = newType;

  //clear tabWidget
  while (tabs->currentPage()) delete tabs->currentPage();

  //TODO: perhaps create empty tab
}

//-----------------------------------------------------------------------------

bool PlotSubwidget::loadFromPlotSpec(const CPlotSpecification *pspec)
{
  if (!pspec) return false;

  //title
  titleLineEdit->setText(pspec->getTitle().c_str());

  //active?
  activeCheckBox->setChecked(pspec->isActive());

  //type
  mType = pspec->getType();

  switch (mType)
    {
      case CPlotItem::plot2d:
        comboType->setCurrentItem(0);
        checkLogX->setChecked(pspec->isLogX());
        checkLogY->setChecked(pspec->isLogY());
        break;
        break;
      default:
        fatalError();
    }

  C_INT32 oldIndex = tabs->currentPageIndex();

  //clear tabWidget
  while (tabs->currentPage()) delete tabs->currentPage();

  //reconstruct tabWidget from curve specs
  const CCopasiVector<CPlotItem> & curves = pspec->getItems();
  unsigned C_INT32 i, imax = curves.size();

  for (i = 0; i < imax; ++i)
    {
      if (curves[i]->getType() == CPlotItem::curve2d)
        {
          Curve2DWidget* curve = new Curve2DWidget(tabs);
          assert(CCopasiRootContainer::getDatamodelList()->size() > 0);
          curve->setModel((*CCopasiRootContainer::getDatamodelList())[0]->getModel());
          curve->LoadFromCurveSpec(curves[i]);
          tabs->addTab(curve, curves[i]->getTitle().c_str());
        }

      if (curves[i]->getType() == CPlotItem::histoItem1d)
        {
          HistoWidget* histo = new HistoWidget(tabs);
          assert(CCopasiRootContainer::getDatamodelList()->size() > 0);
          histo->setModel((*CCopasiRootContainer::getDatamodelList())[0]->getModel());
          histo->LoadFromCurveSpec(curves[i]);
          tabs->addTab(histo, curves[i]->getTitle().c_str());
        }
    }

  tabs->setCurrentPage(oldIndex);

  return true; //TODO really check
}

bool PlotSubwidget::saveToPlotSpec()
{
  CPlotSpecification* pspec = dynamic_cast< CPlotSpecification * >(mpObject);

  if (!pspec) return true;

  pspec->cleanup();

  //title
  if (pspec->getTitle() != TO_UTF8(titleLineEdit->text()))
    {
      pspec->setTitle(TO_UTF8(titleLineEdit->text()));
      protectedNotify(ListViews::PLOT, ListViews::RENAME, mKey);
    }

  //active?
  pspec->setActive(activeCheckBox->isChecked());

  //scales
  pspec->setLogX(checkLogX->isChecked());
  pspec->setLogY(checkLogY->isChecked());

  //curves
  CPlotItem* item;
  unsigned C_INT32 i, imax;
  imax = tabs->count();

  for (i = 0; i < imax; ++i)
    {
      Curve2DWidget* tmpCurve2D = dynamic_cast<Curve2DWidget*>(tabs->page(i));

      if (tmpCurve2D)
        {
          item = pspec->createItem("dummyname", CPlotItem::curve2d);
          tmpCurve2D->SaveToCurveSpec(item);
        }

      HistoWidget* tmpHisto = dynamic_cast<HistoWidget*>(tabs->page(i));

      if (tmpHisto)
        {
          item = pspec->createItem("dummyname", CPlotItem::histoItem1d);
          tmpHisto->SaveToCurveSpec(item);
        }
    }

  //TODO: CopasiParameters

  return true;
}

//-----------------------------------------------------------------------------

//TODO:  save a copy!

bool PlotSubwidget::enterProtected()
{
  CPlotSpecification* pspec = dynamic_cast< CPlotSpecification * >(mpObject);

  if (!pspec)
    {
      mpListView->switchToOtherWidget(42, "");
      return false;
    }

  return loadFromPlotSpec(pspec);
}

//-----------------------------------------------------------------------------

bool PlotSubwidget::update(ListViews::ObjectType objectType, ListViews::Action C_UNUSED(action), const std::string & C_UNUSED(key))
{
  if (mIgnoreUpdates) return true;

  switch (objectType)
    {//TODO: check list:
      case ListViews::MODEL:
      case ListViews::STATE:
      case ListViews::COMPARTMENT:
      case ListViews::METABOLITE:
      case ListViews::REPORT:
      case ListViews::PLOT:
        return loadFromPlotSpec(dynamic_cast< CPlotSpecification * >(CCopasiRootContainer::getKeyFactory()->get(mKey)));
        break;

      default:
        break;
    }

  return true;
}

//-----------------------------------------------------------------------------

bool PlotSubwidget::leave()
{
  return saveToPlotSpec();
}