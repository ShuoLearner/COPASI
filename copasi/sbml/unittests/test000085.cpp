// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/sbml/unittests/test000085.cpp,v $
//   $Revision: 1.1 $
//   $Name:  $
//   $Author: gauges $
//   $Date: 2009/05/08 15:54:06 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include "test000085.h"

#include <sstream>
#include "utilities.hpp"
#include "copasi/CopasiDataModel/CCopasiDataModel.h"
#include "copasi/report/CCopasiRootContainer.h"
#include "copasi/model/CModel.h"

#include "sbml/SBMLDocument.h"
#include "sbml/Model.h"
#include "sbml/Compartment.h"
#include "sbml/UnitDefinition.h"
#include "sbml/Unit.h"
#include "sbml/UnitKind.h"

CCopasiDataModel* test000085::pCOPASIDATAMODEL = NULL;

void test000085::setUp()
{
  // Create the root container.
  CCopasiRootContainer::init(false, 0, NULL);
  // Create the global data model.
  pCOPASIDATAMODEL = CCopasiRootContainer::addDatamodel();
}

void test000085::tearDown()
{
  CCopasiRootContainer::destroy();
}

void test000085::test_import_compartment_units_1()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING1));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getLengthUnitEnum() == CModel::nm);
  // make sure there is no warning about inconsistent units
  unsigned int i = 0, iMax = CCopasiMessage::size();

  while (i < iMax)
    {
      CCopasiMessage message = CCopasiMessage::getLastMessage();
      // make sure there is no message about units not being consistent
      CPPUNIT_ASSERT(message.getNumber() != MCSBML + 24);
      // make sure all units were imported
      CPPUNIT_ASSERT(message.getNumber() != MCSBML + 55);
      ++i;
    }
}

void test000085::test_import_compartment_units_2()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING2));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getLengthUnitEnum() == CModel::m);
  unsigned int i = 0, iMax = CCopasiMessage::size();

  while (i < iMax)
    {
      CCopasiMessage message = CCopasiMessage::getLastMessage();

      // make sure there is a message about the unit not being recognized by
      // COPASI since the scale is off
      if (message.getNumber() == MCSBML + 66)
        {
          break;
        }

      ++i;
    }

  CPPUNIT_ASSERT(i != iMax);
}

void test000085::test_import_compartment_units_3()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;

  try
    {
      CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING3));
    }
  catch (...)
    {
      return;
    }

  // we expect an exception, so we should not end up here
  CPPUNIT_ASSERT(false);
}

void test000085::test_import_compartment_units_4()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING4));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getLengthUnitEnum() == CModel::mm);
  // make sure there is no warning about inconsistent units
  unsigned int i = 0, iMax = CCopasiMessage::size();
  unsigned int found24 = 0;
  unsigned int found55 = 0;

  while (i < iMax)
    {
      CCopasiMessage message = CCopasiMessage::getLastMessage();

      // check for messages about inconsistent units
      if (message.getNumber() == MCSBML + 24)
        {
          ++found24;
        }

      // check for import errors
      if (message.getNumber() == MCSBML + 55)
        {
          ++found55;
        }

      ++i;
    }

  CPPUNIT_ASSERT(found55 == 0);
  CPPUNIT_ASSERT(found24 == 0);
}

void test000085::test_import_compartment_units_5()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;

  try
    {
      CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING5));
    }
  catch (...)
    {
      return;
    }

  // we expect an exception, so we should not end up here
  CPPUNIT_ASSERT(false);
}

void test000085::test_import_compartment_units_6()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;

  try
    {
      CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING6));
    }
  catch (...)
    {
      return;
    }

  // we expect an exception, so we should not end up here
  CPPUNIT_ASSERT(false);
}

void test000085::test_import_compartment_units_7()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING7));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getLengthUnitEnum() == CModel::nm);
  // make sure there is no warning about inconsistent units
  unsigned int i = 0, iMax = CCopasiMessage::size();
  unsigned int found24 = 0;
  unsigned int found55 = 0;

  while (i < iMax)
    {
      CCopasiMessage message = CCopasiMessage::getLastMessage();

      // check for messages about inconsistent units
      if (message.getNumber() == MCSBML + 24)
        {
          ++found24;
        }

      // check for import errors
      if (message.getNumber() == MCSBML + 55)
        {
          ++found55;
        }

      ++i;
    }

  CPPUNIT_ASSERT(found55 == 0);
  CPPUNIT_ASSERT(found24 == 0);
}

