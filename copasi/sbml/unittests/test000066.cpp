// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/sbml/unittests/test000066.cpp,v $
//   $Revision: 1.5 $
//   $Name:  $
//   $Author: gauges $
//   $Date: 2010/03/11 11:52:00 $
// End CVS Header

// Copyright (C) 2010 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include "test000066.h"

#include <sstream>
#include "utilities.hpp"
#include "copasi/CopasiDataModel/CCopasiDataModel.h"
#include "copasi/model/CModel.h"
#include "copasi/model/CMetab.h"
#include "copasi/model/CCompartment.h"
#include "copasi/model/CReaction.h"
#include "copasi/function/CEvaluationNode.h"
#include "copasi/function/CEvaluationNodeVariable.h"

#include "sbml/SBMLDocument.h"
#include "sbml/Model.h"
#include "sbml/Reaction.h"

#include "copasi/report/CCopasiRootContainer.h"

/**
 * These tests are supposed to make sure that assignments on a species with the
 * hasOnlySubstanceUnits flag set are exported correctly.
 * It tests rules and event assignments with and without the flag set.
 */
CCopasiDataModel* test000066::pCOPASIDATAMODEL = NULL;

void test000066::setUp()
{
  // Create the root container.
  CCopasiRootContainer::init(0, NULL, false);
  // Create the global data model.
  pCOPASIDATAMODEL = CCopasiRootContainer::addDatamodel();
}

void test000066::tearDown()
{
  CCopasiRootContainer::destroy();
}

void test000066::test_import_reaction_with_constant_flux()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING1));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getQuantityUnitEnum() == CModel::mMol);
  CPPUNIT_ASSERT(pModel->getVolumeUnitEnum() == CModel::ml);
  CPPUNIT_ASSERT(pModel->getTimeUnitEnum() == CModel::s);
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  const CCompartment* pCompartment = pModel->getCompartments()[0];
  CPPUNIT_ASSERT(pCompartment != NULL);
  CPPUNIT_ASSERT(pCompartment->getStatus() == CModelEntity::FIXED);
  CPPUNIT_ASSERT(pModel->getMetabolites().size() == 1);
  CPPUNIT_ASSERT(pModel->getModelValues().size() == 0);
  CPPUNIT_ASSERT(pModel->getReactions().size() == 1);
  const CReaction* pReaction = pModel->getReactions()[0];
  CPPUNIT_ASSERT(pReaction != NULL);
  CPPUNIT_ASSERT(pReaction->getChemEq().getSubstrates().size() == 0);
  CPPUNIT_ASSERT(pReaction->getChemEq().getProducts().size() == 1);
  CPPUNIT_ASSERT(pReaction->getChemEq().getModifiers().size() == 0);
  CPPUNIT_ASSERT(pReaction->isReversible() == false);
  // check for the local parameter
  CPPUNIT_ASSERT(pReaction->getParameters().size() == 1);
  const CCopasiParameter* pParameter = pReaction->getParameters().getParameter(0);
  CPPUNIT_ASSERT(pReaction->isLocalParameter(0) == true);
  CPPUNIT_ASSERT(pParameter != NULL);
  CPPUNIT_ASSERT(pParameter->getType() == CCopasiParameter::DOUBLE);
  CPPUNIT_ASSERT(fabs(((*(pParameter->getValue().pDOUBLE)) - 0.015) / 0.015) < 1e-12);
  CPPUNIT_ASSERT(pParameter->getObjectName() == "v");
  const CFunction* pFunction = pReaction->getFunction();
  CPPUNIT_ASSERT(pFunction != NULL);
  CPPUNIT_ASSERT(pFunction->getType() == CEvaluationTree::PreDefined);
  CPPUNIT_ASSERT(pFunction->getObjectName() == "Constant flux (irreversible)");
}

