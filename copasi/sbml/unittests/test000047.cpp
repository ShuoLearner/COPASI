// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/sbml/unittests/test000047.cpp,v $
//   $Revision: 1.2 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2008/03/11 23:38:21 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include "test000047.h"

#include <sstream>
#include "utilities.hpp"
#include "copasi/CopasiDataModel/CCopasiDataModel.h"
#include "copasi/model/CModel.h"
#include "copasi/model/CMetab.h"
#include "copasi/model/CCompartment.h"
#include "copasi/model/CModelValue.h"
#include "copasi/model/CReaction.h"
#include "copasi/function/CEvaluationNode.h"
#include "copasi/function/CExpression.h"

void test000047::setUp()
{
  // Create the root container.
  CCopasiContainer::init();

  // Create the global data model.
  CCopasiDataModel::Global = new CCopasiDataModel;
}

void test000047::tearDown()
{
  delete CCopasiDataModel::Global;
  CCopasiDataModel::Global = NULL;
  delete CCopasiContainer::Root;
  CCopasiContainer::Root = NULL;
}

void test000047::test_delay()
{
  CCopasiDataModel* pDataModel = CCopasiDataModel::Global;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getQuantityUnitEnum() == CModel::mMol);
  CPPUNIT_ASSERT(pModel->getVolumeUnitEnum() == CModel::ml);
  CPPUNIT_ASSERT(pModel->getTimeUnitEnum() == CModel::s);
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  const CCompartment* pCompartment = pModel->getCompartments()[0];
  CPPUNIT_ASSERT(pCompartment != NULL);
  CPPUNIT_ASSERT(pCompartment->getStatus() == CModelEntity::FIXED);
  CPPUNIT_ASSERT(pModel->getMetabolites().size() == 2);
  const CMetab* pB = pModel->getMetabolites()[1];
  CPPUNIT_ASSERT(pB != NULL);
  CPPUNIT_ASSERT(pB->getStatus() == CModelEntity::FIXED);
  CMetab* pA = pModel->getMetabolites()[0];
  CPPUNIT_ASSERT(pA != NULL);
  CPPUNIT_ASSERT(pA->getStatus() == CModelEntity::ASSIGNMENT);
  const CExpression* pExpr = pA->getExpressionPtr();
  CPPUNIT_ASSERT(pExpr != NULL);
  const CEvaluationNode* pNode = pExpr->getRoot();
  CPPUNIT_ASSERT(pNode != NULL);
  const CEvaluationNodeCall* pCallNode = dynamic_cast<const CEvaluationNodeCall*>(pNode);
  CPPUNIT_ASSERT(pCallNode != NULL);
  const CEvaluationNodeObject* pObjectNode = dynamic_cast<const CEvaluationNodeObject*>(pCallNode->getChild());
  CPPUNIT_ASSERT(pObjectNode != NULL);
  CCopasiObjectName objectCN = pObjectNode->getObjectCN();
  CPPUNIT_ASSERT(!objectCN.empty());
  std::vector<CCopasiContainer*> listOfContainers;
  listOfContainers.push_back(pModel);
  const CCopasiObject* pObject = CCopasiContainer::ObjectFromName(listOfContainers, objectCN);
  CPPUNIT_ASSERT(pObject != NULL);
  CPPUNIT_ASSERT(pObject->isReference() == true);
  CPPUNIT_ASSERT(pObject->getObjectName() == std::string("Concentration"));
  CPPUNIT_ASSERT(pObject->getObjectParent() == pB);
  const CEvaluationNodeNumber* pNumberNode = dynamic_cast<const CEvaluationNodeNumber*>(pObjectNode->getSibling());
  CPPUNIT_ASSERT(pNumberNode != NULL);
  CPPUNIT_ASSERT(((CEvaluationNodeNumber::SubType)CEvaluationNode::subType(pNumberNode->getType())) == CEvaluationNodeNumber::DOUBLE);
  CPPUNIT_ASSERT(fabs((pNumberNode->value() - 0.5) / 0.5) < 1e-3);

  CPPUNIT_ASSERT(pNumberNode->getSibling() == NULL);

  CPPUNIT_ASSERT(pModel->getModelValues().size() == 1);
  const CModelValue* pModelValue = pModel->getModelValues()[0];
  CPPUNIT_ASSERT(pModelValue != NULL);
  CPPUNIT_ASSERT(pModelValue->getStatus() == CModelEntity::FIXED);
  CPPUNIT_ASSERT(pModel->getReactions().size() == 0);
  CPPUNIT_ASSERT(CCopasiMessage::size() == 2);
  CCopasiMessage message = CCopasiMessage::getLastMessage();
  CPPUNIT_ASSERT(message.getType() == CCopasiMessage::WARNING);
  std::string s = message.getText();
  CPPUNIT_ASSERT(!s.empty());
  CPPUNIT_ASSERT(s.find(std::string("COPASI does not support time delays. Calculations on this model will most likely lead to unusable results.")) != std::string::npos);
  // right now, we don't care about the last message since it is a units
  // warning from libSBML
}

const char* test000047::MODEL_STRING =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2/version3\" level=\"2\" version=\"3\">\n"
  "  <model id=\"Model_1\" name=\"New Model\">\n"
  "    <notes>\n"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">\n"
  "        <p>Model with fixed compartment volume, two species with hasOnlySubstanceUnits flag set to false. The units are set to ml and mMol.</p>\n"
  "      </body>\n"
  "    </notes>\n"
  "    <listOfUnitDefinitions>\n"
  "      <unitDefinition id=\"volume\">\n"
  "        <listOfUnits>\n"
  "          <unit kind=\"litre\" scale=\"-3\"/>\n"
  "        </listOfUnits>\n"
  "      </unitDefinition>\n"
  "      <unitDefinition id=\"substance\">\n"
  "        <listOfUnits>\n"
  "          <unit kind=\"mole\" scale=\"-3\"/>\n"
  "        </listOfUnits>\n"
  "      </unitDefinition>\n"
  "    </listOfUnitDefinitions>\n"
  "    <listOfCompartments>\n"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\"/>\n"
  "    </listOfCompartments>\n"
  "    <listOfSpecies>\n"
  "      <species id=\"species_1\" name=\"A\" compartment=\"compartment_1\" initialConcentration=\"1\" constant=\"false\" />\n"
  "      <species id=\"species_2\" name=\"B\" compartment=\"compartment_1\" initialConcentration=\"1\" constant=\"true\" />\n"
  "    </listOfSpecies>\n"
  "    <listOfParameters>\n"
  "      <parameter id=\"parameter_1\" name=\"K\" value=\"0\"/>\n"
  "    </listOfParameters>\n"
  "    <listOfRules>\n"
  "      <assignmentRule variable=\"species_1\">\n"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">\n"
  "          <apply>\n"
  "            <csymbol encoding=\"text\" definitionURL=\"http://www.sbml.org/sbml/symbols/delay\">\n"
  "             delay\n"
  "            </csymbol>\n"
  "            <ci> species_2 </ci>\n"
  "            <cn> 0.5 </cn>\n"
  "          </apply>\n"
  "        </math>\n"
  "      </assignmentRule>\n"
  "    </listOfRules>\n"
  "  </model>\n"
  "</sbml>\n"
;