void test000085::test_import_compartment_units_8()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING8));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getLengthUnitEnum() == CModel::m);
  // make sure there is no warning about inconsistent units
  unsigned int i = 0, iMax = CCopasiMessage::size();
  unsigned int found24 = 0;
  unsigned int found55 = 0;

  while (i < iMax)
    {
      CCopasiMessage message = CCopasiMessage::getLastMessage();

      // check for messages about inconsistent units
      if (message.getNumber() == MCSBML + 24)
        {
          ++found24;
        }

      // check for import errors
      if (message.getNumber() == MCSBML + 55)
        {
          ++found55;
        }

      ++i;
    }

  CPPUNIT_ASSERT(found55 == 0);
  CPPUNIT_ASSERT(found24 == 0);
}

void test000085::test_import_compartment_units_9()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING9));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getLengthUnitEnum() == CModel::m);
  // make sure there is no warning about inconsistent units
  unsigned int i = 0, iMax = CCopasiMessage::size();
  unsigned int found24 = 0;
  unsigned int found55 = 0;

  while (i < iMax)
    {
      CCopasiMessage message = CCopasiMessage::getLastMessage();

      // check for messages about inconsistent units
      if (message.getNumber() == MCSBML + 24)
        {
          ++found24;
        }

      // check for import errors
      if (message.getNumber() == MCSBML + 55)
        {
          ++found55;
        }

      ++i;
    }

  CPPUNIT_ASSERT(found55 == 0);
  CPPUNIT_ASSERT(found24 == 0);
}

void test000085::test_import_compartment_units_10()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING10));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getLengthUnitEnum() == CModel::m);
  // make sure there is no warning about inconsistent units
  unsigned int i = 0, iMax = CCopasiMessage::size();
  unsigned int found24 = 0;
  unsigned int found55 = 0;

  while (i < iMax)
    {
      CCopasiMessage message = CCopasiMessage::getLastMessage();

      // check for messages about inconsistent units
      if (message.getNumber() == MCSBML + 24)
        {
          ++found24;
        }

      // check for import errors
      if (message.getNumber() == MCSBML + 55)
        {
          ++found55;
        }

      ++i;
    }

  CPPUNIT_ASSERT(found55 == 0);
  CPPUNIT_ASSERT(found24 == 0);
}

void test000085::test_import_compartment_units_11()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;

  try
    {
      CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING11));
    }
  catch (...)
    {
      return;
    }

  // we expect an exception, so we should not end up here
  CPPUNIT_ASSERT(false);
}

void test000085::test_import_compartment_units_12()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;

  try
    {
      CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING12));
    }
  catch (...)
    {
      return;
    }

  // we expect an exception, so we should not end up here
  CPPUNIT_ASSERT(false);
}

void test000085::test_import_compartment_units_13()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING13));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 1);
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getLengthUnitEnum() == CModel::dimensionlessLength);
  // make sure there is no warning about inconsistent units
  unsigned int i = 0, iMax = CCopasiMessage::size();

  while (i < iMax)
    {
      CCopasiMessage message = CCopasiMessage::getLastMessage();
      // make sure there is no message about units not being consistent
      CPPUNIT_ASSERT(message.getNumber() != MCSBML + 24);
      // make sure all units were imported
      CPPUNIT_ASSERT(message.getNumber() != MCSBML + 55);
      ++i;
    }
}

void test000085::test_import_compartment_units_14()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;

  try
    {
      CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING14));
    }
  catch (...)
    {
      return;
    }

  // we expect an exception, so we should not end up here
  CPPUNIT_ASSERT(false);
}

void test000085::test_import_compartment_units_15()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;

  try
    {
      CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING15));
    }
  catch (...)
    {
      return;
    }

  // we expect an exception, so we should not end up here
  CPPUNIT_ASSERT(false);
}

void test000085::test_import_compartment_units_16()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;

  try
    {
      CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING16));
    }
  catch (...)
    {
      return;
    }

  // we expect an exception, so we should not end up here
  CPPUNIT_ASSERT(false);
}