void test000066::test_import_reaction_with_constant_flux_function()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING2));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getQuantityUnitEnum() == CModel::mMol);
  CPPUNIT_ASSERT(pModel->getVolumeUnitEnum() == CModel::ml);
  CPPUNIT_ASSERT(pModel->getTimeUnitEnum() == CModel::s);
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  const CCompartment* pCompartment = pModel->getCompartments()[0];
  CPPUNIT_ASSERT(pCompartment != NULL);
  CPPUNIT_ASSERT(pCompartment->getStatus() == CModelEntity::FIXED);
  CPPUNIT_ASSERT(pModel->getMetabolites().size() == 1);
  CPPUNIT_ASSERT(pModel->getModelValues().size() == 0);
  CPPUNIT_ASSERT(pModel->getReactions().size() == 1);
  const CReaction* pReaction = pModel->getReactions()[0];
  CPPUNIT_ASSERT(pReaction != NULL);
  CPPUNIT_ASSERT(pReaction->getChemEq().getSubstrates().size() == 0);
  CPPUNIT_ASSERT(pReaction->getChemEq().getProducts().size() == 1);
  CPPUNIT_ASSERT(pReaction->getChemEq().getModifiers().size() == 0);
  CPPUNIT_ASSERT(pReaction->isReversible() == false);
  // check for the local parameter
  CPPUNIT_ASSERT(pReaction->getParameters().size() == 1);
  const CCopasiParameter* pParameter = pReaction->getParameters().getParameter(0);
  CPPUNIT_ASSERT(pReaction->isLocalParameter(0) == true);
  CPPUNIT_ASSERT(pParameter != NULL);
  CPPUNIT_ASSERT(pParameter->getType() == CCopasiParameter::DOUBLE);
  CPPUNIT_ASSERT(fabs(((*pParameter->getValue().pDOUBLE) - 0.015) / 0.015) < 1e-12);
  CPPUNIT_ASSERT(pParameter->getObjectName() == "v");
  const CFunction* pFunction = pReaction->getFunction();
  CPPUNIT_ASSERT(pFunction != NULL);
  CPPUNIT_ASSERT(pFunction->getType() == CEvaluationTree::PreDefined);
  CPPUNIT_ASSERT(pFunction->getObjectName() == "Constant flux (irreversible)");
}

const char* test000066::MODEL_STRING1 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2/version3\" level=\"2\" version=\"3\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>Model with one reaction that has constant flux kinetics.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"volume\">"
  "        <listOfUnits>"
  "          <unit kind=\"litre\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "      <unitDefinition id=\"substance\">"
  "        <listOfUnits>"
  "          <unit kind=\"mole\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\"/>"
  "    </listOfCompartments>"
  "    <listOfSpecies>"
  "      <species id=\"species_1\" name=\"A\" compartment=\"compartment_1\" initialConcentration=\"1\"/>"
  "    </listOfSpecies>"
  "  <listOfReactions>"
  "    <reaction id=\"reaction1\" reversible=\"false\">"
  "      <listOfProducts>"
  "        <speciesReference species=\"species_1\"/>"
  "      </listOfProducts>"
  "      <kineticLaw>"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">"
  "          <apply>"
  "            <times/>"
  "            <ci> compartment_1 </ci>"
  "            <ci> k </ci>"
  "          </apply>"
  "        </math>"
  "        <listOfParameters>"
  "          <parameter id=\"k\" value=\"0.015\"/>"
  "        </listOfParameters>"
  "      </kineticLaw>"
  "    </reaction>"
  "   </listOfReactions>"
  "  </model>"
  "</sbml>"
  ;

const char* test000066::MODEL_STRING2 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2/version3\" level=\"2\" version=\"3\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>Model with one reaction with constant flux kinetics.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfFunctionDefinitions>"
  "      <functionDefinition id=\"function_1\" name=\"MM\">"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">"
  "          <lambda>"
  "            <bvar>"
  "              <ci> k </ci>"
  "            </bvar>"
  "            <ci> k </ci>"
  "          </lambda>"
  "        </math>"
  "      </functionDefinition>"
  "    </listOfFunctionDefinitions>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"volume\">"
  "        <listOfUnits>"
  "          <unit kind=\"litre\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "      <unitDefinition id=\"substance\">"
  "        <listOfUnits>"
  "          <unit kind=\"mole\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\"/>"
  "    </listOfCompartments>"
  "    <listOfSpecies>"
  "      <species id=\"species_1\" name=\"A\" compartment=\"compartment_1\" initialConcentration=\"1\"/>"
  "    </listOfSpecies>"
  "  <listOfReactions>"
  "    <reaction id=\"reaction1\" reversible=\"false\">"
  "      <listOfProducts>"
  "        <speciesReference species=\"species_1\"/>"
  "      </listOfProducts>"
  "      <kineticLaw>"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">"
  "          <apply>"
  "            <times/>"
  "            <ci> compartment_1 </ci>"
  "            <apply>"
  "              <ci> function_1 </ci>"
  "              <ci> k1 </ci>"
  "            </apply>"
  "          </apply>"
  "        </math>"
  "        <listOfParameters>"
  "          <parameter id=\"k1\" value=\"0.015\"/>"
  "        </listOfParameters>"
  "      </kineticLaw>"
  "    </reaction>"
  "  </listOfReactions>"
  "  </model>"
  "</sbml>"
  ;

