// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/MIRIAMUI/Attic/CQRDFListViewWidget.ui.h,v $
//   $Revision: 1.6 $
//   $Name:  $
//   $Author: aekamal $
//   $Date: 2008/01/31 05:01:51 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include "CRDFListViewItem.h"

#include "model/CModel.h"
#include "CopasiDataModel/CCopasiDataModel.h"
#include "report/CKeyFactory.h"
#include "model/CModelValue.h"
#include "MIRIAM/CRDFGraph.h"
#include "MIRIAM/CRDFNode.h"
#include "MIRIAM/CRDFParser.h"
#include "MIRIAM/CRDFWriter.h"

void CQRDFListViewWidget::init()
{}

bool CQRDFListViewWidget::enter(const std::string & key)
{
  mKey = key;
  load();

  return true;
}

bool CQRDFListViewWidget::leave()
{
  save();

  return true;
}

void CQRDFListViewWidget::load()
{
  mpListView->clear();
  mpListView->clearVisitedNodes();

  CRDFGraph * pGraph = NULL;
  CModelEntity * pEntity = NULL;

  if ((pEntity = dynamic_cast< CModelEntity * >(GlobalKeys.get(mKey))) != NULL)
    pGraph = CRDFParser::graphFromXml(pEntity->getMiriamAnnotation());

  if (pGraph == NULL)
    return;

  std::map< std::string, CRDFNode * >::const_iterator itMap = pGraph->getLocalResourceNodeMap().begin();
  std::map< std::string, CRDFNode * >::const_iterator endMap = pGraph->getLocalResourceNodeMap().end();

  const CRDFNode * pNode;
  for (;itMap != endMap; ++itMap)
    {
      pNode = itMap->second;
      if (pNode && pNode->isSubjectNode() && !pNode->isObjectNode())
        {
          CRDFListViewItem * pItem = new CRDFListViewItem(mpListView, NULL);
          pItem->setNode(pNode);
        }
    }

  pdelete(pGraph);
}

void CQRDFListViewWidget::save()
{
  qWarning("CQRDFListViewWidget::save(): Not implemented yet");
}

void CQRDFListViewWidget::destroy()
{}

bool CQRDFListViewWidget::update(ListViews::ObjectType, ListViews::Action, const std::string&)
{
  qWarning("CQRDFListViewWidget::update(ListViews::ObjectType,ListViews::Action,const std::string&): Not implemented yet");
  return FALSE;
}