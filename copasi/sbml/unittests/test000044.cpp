// Copyright (C) 2010 - 2013 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include "test000044.h"

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

#include "copasi/report/CCopasiRootContainer.h"

CCopasiDataModel* test000044::pCOPASIDATAMODEL = NULL;

void test000044::setUp()
{
  // Create the root container.
  CCopasiRootContainer::init(0, NULL, false);
  // Create the global data model.
  pCOPASIDATAMODEL = CCopasiRootContainer::addDatamodel();
}

void test000044::tearDown()
{
  CCopasiRootContainer::destroy();
}

void test000044::test_stoichiometricExpression()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
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
  CMetab* pA = pModel->getMetabolites()[0];
  CPPUNIT_ASSERT(pA != NULL);
  CPPUNIT_ASSERT(pA->getStatus() == CModelEntity::REACTIONS);
  const CMetab* pB = pModel->getMetabolites()[1];
  CPPUNIT_ASSERT(pB != NULL);
  CPPUNIT_ASSERT(pB->getStatus() == CModelEntity::REACTIONS);
  CPPUNIT_ASSERT(pModel->getModelValues().size() == 1);
  const CModelValue* pModelValue = pModel->getModelValues()[0];
  CPPUNIT_ASSERT(pModelValue != NULL);
  CPPUNIT_ASSERT(pModelValue->getStatus() == CModelEntity::FIXED);

  CPPUNIT_ASSERT(pModel->getReactions().size() == 1);
  const CReaction* pReaction1 = pModel->getReactions()[0];
  CPPUNIT_ASSERT(pReaction1 != NULL);
  CPPUNIT_ASSERT(pReaction1->isReversible() == true);
  // check the kinetic law
  const CFunction* pKineticFunction = pReaction1->getFunction();
  CPPUNIT_ASSERT(pKineticFunction != NULL);
  const CMassAction* pMassAction = dynamic_cast<const CMassAction*>(pKineticFunction);
  CPPUNIT_ASSERT(pMassAction != NULL);
  const CChemEq* pChemEq = &pReaction1->getChemEq();
  CPPUNIT_ASSERT(pChemEq != NULL);
  CPPUNIT_ASSERT(pChemEq->getCompartmentNumber() == 1);
  CPPUNIT_ASSERT(pChemEq->getSubstrates().size() == 1);
  const CChemEqElement* pElement = pChemEq->getSubstrates()[0];
  CPPUNIT_ASSERT(pElement != NULL);
  CPPUNIT_ASSERT(fabs((pElement->getMultiplicity() - 2.0) / 2.0) < 1e-3);
  CPPUNIT_ASSERT(pElement->getMetabolite() == pA);
  CPPUNIT_ASSERT(pChemEq->getProducts().size() == 1);
  pElement = pChemEq->getProducts()[0];
  CPPUNIT_ASSERT(pElement != NULL);
  CPPUNIT_ASSERT(fabs((pElement->getMultiplicity() - 3.0) / 3.0) < 1e-3);
  CPPUNIT_ASSERT(pElement->getMetabolite() == pB);
  CPPUNIT_ASSERT(pChemEq->getModifiers().size() == 0);
  //CPPUNIT_ASSERT(CCopasiMessage::size() == 5);
  CCopasiMessage message = CCopasiMessage::getLastMessage();
  CPPUNIT_ASSERT(message.getType() == CCopasiMessage::WARNING);
  std::string s = message.getText();
  CPPUNIT_ASSERT(!s.empty());
  CPPUNIT_ASSERT(s.find(std::string("One or more stoichiometric expressions were evaluated and converted to constants values.")) != std::string::npos);
  // the other four messages are libSBML unit warnings I don't care about right
  // now.
}

const char* test000044::MODEL_STRING =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2/version3\" level=\"2\" version=\"3\">\n"
  "  <model id=\"Model_1\" name=\"New Model\">\n"
  "    <notes>\n"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">\n"
  "        <p>Model with fixed compartment volume, two species with hasOnlySubstanceUnits flag set to false. The units are set to ml and item. There is a stoichiometric expression for the only species in the only reaction. The expression consists of the number 2.0</p>\n"
  "        <p>The substrate of the reaction in the imported model should have a multiplicity of 2.</p>\n"
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
  "      <species id=\"species_1\" name=\"A\" compartment=\"compartment_1\" initialConcentration=\"1\"/>\n"
  "      <species id=\"species_2\" name=\"B\" compartment=\"compartment_1\" initialConcentration=\"1\"/>\n"
  "    </listOfSpecies>\n"
  "    <listOfParameters>\n"
  "      <parameter id=\"parameter_1\" name=\"K\" value=\"0\"/>\n"
  "    </listOfParameters>\n"
  "    <listOfReactions>\n"
  "      <reaction id=\"reaction_1\" name=\"reaction_0\" reversible=\"true\">\n"
  "        <listOfReactants>\n"
  "          <speciesReference species=\"species_1\">\n"
  "            <stoichiometryMath>\n"
  "            <math xmlns=\"http://www.w3.org/1998/Math/MathML\">\n"
  "             <cn>2.0</cn>\n"
  "            </math>\n"
  "            </stoichiometryMath>\n"
  "          </speciesReference>\n"
  "        </listOfReactants>\n"
  "        <listOfProducts>\n"
  "          <speciesReference species=\"species_2\">\n"
  "            <stoichiometryMath>\n"
  "            <math xmlns=\"http://www.w3.org/1998/Math/MathML\">\n"
  "             <cn>3.0</cn>\n"
  "            </math>\n"
  "            </stoichiometryMath>\n"
  "          </speciesReference>\n"
  "        </listOfProducts>\n"
  "        <kineticLaw>\n"
  "          <math xmlns=\"http://www.w3.org/1998/Math/MathML\">\n"
  "            <apply>\n"
  "              <times/>\n"
  "              <ci> compartment_1 </ci>\n"
  "              <apply>\n"
  "                <minus/>\n"
  "                <apply>\n"
  "                  <times/>\n"
  "                  <ci> k1 </ci>\n"
  "                  <ci> species_1 </ci>\n"
  "                </apply>\n"
  "                <apply>\n"
  "                  <times/>\n"
  "                  <ci> k2 </ci>\n"
  "                  <ci> species_2 </ci>\n"
  "                </apply>\n"
  "              </apply>\n"
  "            </apply>\n"
  "          </math>\n"
  "          <listOfParameters>\n"
  "            <parameter id=\"k1\" value=\"0.1\"/>\n"
  "            <parameter id=\"k2\" value=\"0.1\"/>\n"
  "          </listOfParameters>\n"
  "        </kineticLaw>\n"
  "      </reaction>\n"
  "    </listOfReactions>\n"
  "  </model>\n"
  "</sbml>\n"
  ;