void test000066::test_import_reaction_with_constant_flux_global_parameter()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING3));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getQuantityUnitEnum() == CModel::mMol);
  CPPUNIT_ASSERT(pModel->getVolumeUnitEnum() == CModel::ml);
  CPPUNIT_ASSERT(pModel->getTimeUnitEnum() == CModel::s);
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  const CCompartment* pCompartment = pModel->getCompartments()[0];
  CPPUNIT_ASSERT(pCompartment != NULL);
  CPPUNIT_ASSERT(pCompartment->getStatus() == CModelEntity::FIXED);
  CPPUNIT_ASSERT(pModel->getMetabolites().size() == 1);
  CPPUNIT_ASSERT(pModel->getModelValues().size() == 1);
  CPPUNIT_ASSERT(pModel->getReactions().size() == 1);
  const CReaction* pReaction = pModel->getReactions()[0];
  CPPUNIT_ASSERT(pReaction != NULL);
  CPPUNIT_ASSERT(pReaction->getChemEq().getSubstrates().size() == 0);
  CPPUNIT_ASSERT(pReaction->getChemEq().getProducts().size() == 1);
  CPPUNIT_ASSERT(pReaction->getChemEq().getModifiers().size() == 0);
  CPPUNIT_ASSERT(pReaction->isReversible() == false);
  // check for the global parameter
  CPPUNIT_ASSERT(pReaction->getParameters().size() == 1);
  CPPUNIT_ASSERT(pReaction->isLocalParameter(0) == false);
  const CFunction* pFunction = pReaction->getFunction();
  CPPUNIT_ASSERT(pFunction != NULL);
  CPPUNIT_ASSERT(pFunction->getType() == CEvaluationTree::PreDefined);
  CPPUNIT_ASSERT(pFunction->getObjectName() == "Constant flux (irreversible)");
}

void test000066::test_import_reaction_with_constant_flux_function_global_parameter()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING4));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getQuantityUnitEnum() == CModel::mMol);
  CPPUNIT_ASSERT(pModel->getVolumeUnitEnum() == CModel::ml);
  CPPUNIT_ASSERT(pModel->getTimeUnitEnum() == CModel::s);
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  const CCompartment* pCompartment = pModel->getCompartments()[0];
  CPPUNIT_ASSERT(pCompartment != NULL);
  CPPUNIT_ASSERT(pCompartment->getStatus() == CModelEntity::FIXED);
  CPPUNIT_ASSERT(pModel->getMetabolites().size() == 1);
  CPPUNIT_ASSERT(pModel->getModelValues().size() == 1);
  CPPUNIT_ASSERT(pModel->getReactions().size() == 1);
  const CReaction* pReaction = pModel->getReactions()[0];
  CPPUNIT_ASSERT(pReaction != NULL);
  CPPUNIT_ASSERT(pReaction->getChemEq().getSubstrates().size() == 0);
  CPPUNIT_ASSERT(pReaction->getChemEq().getProducts().size() == 1);
  CPPUNIT_ASSERT(pReaction->getChemEq().getModifiers().size() == 0);
  CPPUNIT_ASSERT(pReaction->isReversible() == false);
  // check for the global parameter
  CPPUNIT_ASSERT(pReaction->getParameters().size() == 1);
  CPPUNIT_ASSERT(pReaction->isLocalParameter(0) == false);
  const CFunction* pFunction = pReaction->getFunction();
  CPPUNIT_ASSERT(pFunction != NULL);
  CPPUNIT_ASSERT(pFunction->getType() == CEvaluationTree::PreDefined);
  CPPUNIT_ASSERT(pFunction->getObjectName() == "Constant flux (irreversible)");
}