void test000085::test_import_compartment_units_17()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;

  try
    {
      CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING17));
    }
  catch (...)
    {
      return;
    }

  // we expect an exception, so we should not end up here
  CPPUNIT_ASSERT(false);
}

void test000085::test_import_compartment_units_18()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING18));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 2);
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getLengthUnitEnum() == CModel::m);
  // make sure there is no warning about inconsistent units
  unsigned int i = 0, iMax = CCopasiMessage::size();
  unsigned int found24 = 0;
  unsigned int found55 = 0;

  while (i < iMax)
    {
      CCopasiMessage message = CCopasiMessage::getLastMessage();

      // check for messages about inconsistent units
      if (message.getNumber() == MCSBML + 24)
        {
          ++found24;
        }

      // check for import errors
      if (message.getNumber() == MCSBML + 55)
        {
          ++found55;
        }

      ++i;
    }

  CPPUNIT_ASSERT(found55 == 0);
  CPPUNIT_ASSERT(found24 == 0);
}

void test000085::test_import_compartment_units_19()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING19));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 2);
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getLengthUnitEnum() == CModel::m);
  // make sure there is no warning about inconsistent units
  unsigned int i = 0, iMax = CCopasiMessage::size();
  unsigned int found24 = 0;
  unsigned int found55 = 0;

  while (i < iMax)
    {
      CCopasiMessage message = CCopasiMessage::getLastMessage();

      // check for messages about inconsistent units
      if (message.getNumber() == MCSBML + 24)
        {
          ++found24;
        }

      // check for import errors
      if (message.getNumber() == MCSBML + 55)
        {
          ++found55;
        }

      ++i;
    }

  CPPUNIT_ASSERT(found55 == 0);
  CPPUNIT_ASSERT(found24 == 0);
}

void test000085::test_import_compartment_units_20()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING20));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 2);
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getLengthUnitEnum() == CModel::m);
  // make sure there is no warning about inconsistent units
  unsigned int i = 0, iMax = CCopasiMessage::size();
  unsigned int found24 = 0;
  unsigned int found55 = 0;

  while (i < iMax)
    {
      CCopasiMessage message = CCopasiMessage::getLastMessage();

      // check for messages about inconsistent units
      if (message.getNumber() == MCSBML + 24)
        {
          ++found24;
        }

      // check for import errors
      if (message.getNumber() == MCSBML + 55)
        {
          ++found55;
        }

      ++i;
    }

  CPPUNIT_ASSERT(found55 == 0);
  CPPUNIT_ASSERT(found24 == 1);
}

void test000085::test_import_compartment_units_21()
{
  CCopasiDataModel* pDataModel = pCOPASIDATAMODEL;
  CPPUNIT_ASSERT(pDataModel->importSBMLFromString(MODEL_STRING21));
  CModel* pModel = pDataModel->getModel();
  CPPUNIT_ASSERT(pModel->getCompartments().size() == 2);
  CPPUNIT_ASSERT(pModel != NULL);
  CPPUNIT_ASSERT(pModel->getLengthUnitEnum() == CModel::m);
  // make sure there is no warning about inconsistent units
  unsigned int i = 0, iMax = CCopasiMessage::size();
  unsigned int found24 = 0;
  unsigned int found55 = 0;

  while (i < iMax)
    {
      CCopasiMessage message = CCopasiMessage::getLastMessage();

      // check for messages about inconsistent units
      if (message.getNumber() == MCSBML + 24)
        {
          ++found24;
        }

      // check for import errors
      if (message.getNumber() == MCSBML + 55)
        {
          ++found55;
        }

      ++i;
    }

  CPPUNIT_ASSERT(found55 == 0);
  CPPUNIT_ASSERT(found24 == 1);
}