const char* test000066::MODEL_STRING3 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2/version3\" level=\"2\" version=\"3\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>Model with one reaction that has constant flux kinetics.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"volume\">"
  "        <listOfUnits>"
  "          <unit kind=\"litre\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "      <unitDefinition id=\"substance\">"
  "        <listOfUnits>"
  "          <unit kind=\"mole\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\"/>"
  "    </listOfCompartments>"
  "    <listOfSpecies>"
  "      <species id=\"species_1\" name=\"A\" compartment=\"compartment_1\" initialConcentration=\"1\"/>"
  "    </listOfSpecies>"
  "    <listOfParameters>"
  "      <parameter id=\"parameter_1\" name=\"K\" value=\"1.3\"/>"
  "    </listOfParameters>"
  "  <listOfReactions>"
  "    <reaction id=\"reaction1\" reversible=\"false\">"
  "      <listOfProducts>"
  "        <speciesReference species=\"species_1\"/>"
  "      </listOfProducts>"
  "      <kineticLaw>"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">"
  "          <apply>"
  "            <times/>"
  "            <ci> compartment_1 </ci>"
  "            <ci> parameter_1 </ci>"
  "          </apply>"
  "        </math>"
  "      </kineticLaw>"
  "    </reaction>"
  "   </listOfReactions>"
  "  </model>"
  "</sbml>"
  ;

const char* test000066::MODEL_STRING4 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2/version3\" level=\"2\" version=\"3\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>Model with one reaction with constant flux kinetics.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfFunctionDefinitions>"
  "      <functionDefinition id=\"function_1\" name=\"MM\">"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">"
  "          <lambda>"
  "            <bvar>"
  "              <ci> k </ci>"
  "            </bvar>"
  "            <ci> k </ci>"
  "          </lambda>"
  "        </math>"
  "      </functionDefinition>"
  "    </listOfFunctionDefinitions>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"volume\">"
  "        <listOfUnits>"
  "          <unit kind=\"litre\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "      <unitDefinition id=\"substance\">"
  "        <listOfUnits>"
  "          <unit kind=\"mole\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\"/>"
  "    </listOfCompartments>"
  "    <listOfSpecies>"
  "      <species id=\"species_1\" name=\"A\" compartment=\"compartment_1\" initialConcentration=\"1\"/>"
  "    </listOfSpecies>"
  "    <listOfParameters>"
  "      <parameter id=\"parameter_1\" name=\"K\" value=\"1.3\"/>"
  "    </listOfParameters>"
  "  <listOfReactions>"
  "    <reaction id=\"reaction1\" reversible=\"false\">"
  "      <listOfProducts>"
  "        <speciesReference species=\"species_1\"/>"
  "      </listOfProducts>"
  "      <kineticLaw>"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">"
  "          <apply>"
  "            <times/>"
  "            <ci> compartment_1 </ci>"
  "            <apply>"
  "              <ci> function_1 </ci>"
  "              <ci> parameter_1 </ci>"
  "            </apply>"
  "          </apply>"
  "        </math>"
  "      </kineticLaw>"
  "    </reaction>"
  "  </listOfReactions>"
  "  </model>"
  "</sbml>"
  ;

void test000066::test_import_reaction_with_constant_flux_reversible()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING5));
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
  CPPUNIT_ASSERT(pModel->getModelValues().size() == 0);
  CPPUNIT_ASSERT(pModel->getReactions().size() == 1);
  const CReaction* pReaction = pModel->getReactions()[0];
  CPPUNIT_ASSERT(pReaction != NULL);
  CPPUNIT_ASSERT(pReaction->getChemEq().getSubstrates().size() == 1);
  CPPUNIT_ASSERT(pReaction->getChemEq().getProducts().size() == 1);
  CPPUNIT_ASSERT(pReaction->getChemEq().getModifiers().size() == 0);
  CPPUNIT_ASSERT(pReaction->isReversible() == true);
  // check for the local parameter
  CPPUNIT_ASSERT(pReaction->getParameters().size() == 1);
  const CCopasiParameter* pParameter = pReaction->getParameters().getParameter(0);
  CPPUNIT_ASSERT(pReaction->isLocalParameter(0) == true);
  CPPUNIT_ASSERT(pParameter != NULL);
  CPPUNIT_ASSERT(pParameter->getType() == CCopasiParameter::DOUBLE);
  CPPUNIT_ASSERT(fabs(((*(pParameter->getValue().pDOUBLE)) - 0.015) / 0.015) < 1e-12);
  CPPUNIT_ASSERT(pParameter->getObjectName() == "v");
  const CFunction* pFunction = pReaction->getFunction();
  CPPUNIT_ASSERT(pFunction != NULL);
  CPPUNIT_ASSERT(pFunction->getType() == CEvaluationTree::PreDefined);
  CPPUNIT_ASSERT(pFunction->getObjectName() == "Constant flux (reversible)");
}

void test000066::test_import_reaction_with_constant_flux_reversible_function()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING6));
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
  CPPUNIT_ASSERT(pModel->getModelValues().size() == 0);
  CPPUNIT_ASSERT(pModel->getReactions().size() == 1);
  const CReaction* pReaction = pModel->getReactions()[0];
  CPPUNIT_ASSERT(pReaction != NULL);
  CPPUNIT_ASSERT(pReaction->getChemEq().getSubstrates().size() == 1);
  CPPUNIT_ASSERT(pReaction->getChemEq().getProducts().size() == 1);
  CPPUNIT_ASSERT(pReaction->getChemEq().getModifiers().size() == 0);
  CPPUNIT_ASSERT(pReaction->isReversible() == true);
  // check for the local parameter
  CPPUNIT_ASSERT(pReaction->getParameters().size() == 1);
  const CCopasiParameter* pParameter = pReaction->getParameters().getParameter(0);
  CPPUNIT_ASSERT(pReaction->isLocalParameter(0) == true);
  CPPUNIT_ASSERT(pParameter != NULL);
  CPPUNIT_ASSERT(pParameter->getType() == CCopasiParameter::DOUBLE);
  CPPUNIT_ASSERT(fabs(((*pParameter->getValue().pDOUBLE) - 0.015) / 0.015) < 1e-12);
  CPPUNIT_ASSERT(pParameter->getObjectName() == "v");
  const CFunction* pFunction = pReaction->getFunction();
  CPPUNIT_ASSERT(pFunction != NULL);
  CPPUNIT_ASSERT(pFunction->getType() == CEvaluationTree::PreDefined);
  CPPUNIT_ASSERT(pFunction->getObjectName() == "Constant flux (reversible)");
}

const char* test000066::MODEL_STRING5 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2/version3\" level=\"2\" version=\"3\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>Model with one reaction that has constant flux kinetics.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"volume\">"
  "        <listOfUnits>"
  "          <unit kind=\"litre\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "      <unitDefinition id=\"substance\">"
  "        <listOfUnits>"
  "          <unit kind=\"mole\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\"/>"
  "    </listOfCompartments>"
  "    <listOfSpecies>"
  "      <species id=\"species_1\" name=\"A\" compartment=\"compartment_1\" initialConcentration=\"1\"/>"
  "      <species id=\"species_2\" name=\"B\" compartment=\"compartment_1\" initialConcentration=\"1\"/>"
  "    </listOfSpecies>"
  "  <listOfReactions>"
  "    <reaction id=\"reaction1\" reversible=\"true\">"
  "      <listOfReactants>"
  "        <speciesReference species=\"species_1\"/>"
  "      </listOfReactants>"
  "      <listOfProducts>"
  "        <speciesReference species=\"species_2\"/>"
  "      </listOfProducts>"
  "      <kineticLaw>"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">"
  "          <apply>"
  "            <times/>"
  "            <ci> compartment_1 </ci>"
  "            <ci> k </ci>"
  "          </apply>"
  "        </math>"
  "        <listOfParameters>"
  "          <parameter id=\"k\" value=\"0.015\"/>"
  "        </listOfParameters>"
  "      </kineticLaw>"
  "    </reaction>"
  "   </listOfReactions>"
  "  </model>"
  "</sbml>"
  ;