const char* test000085::MODEL_STRING1 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with global length unit nm.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"length\">"
  "        <listOfUnits>"
  "          <unit kind=\"metre\" scale=\"-9\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\" />"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING2 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with global length unit 10e-4* m.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"length\">"
  "        <listOfUnits>"
  "          <unit kind=\"metre\" scale=\"-4\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\" />"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING3 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with global length unit mole.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"length\">"
  "        <listOfUnits>"
  "          <unit kind=\"mole\" scale=\"0\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\" />"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING4 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with global length unit nl</p>"
  "        <p> and a compartment with mm units</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"length\">"
  "        <listOfUnits>"
  "          <unit kind=\"metre\" scale=\"-9\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "      <unitDefinition id=\"my_length\">"
  "        <listOfUnits>"
  "          <unit kind=\"metre\" scale=\"-3\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"my_length\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING5 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with global length unit nm</p>"
  "        <p>and a compartment with mole units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"length\">"
  "        <listOfUnits>"
  "          <unit kind=\"metre\" scale=\"-9\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "      <unitDefinition id=\"my_length\">"
  "        <listOfUnits>"
  "          <unit kind=\"mole\" scale=\"0\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"my_length\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING6 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with global length unit mole</p>"
  "        <p>and a compartment with nm units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"my_length\">"
  "        <listOfUnits>"
  "          <unit kind=\"metre\" scale=\"-9\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "      <unitDefinition id=\"length\">"
  "        <listOfUnits>"
  "          <unit kind=\"mole\" scale=\"0\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"my_length\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING7 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and a compartment with nm units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"my_length\">"
  "        <listOfUnits>"
  "          <unit kind=\"metre\" scale=\"-9\"/>"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"my_length\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING8 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and a compartment with length units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"length\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING9 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and a compartment with metre units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"metre\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING10 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and a compartment with no units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  />"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING11 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and a 1D compartment with volume units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"volume\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING12 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and a 1D compartment with area units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"area\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING13 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and a 1D compartment with dimensionless units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"dimensionless\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING14 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and a 1D compartment with m^2 units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"my_length\">"
  "        <listOfUnits>"
  "          <unit kind=\"metre\" scale=\"0\" exponent\"2\" />"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"my_length\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING15 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and a 1D compartment with m^3 units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"my_length\">"
  "        <listOfUnits>"
  "          <unit kind=\"metre\" scale=\"0\" exponent=\"3\" />"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"my_length\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING16 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and a 1D compartment with mole units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"my_length\">"
  "        <listOfUnits>"
  "          <unit kind=\"mole\" scale=\"0\" />"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"my_length\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING17 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and a 1D compartment with substance units.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"substance\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING18 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and two compartments. One has no unit, the other uses length as the unit.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  />"
  "      <compartment id=\"compartment_2\" name=\"compartment_2\" size=\"1\" spatialDimensions=\"1\"  units=\"length\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING19 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and two compartments. One has no unit, the other uses metre as the unit.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  />"
  "      <compartment id=\"compartment_2\" name=\"compartment_2\" size=\"1\" spatialDimensions=\"1\"  units=\"metre\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING20 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and two compartments. One has no unit, the other uses nm as the unit.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"my_length\">"
  "        <listOfUnits>"
  "          <unit kind=\"metre\" scale=\"-9\" />"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  />"
  "      <compartment id=\"compartment_2\" name=\"compartment_2\" size=\"1\" spatialDimensions=\"1\"  units=\"my_length\"/>"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;

const char* test000085::MODEL_STRING21 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<sbml xmlns=\"http://www.sbml.org/sbml/level2\" level=\"2\" version=\"1\">"
  "  <model id=\"Model_1\" name=\"New Model\">"
  "    <notes>"
  "      <body xmlns=\"http://www.w3.org/1999/xhtml\">"
  "        <p>L2V1 model with no global length unit</p>"
  "        <p>and two compartments. One has nm as the unit, the other one has no unit.</p>"
  "      </body>"
  "    </notes>"
  "    <listOfUnitDefinitions>"
  "      <unitDefinition id=\"my_length\">"
  "        <listOfUnits>"
  "          <unit kind=\"metre\" scale=\"-9\" />"
  "        </listOfUnits>"
  "      </unitDefinition>"
  "    </listOfUnitDefinitions>"
  "    <listOfCompartments>"
  "      <compartment id=\"compartment_1\" name=\"compartment\" size=\"1\" spatialDimensions=\"1\"  units=\"my_length\"/>"
  "      <compartment id=\"compartment_2\" name=\"compartment_2\" size=\"1\" spatialDimensions=\"1\"  />"
  "    </listOfCompartments>"
  "  </model>"
  "</sbml>"
  ;