const char* test000066::MODEL_STRING6 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2/version3\" level=\"2\" version=\"3\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>Model with one reaction with constant flux kinetics.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfFunctionDefinitions>"
  "      <functionDefinition id=\"function_1\" name=\"MM\">"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">"
  "          <lambda>"
  "            <bvar>"
  "              <ci> k </ci>"
  "            </bvar>"
  "            <ci> k </ci>"
  "          </lambda>"
  "        </math>"
  "      </functionDefinition>"
  "    </listOfFunctionDefinitions>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"volume\">"
  "        <listOfUnits>"
  "          <unit kind=\"litre\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "      <unitDefinition id=\"substance\">"
  "        <listOfUnits>"
  "          <unit kind=\"mole\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\"/>"
  "    </listOfCompartments>"
  "    <listOfSpecies>"
  "      <species id=\"species_1\" name=\"A\" compartment=\"compartment_1\" initialConcentration=\"1\"/>"
  "      <species id=\"species_2\" name=\"B\" compartment=\"compartment_1\" initialConcentration=\"1\"/>"
  "    </listOfSpecies>"
  "  <listOfReactions>"
  "    <reaction id=\"reaction1\" reversible=\"true\">"
  "      <listOfReactants>"
  "        <speciesReference species=\"species_1\"/>"
  "      </listOfReactants>"
  "      <listOfProducts>"
  "        <speciesReference species=\"species_2\"/>"
  "      </listOfProducts>"
  "      <kineticLaw>"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">"
  "          <apply>"
  "            <times/>"
  "            <ci> compartment_1 </ci>"
  "            <apply>"
  "              <ci> function_1 </ci>"
  "              <ci> k1 </ci>"
  "            </apply>"
  "          </apply>"
  "        </math>"
  "        <listOfParameters>"
  "          <parameter id=\"k1\" value=\"0.015\"/>"
  "        </listOfParameters>"
  "      </kineticLaw>"
  "    </reaction>"
  "  </listOfReactions>"
  "  </model>"
  "</sbml>"
  ;

void test000066::test_import_reaction_with_constant_flux_reversible_global_parameter()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING7));
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
  CPPUNIT_ASSERT(pModel->getModelValues().size() == 1);
  CPPUNIT_ASSERT(pModel->getReactions().size() == 1);
  const CReaction* pReaction = pModel->getReactions()[0];
  CPPUNIT_ASSERT(pReaction != NULL);
  CPPUNIT_ASSERT(pReaction->getChemEq().getSubstrates().size() == 1);
  CPPUNIT_ASSERT(pReaction->getChemEq().getProducts().size() == 1);
  CPPUNIT_ASSERT(pReaction->getChemEq().getModifiers().size() == 0);
  CPPUNIT_ASSERT(pReaction->isReversible() == true);
  // check for the global parameter
  CPPUNIT_ASSERT(pReaction->getParameters().size() == 1);
  CPPUNIT_ASSERT(pReaction->isLocalParameter(0) == false);
  const CFunction* pFunction = pReaction->getFunction();
  CPPUNIT_ASSERT(pFunction != NULL);
  CPPUNIT_ASSERT(pFunction->getType() == CEvaluationTree::PreDefined);
  CPPUNIT_ASSERT(pFunction->getObjectName() == "Constant flux (reversible)");
}

void test000066::test_import_reaction_with_constant_flux_reversible_function_global_parameter()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING8));
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
  CPPUNIT_ASSERT(pModel->getModelValues().size() == 1);
  CPPUNIT_ASSERT(pModel->getReactions().size() == 1);
  const CReaction* pReaction = pModel->getReactions()[0];
  CPPUNIT_ASSERT(pReaction != NULL);
  CPPUNIT_ASSERT(pReaction->getChemEq().getSubstrates().size() == 1);
  CPPUNIT_ASSERT(pReaction->getChemEq().getProducts().size() == 1);
  CPPUNIT_ASSERT(pReaction->getChemEq().getModifiers().size() == 0);
  CPPUNIT_ASSERT(pReaction->isReversible() == true);
  // check for the global parameter
  CPPUNIT_ASSERT(pReaction->getParameters().size() == 1);
  CPPUNIT_ASSERT(pReaction->isLocalParameter(0) == false);
  const CFunction* pFunction = pReaction->getFunction();
  CPPUNIT_ASSERT(pFunction != NULL);
  CPPUNIT_ASSERT(pFunction->getType() == CEvaluationTree::PreDefined);
  CPPUNIT_ASSERT(pFunction->getObjectName() == "Constant flux (reversible)");
}

const char* test000066::MODEL_STRING7 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2/version3\" level=\"2\" version=\"3\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>Model with one reaction that has constant flux kinetics.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"volume\">"
  "        <listOfUnits>"
  "          <unit kind=\"litre\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "      <unitDefinition id=\"substance\">"
  "        <listOfUnits>"
  "          <unit kind=\"mole\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\"/>"
  "    </listOfCompartments>"
  "    <listOfSpecies>"
  "      <species id=\"species_1\" name=\"A\" compartment=\"compartment_1\" initialConcentration=\"1\"/>"
  "      <species id=\"species_2\" name=\"B\" compartment=\"compartment_1\" initialConcentration=\"1\"/>"
  "    </listOfSpecies>"
  "    <listOfParameters>"
  "      <parameter id=\"parameter_1\" name=\"K\" value=\"1.3\"/>"
  "    </listOfParameters>"
  "  <listOfReactions>"
  "    <reaction id=\"reaction1\" reversible=\"true\">"
  "      <listOfReactants>"
  "        <speciesReference species=\"species_1\"/>"
  "      </listOfReactants>"
  "      <listOfProducts>"
  "        <speciesReference species=\"species_2\"/>"
  "      </listOfProducts>"
  "      <kineticLaw>"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">"
  "          <apply>"
  "            <times/>"
  "            <ci> compartment_1 </ci>"
  "            <ci> parameter_1 </ci>"
  "          </apply>"
  "        </math>"
  "      </kineticLaw>"
  "    </reaction>"
  "   </listOfReactions>"
  "  </model>"
  "</sbml>"
  ;

const char* test000066::MODEL_STRING8 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2/version3\" level=\"2\" version=\"3\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>Model with one reaction with constant flux kinetics.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfFunctionDefinitions>"
  "      <functionDefinition id=\"function_1\" name=\"MM\">"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">"
  "          <lambda>"
  "            <bvar>"
  "              <ci> k </ci>"
  "            </bvar>"
  "            <ci> k </ci>"
  "          </lambda>"
  "        </math>"
  "      </functionDefinition>"
  "    </listOfFunctionDefinitions>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"volume\">"
  "        <listOfUnits>"
  "          <unit kind=\"litre\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "      <unitDefinition id=\"substance\">"
  "        <listOfUnits>"
  "          <unit kind=\"mole\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\"/>"
  "    </listOfCompartments>"
  "    <listOfSpecies>"
  "      <species id=\"species_1\" name=\"A\" compartment=\"compartment_1\" initialConcentration=\"1\"/>"
  "      <species id=\"species_2\" name=\"B\" compartment=\"compartment_1\" initialConcentration=\"1\"/>"
  "    </listOfSpecies>"
  "    <listOfParameters>"
  "      <parameter id=\"parameter_1\" name=\"K\" value=\"1.3\"/>"
  "    </listOfParameters>"
  "  <listOfReactions>"
  "    <reaction id=\"reaction1\" reversible=\"true\">"
  "      <listOfReactants>"
  "        <speciesReference species=\"species_1\"/>"
  "      </listOfReactants>"
  "      <listOfProducts>"
  "        <speciesReference species=\"species_2\"/>"
  "      </listOfProducts>"
  "      <kineticLaw>"
  "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\">"
  "          <apply>"
  "            <times/>"
  "            <ci> compartment_1 </ci>"
  "            <apply>"
  "              <ci> function_1 </ci>"
  "              <ci> parameter_1 </ci>"
  "            </apply>"
  "          </apply>"
  "        </math>"
  "      </kineticLaw>"
  "    </reaction>"
  "  </listOfReactions>"
  "  </model>"
  "</sbml>"
  ;
