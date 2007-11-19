// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/sbml/Attic/SBMLExporter.cpp,v $
//   $Revision: 1.115 $
//   $Name:  $
//   $Author: gauges $
//   $Date: 2007/11/19 14:14:25 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#ifdef WITH_LAYOUT
# define USE_LAYOUT 1
#endif // WITH_LAYOUT

#include <math.h>
#include <list>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <sbml/Unit.h>
#include <sbml/UnitDefinition.h>
#include <sbml/UnitKind.h>
#include <sbml/SBMLDocument.h>
#include <sbml/SBMLWriter.h>
#include <sbml/SpeciesReference.h>
#include <sbml/Rule.h>
#include <sbml/FunctionDefinition.h>
#include <sbml/Event.h>
#include <sbml/Model.h>
#include <sbml/Compartment.h>
#include <sbml/Species.h>
#include <sbml/Reaction.h>
#include <sbml/KineticLaw.h>
#include <sbml/math/ASTNode.h>
#include <sbml/Parameter.h>
#include <sbml/xml/XMLNode.h>
#include <sbml/xml/XMLErrorLog.h>
#include <sbml/xml/XMLInputStream.h>

#include "copasi.h"

#include "SBMLExporter.h"
#include "SBMLIncompatibility.h"

#include "ConverterASTNode.h"
#include "UnitConversionFactory.hpp"

#include "report/CKeyFactory.h"
#include "CopasiDataModel/CCopasiDataModel.h"
#include "utilities/CVersion.h"
#include "utilities/CProcessReport.h"
#include "utilities/CCopasiTree.h"
#include "function/CFunctionDB.h"
#include "function/CExpression.h"
#include "compareExpressions/compare_utilities.h"

#ifdef WITH_LAYOUT
#include "layout/CListOfLayouts.h"
#endif //WITH_LAYOUT

#include "xml/CCopasiXMLInterface.h"

const char* SBMLExporter::HTML_HEADER = "<body xmlns=\"http://www.w3.org/1999/xhtml\">";

const char* SBMLExporter::HTML_FOOTER = "</body>";

/**
 ** Constructor for the exporter.
 */
SBMLExporter::SBMLExporter(): sbmlDocument(NULL), mpIdSet(NULL), mpExportHandler(NULL), mExportExpressions(false)
{
  this->mpIdSet = new std::set<std::string>;
  this->mpUsedFunctions = new std::list<const CEvaluationTree*>;
}

/**
 ** Destructor for the exporter.
 */
SBMLExporter::~SBMLExporter()
{
  pdelete(this->mpIdSet);
  pdelete(this->mpUsedFunctions);
}

/**
 ** This method takes a copasi CModel object, creates an SBMLDocument from
 ** it and returns it as a string.
 ** On failure an empty string is returned.
 */
std::string SBMLExporter::exportSBMLToString(CCopasiDataModel* pDataModel,
    int sbmlLevel , int sbmlVersion ,
    bool incompleteExport, bool checkSBMLCompatibility)
{
  this->mHandledSBMLObjects.clear();
  this->mpCopasiModel = pDataModel->getModel();
  // check the model for SBML compatibility
  if (sbmlVersion == 0 || sbmlLevel > 2) sbmlLevel = 2;
  if (sbmlVersion == 0 || sbmlVersion > 3) sbmlVersion = 3;
  if (checkSBMLCompatibility == true)
    {
      std::vector<SBMLIncompatibility> compatibilityResult = isModelSBMLCompatible(pDataModel, sbmlLevel, sbmlVersion);
      // TODO if the results from the incompatibility check are negative and the
      // incomplete export flag is true, remove everything that can not be exported
      // the export
      if (!compatibilityResult.empty())
        {
          // display the inconsistencies
          return std::string();
        }
    }
  /* create the SBMLDocument from the copasi model */
  this->createSBMLDocumentFromCModel(pDataModel, sbmlLevel, sbmlVersion, incompleteExport);

#ifdef WITH_LAYOUT
  if (sbmlLevel > 1) // don't add layouts for level 1
    {
      this->addLayoutsToSBMLDocument(pDataModel->getListOfLayouts(), pDataModel);
    }
#endif //WITH_LAYOUT

  this->removeUnusedObjects(pDataModel);
  // make a copy of the document

  SBMLDocument* pTmpSBMLDocument = this->sbmlDocument;
  if (!(sbmlLevel == 2 && sbmlVersion == 3))
    {
      // make a copy of the model
      pTmpSBMLDocument = dynamic_cast<SBMLDocument*>(this->sbmlDocument->clone());
      if (sbmlLevel == 1)
        {
          if (sbmlVersion != 2) sbmlVersion = 2;
          if (pTmpSBMLDocument != NULL && pTmpSBMLDocument->getModel() != NULL)
            {
              // expand all function calls in rules and kinetic laws
              ListOf* pList = pTmpSBMLDocument->getModel()->getListOfReactions();
              unsigned int i;
              if (pList != NULL)
                {
                  for (i = 0;i < pList->size();++i)
                    {
                      Reaction* pReaction = dynamic_cast<Reaction*>(pList->get(i));
                      if (pReaction != NULL && pReaction->getKineticLaw() != NULL && pReaction->getKineticLaw()->getMath() != NULL)
                        {
                          ASTNode* pExpanded = create_expression(pReaction->getKineticLaw()->getMath(), pTmpSBMLDocument->getModel());
                          if (pExpanded != NULL)
                            {
                              pReaction->getKineticLaw()->setMath(pExpanded);
                            }
                        }
                    }
                }
              pList = pTmpSBMLDocument->getModel()->getListOfRules();
              if (pList != NULL)
                {
                  for (i = 0;i < pList->size();++i)
                    {
                      Rule* pRule = dynamic_cast<Rule*>(pList->get(i));
                      if (pRule != NULL && pRule->getMath() != NULL)
                        {
                          ASTNode* pExpanded = create_expression(pRule->getMath(), pTmpSBMLDocument->getModel());
                          if (pExpanded != NULL)
                            {
                              pRule->setMath(pExpanded);
                            }
                        }
                    }
                }

              // delete the list of function definitions
              pList = pTmpSBMLDocument->getModel()->getListOfFunctionDefinitions();
              SBase* pSBase = NULL;
              while (pList->size() > 0)
                {
                  pSBase = pList->remove(0);
                  delete pSBase;
                }
            }
        }
      pTmpSBMLDocument->setLevelAndVersion(sbmlLevel, sbmlVersion);
    }
  if (pTmpSBMLDocument->getModel() != NULL)
    {
      SBMLWriter* writer = new SBMLWriter();

      writer->setProgramName("COPASI");
      writer->setProgramVersion(pDataModel->getVersion()->getVersion().c_str());

      char* d = writer->writeToString(pTmpSBMLDocument);
      std::string returnValue = d;
      if (d) free(d);
      pdelete(writer);
      if (pTmpSBMLDocument != this->sbmlDocument) delete pTmpSBMLDocument;
      return returnValue;
    }
  else
    {
      /* if no SBMLDocument could be created return an empty string */
      if (pTmpSBMLDocument != this->sbmlDocument) delete pTmpSBMLDocument;
      return std::string();
    }
}

/**
 ** This method takes a copasi CModel object, creates an SBMLDocument from
 ** it and writes it to a file. The filename is given as the second
 ** argument to the function. The function return "true" on success and
 ** "false" on failure.
 */
bool SBMLExporter::exportSBML(CCopasiDataModel* pDataModel,
                              std::string sbmlFilename, bool overwriteFile, int sbmlLevel, int sbmlVersion, bool incompleteExport, bool checkSBMLCompatibility)
{
  bool success = true;
  this->mHandledSBMLObjects.clear();
  this->mpCopasiModel = pDataModel->getModel();
  /* create a string that represents the SBMLDocument */
  std::string str = this->exportSBMLToString(pDataModel,
                    sbmlLevel, sbmlVersion, incompleteExport, checkSBMLCompatibility);
  if (!str.empty())
    {
      /* check if the file already exisits.
         If yes, write if overwrite is true,
         else create an appropriate  CCopasiMessage. */
      std::ifstream testInfile(utf8ToLocale(sbmlFilename).c_str(), std::ios::in);
      if (testInfile && !overwriteFile)
        {
          // create a CCopasiMessage with the appropriate error
          CCopasiMessage(CCopasiMessage::ERROR, MCDirEntry + 1, sbmlFilename.c_str());
          return false;
        }
      /* write the document to a file */
      std::ofstream outfile(utf8ToLocale(sbmlFilename).c_str(), std::ios::out | std::ios::trunc);
      outfile << str;
      outfile.close();
    }
  else
    {
      /* if no SBMLDocument could be created return false */
      success = false;
    }
  return success;
}

/**
 **  This method takes a copasi CModel object and generates a SBMLDocument
 ** object from it.
 ** Optionally the method takes two integers that specify the level and the
 ** version number of the SBMLDocument that will be generated.
 */
SBMLDocument* SBMLExporter::createSBMLDocumentFromCModel(CCopasiDataModel* pDataModel, int sbmlLevel, int sbmlVersion, bool incompleteExport)
{
  if (mpExportHandler)
    {
      mStep = 0;
      mTotalSteps = 7;
      mpExportHandler->setName("SBML Export");
      mHStep = mpExportHandler->addItem("Exporting model to SBML...",
                                        CCopasiParameter::UINT,
                                        & mStep,
                                        &mTotalSteps);
    }

  this->sbmlDocument = pDataModel->getCurrentSBMLDocument();
  if (!this->sbmlDocument)
    {
      /* create a new document object */
      this->sbmlDocument = new SBMLDocument();
    }
  this->sbmlDocument->setLevelAndVersion(sbmlLevel, sbmlVersion);
  /* create the model object from the copasi model */
  this->createSBMLModelFromCModel(pDataModel, sbmlLevel, sbmlVersion, incompleteExport);
  if (mpExportHandler)
    {
      mStep = 7;
      if (!mpExportHandler->progress(mHStep)) return false;
    }
  this->createFunctionDefinitions();
  if (mpExportHandler)
    {
      mpExportHandler->finish(mHStep);
    }
  return this->sbmlDocument;
}

/**
 ** This method taked a copasi CModel and generates a SBML Model object
 **  which is returned. On failure NULL is returned.
 */
Model* SBMLExporter::createSBMLModelFromCModel(CCopasiDataModel* pDataModel, int sbmlLevel, int sbmlVersion, bool incompleteExport)
{
  unsigned C_INT32 step = 0, totalSteps, hStep;
  Model* sbmlModel = NULL;
  CModel* copasiModel = pDataModel->getModel();
  std::map<CCopasiObject*, SBase*>& copasi2sbmlmap = pDataModel->getCopasi2SBMLMap();
  std::map<CCopasiObject*, SBase*>::const_iterator pos = copasi2sbmlmap.find(copasiModel);
  if (this->mpIdSet) pdelete(this->mpIdSet);
  if (pos != copasi2sbmlmap.end())
    {
      sbmlModel = dynamic_cast<Model*>(pos->second);
      assert(sbmlModel);
      if (sbmlDocument->getModel() != sbmlModel)
        {
          sbmlDocument->setModel(sbmlModel);
          delete sbmlModel;
          sbmlModel = sbmlDocument->getModel();
          copasi2sbmlmap[copasiModel] = sbmlModel;
        }
      this->mpIdSet = SBMLExporter::createIdSet(sbmlModel, pDataModel);
    }
  else
    {
      /* create a new model object */
      this->mpIdSet = SBMLExporter::createIdSet(sbmlModel, pDataModel);
      sbmlModel = new Model();
      sbmlModel->setId(copasiModel->getKey().c_str());
      sbmlDocument->setModel(sbmlModel);
      delete sbmlModel;
      sbmlModel = sbmlDocument->getModel();
      copasi2sbmlmap[copasiModel] = sbmlModel;
    }
  this->mHandledSBMLObjects.insert(sbmlModel);
  if (!copasiModel->getObjectName().empty())
    {
      sbmlModel->setName(copasiModel->getObjectName().c_str());
    }
  if ((!copasiModel->getComments().empty()) && !(this->isEmptyString(copasiModel->getComments())))
    {
      XMLErrorLog* log = new XMLErrorLog();
      XMLInputStream stream(copasiModel->getComments().c_str(), false, "", log);
      XMLNode* c = new XMLNode(stream);
      if (log->getNumErrors() == 0)
        {
          sbmlModel->setNotes(c);
        }
      if (c != NULL) delete c;
      delete log;
    }
  /* if the copasi volume unit does not correspond to the default SBML volume
  ** unit, we have to create a UnitDefinition and make it the default in the
  ** SBML model.
  */
  if (!(copasiModel->getVolumeUnitEnum() == CModel::l))
    {
      UnitDefinition* uDef = this->createSBMLVolumeUnitDefinitionFromCopasiVolumeUnit(copasiModel->getVolumeUnitEnum());
      if (sbmlModel->getUnitDefinition("volume"))
        {
          if (!UnitConversionFactory::areEqual(*sbmlModel->getUnitDefinition("volume"), *uDef))
            {
              ListOf* list = sbmlModel->getListOfUnitDefinitions();
              unsigned int i = list->size();
              for (; i > 0;--i)
                {
                  UnitDefinition* uDef = dynamic_cast<UnitDefinition*>(list->get(i - 1));
                  if (uDef && uDef->getId() == "volume")
                    {
                      list->remove(i - 1);
                      pdelete(uDef);
                      break;
                    }
                }
              sbmlModel->addUnitDefinition(uDef);
            }
        }
      else
        {
          sbmlModel->addUnitDefinition(uDef);
        }
      delete uDef;
    }
  /* if the copasi time unit does not correspond to the default SBML time
  ** unit, we have to create a UnitDefinition and make it the default in the
  ** SBML model.
  */
  if (!(copasiModel->getTimeUnitEnum() == CModel::s))
    {
      UnitDefinition* uDef = this->createSBMLTimeUnitDefinitionFromCopasiTimeUnit(copasiModel->getTimeUnitEnum());
      if (sbmlModel->getUnitDefinition("time"))
        {
          if (!UnitConversionFactory::areEqual(*sbmlModel->getUnitDefinition("time"), *uDef))
            {
              ListOf* list = sbmlModel->getListOfUnitDefinitions();
              unsigned int i = list->size();
              for (; i > 0;++i)
                {
                  UnitDefinition* uDef = dynamic_cast<UnitDefinition*>(list->get(i - 1));
                  if (uDef->getId() == "time")
                    {
                      list->remove(i - 1);
                      pdelete(uDef);
                      break;
                    }
                }
              sbmlModel->addUnitDefinition(uDef);
            }
        }
      else
        {
          sbmlModel->addUnitDefinition(uDef);
        }
      delete uDef;
    }
  /* if the copasi quantity unit does not correspond to the default SBML
  ** substance
  ** unit, we have to create a UnitDefinition and make it the default in the
  ** SBML model.
  */
  if (!(copasiModel->getQuantityUnitEnum() == CModel::Mol))
    {
      UnitDefinition* uDef = this->createSBMLSubstanceUnitDefinitionFromCopasiQuantityUnit(copasiModel->getQuantityUnitEnum());
      if (sbmlModel->getUnitDefinition("substance"))
        {
          if (!UnitConversionFactory::areEqual(*sbmlModel->getUnitDefinition("substance"), *uDef))
            {
              ListOf* list = sbmlModel->getListOfUnitDefinitions();
              unsigned int i = list->size();
              for (; i > 0;++i)
                {
                  UnitDefinition* uDef = dynamic_cast<UnitDefinition*>(list->get(i - 1));
                  if (uDef->getId() == "substance")
                    {
                      list->remove(i - 1);
                      pdelete(uDef);
                      break;
                    }
                }
              sbmlModel->addUnitDefinition(uDef);
            }
        }
      else
        {
          sbmlModel->addUnitDefinition(uDef);
        }
      delete uDef;
    }
  /* create all compartments */
  unsigned int counter, iMax = copasiModel->getCompartments().size();
  if (mpExportHandler)
    {
      mStep = 1;
      mpExportHandler->progress(mHStep);
      totalSteps = iMax;
      step = 0;
      hStep = mpExportHandler->addItem("Creating compartments...",
                                       CCopasiParameter::UINT,
                                       & step,
                                       &totalSteps);
    }
  for (counter = 0; counter < iMax; counter++)
    {
      CCompartment* pCopasiCompartment = copasiModel->getCompartments()[counter];
      Compartment* sbmlCompartment = this->createSBMLCompartmentFromCCompartment(pCopasiCompartment, pDataModel);
      if (!sbmlModel->getCompartment(sbmlCompartment->getId()))
        {
          sbmlModel->addCompartment(sbmlCompartment);
          Compartment* pTmpCompartment = sbmlModel->getCompartment(sbmlCompartment->getId());
          // delete the compartment since a copy was added above
          delete sbmlCompartment;
          assert(pTmpCompartment);
          copasi2sbmlmap[pCopasiCompartment] = pTmpCompartment;
          this->mHandledSBMLObjects.insert(pTmpCompartment);
        }
      ++step;
      if (mpExportHandler && !mpExportHandler->progress(hStep)) return false;
    }
  /* create all metabolites */
  iMax = copasiModel->getMetabolites().size();
  if (mpExportHandler)
    {
      mpExportHandler->finish(hStep);
      mStep = 2;
      mpExportHandler->progress(mHStep);
      totalSteps = iMax;
      step = 0;
      hStep = mpExportHandler->addItem("Creating species...",
                                       CCopasiParameter::UINT,
                                       & step,
                                       &totalSteps);
    }
  for (counter = 0; counter < iMax; counter++)
    {
      CMetab* pCopasiSpecies = copasiModel->getMetabolites()[counter];
      Species* sbmlSpecies = this->createSBMLSpeciesFromCMetab(pCopasiSpecies, pDataModel);
      if (!sbmlModel->getSpecies(sbmlSpecies->getId()))
        {
          sbmlModel->addSpecies(sbmlSpecies);
          Species* pTmpSpecies = sbmlModel->getSpecies(sbmlSpecies->getId());
          // delete the species since a copy was added above
          delete sbmlSpecies;
          assert(pTmpSpecies);
          copasi2sbmlmap[pCopasiSpecies] = pTmpSpecies;
          this->mHandledSBMLObjects.insert(pTmpSpecies);
        }
      ++step;
      if (mpExportHandler && !mpExportHandler->progress(hStep)) return false;
    }
  /* create all global parameters */
  iMax = copasiModel->getModelValues().size();
  if (mpExportHandler)
    {
      mpExportHandler->finish(hStep);
      mStep = 3;
      mpExportHandler->progress(mHStep);
      totalSteps = iMax;
      step = 0;
      hStep = mpExportHandler->addItem("Creating global parameters...",
                                       CCopasiParameter::UINT,
                                       & step,
                                       &totalSteps);
    }
  for (counter = 0; counter < iMax; counter++)
    {
      CModelValue* pCopasiParameter = copasiModel->getModelValues()[counter];
      Parameter* sbmlParameter = this->createSBMLParameterFromCModelValue(pCopasiParameter, pDataModel);
      if (!sbmlModel->getParameter(sbmlParameter->getId()))
        {
          sbmlModel->addParameter(sbmlParameter);
          Parameter* pTmpParameter = sbmlModel->getParameter(sbmlParameter->getId());
          // delete the parameter since a copy was added above
          delete sbmlParameter;
          assert(pTmpParameter);
          copasi2sbmlmap[pCopasiParameter] = pTmpParameter;
          this->mHandledSBMLObjects.insert(pTmpParameter);
        }
      ++step;
      if (mpExportHandler && !mpExportHandler->progress(hStep)) return false;
    }

  /* create all reactions */
  iMax = copasiModel->getReactions().size();
  if (mpExportHandler)
    {
      mpExportHandler->finish(hStep);
      mStep = 4;
      mpExportHandler->progress(mHStep);
      totalSteps = iMax;
      step = 0;
      hStep = mpExportHandler->addItem("Creating reactions...",
                                       CCopasiParameter::UINT,
                                       & step,
                                       &totalSteps);
    }
  for (counter = 0; counter < iMax; counter++)
    {
      CReaction* pCopasiReaction = copasiModel->getReactions()[counter];
      Reaction* sbmlReaction = this->createSBMLReactionFromCReaction(pCopasiReaction, pDataModel);
      if (!sbmlModel->getReaction(sbmlReaction->getId()))
        {
          sbmlModel->addReaction(sbmlReaction);
          Reaction* pTmpReaction = sbmlModel->getReaction(sbmlReaction->getId());
          // delete the reaction since a copy was added above
          delete sbmlReaction;
          assert(pTmpReaction);
          copasi2sbmlmap[pCopasiReaction] = pTmpReaction;
          this->mHandledSBMLObjects.insert(pTmpReaction);
        }
      ++step;
      if (mpExportHandler && !mpExportHandler->progress(hStep)) return false;
    }

  /* create all rules */
  // this is done last so that all reactions have already been created
  // in case we have a rule that depends on a reaction
  std::vector<Rule*> rules;
  iMax = copasiModel->getCompartments().size();
  if (mpExportHandler)
    {
      mpExportHandler->finish(hStep);
      mStep = 5;
      mpExportHandler->progress(mHStep);
      totalSteps = iMax;
      step = 0;
      hStep = mpExportHandler->addItem("Creating rate rules and assignment rules for compartments...",
                                       CCopasiParameter::UINT,
                                       & step,
                                       &totalSteps);
    }
  for (counter = 0; counter < iMax; counter++)
    {
      Rule* sbmlRule = this->createRuleFromCModelEntity(copasiModel->getCompartments()[counter], pDataModel, sbmlLevel, sbmlVersion, incompleteExport);
      if (sbmlRule != NULL)
        {
          rules.push_back(sbmlRule);
        }
      ++step;
      if (mpExportHandler && !mpExportHandler->progress(hStep)) return false;
    }
  iMax = copasiModel->getMetabolites().size();
  if (mpExportHandler)
    {
      mpExportHandler->finish(hStep);
      mStep = 5;
      mpExportHandler->progress(mHStep);
      totalSteps = iMax;
      step = 0;
      hStep = mpExportHandler->addItem("Creating rate rules and assignment rules for metabolites...",
                                       CCopasiParameter::UINT,
                                       & step,
                                       &totalSteps);
    }
  for (counter = 0; counter < iMax; counter++)
    {
      Rule* sbmlRule = this->createRuleFromCModelEntity(copasiModel->getMetabolites()[counter], pDataModel, sbmlLevel, sbmlVersion, incompleteExport);
      if (sbmlRule != NULL)
        {
          rules.push_back(sbmlRule);
        }
      ++step;
      if (mpExportHandler && !mpExportHandler->progress(hStep)) return false;
    }
  iMax = copasiModel->getModelValues().size();
  if (mpExportHandler)
    {
      mpExportHandler->finish(hStep);
      mStep = 5;
      mpExportHandler->progress(mHStep);
      totalSteps = iMax;
      step = 0;
      hStep = mpExportHandler->addItem("Creating rate rules and assignment rules for global parameters...",
                                       CCopasiParameter::UINT,
                                       & step,
                                       &totalSteps);
    }
  for (counter = 0; counter < iMax; counter++)
    {
      Rule* sbmlRule = this->createRuleFromCModelEntity(copasiModel->getModelValues()[counter], pDataModel, sbmlLevel, sbmlVersion, incompleteExport);
      if (sbmlRule != NULL)
        {
          rules.push_back(sbmlRule);
        }
      ++step;
      if (mpExportHandler && !mpExportHandler->progress(hStep)) return false;
    }
  this->exportRules(rules);
  pdelete(this->mpIdSet);
  if (mpExportHandler)
    {
      mpExportHandler->finish(hStep);
    }
  return sbmlModel;
}

/**
 ** This method takes a string that specifies the time unit used in the
 ** copasi model and returns a pointer to the corresponding SBML
 ** UnitDefinition object.
 */
UnitDefinition* SBMLExporter::createSBMLTimeUnitDefinitionFromCopasiTimeUnit(CModel::TimeUnit u)
{
  UnitDefinition* uDef = new UnitDefinition("time");
  uDef->setId("time");
  Unit unit;

  switch (u)
    {
    case CModel::d:
      unit = Unit(UNIT_KIND_SECOND, 1, 0);
      unit.setMultiplier(86400);
      break;
    case CModel::h:
      unit = Unit(UNIT_KIND_SECOND, 1, 0);
      unit.setMultiplier(3600);
      break;
    case CModel::min:
      unit = Unit(UNIT_KIND_SECOND, 1, 0);
      unit.setMultiplier(60);
      break;
    case CModel::ms:
      unit = Unit(UNIT_KIND_SECOND, 1, -3);
      break;
    case CModel::micros:
      unit = Unit(UNIT_KIND_SECOND, 1, -6);
      break;
    case CModel::ns:
      unit = Unit(UNIT_KIND_SECOND, 1, -9);
      break;
    case CModel::ps:
      unit = Unit(UNIT_KIND_SECOND, 1, -12);
      break;
    case CModel::fs:
      unit = Unit(UNIT_KIND_SECOND, 1, -15);
      break;
    default:
      CCopasiMessage(CCopasiMessage::EXCEPTION, "SBMLExporter Error: Unknown copasi time unit.");
      break;
    }

  uDef->addUnit(&unit);
  return uDef;
}

/**
 ** This method takes a string that specifies the substance unit used in the
 ** copasi model and returns a pointer to the corresponding SBML
 ** UnitDefinition object.
 */
UnitDefinition* SBMLExporter::createSBMLSubstanceUnitDefinitionFromCopasiQuantityUnit(CModel::QuantityUnit u)
{
  UnitDefinition* uDef = new UnitDefinition("substance");
  uDef->setId("substance");
  Unit unit;
  switch (u)
    {
    case CModel::mMol:
      unit = Unit(UNIT_KIND_MOLE, 1, -3);
      break;
    case CModel::microMol:
      unit = Unit(UNIT_KIND_MOLE, 1, -6);
      break;
    case CModel::nMol:
      unit = Unit(UNIT_KIND_MOLE, 1, -9);
      break;
    case CModel::pMol:
      unit = Unit(UNIT_KIND_MOLE, 1, -12);
      break;
    case CModel::fMol:
      unit = Unit(UNIT_KIND_MOLE, 1, -15);
      break;
    case CModel::number:
      unit = Unit(UNIT_KIND_ITEM, 1, 0);
      break;
    default:
      CCopasiMessage(CCopasiMessage::EXCEPTION, "SBMLExporter Error: Unknown copasi quantity unit.");
      break;
    }
  uDef->addUnit(&unit);
  return uDef;
}

/**
 ** This method takes a string that specifies the volume unit used in the
 ** copasi model and returns a pointer to the corresponding SBML
 ** UnitDefinition object.
 */
UnitDefinition* SBMLExporter::createSBMLVolumeUnitDefinitionFromCopasiVolumeUnit(CModel::VolumeUnit u)
{
  UnitDefinition* uDef = new UnitDefinition("volume");
  uDef->setId("volume");
  Unit unit;
  switch (u)
    {
    case CModel::ml:
      unit = Unit(UNIT_KIND_LITRE, 1, -3);
      break;
    case CModel::microl:
      unit = Unit(UNIT_KIND_LITRE, 1, -6);
      break;
    case CModel::nl:
      unit = Unit(UNIT_KIND_LITRE, 1, -9);
      break;
    case CModel::pl:
      unit = Unit(UNIT_KIND_LITRE, 1, -12);
      break;
    case CModel::fl:
      unit = Unit(UNIT_KIND_LITRE, 1, -15);
      break;
    case CModel::m3:
      unit = Unit(UNIT_KIND_METRE, 3, 0);
      break;
    default:
      CCopasiMessage(CCopasiMessage::EXCEPTION, "SBMLExporter Error: Unknown copasi volume unit.");
      break;
    }
  uDef->addUnit(&unit);
  return uDef;
}

/**
 ** This method takes a pointer to a copasi CCompartment object and creates
 ** a SBML Compartment. The pointer to the SBML Comprtment is returned.
 */
Compartment* SBMLExporter::createSBMLCompartmentFromCCompartment(CCompartment* copasiCompartment, CCopasiDataModel* pDataModel)
{
  Compartment* sbmlCompartment = NULL;
  std::map<CCopasiObject*, SBase*>& copasi2sbmlmap = pDataModel->getCopasi2SBMLMap();
  std::map<CCopasiObject*, SBase*>::iterator pos = copasi2sbmlmap.find(copasiCompartment);
  if (pos != copasi2sbmlmap.end())
    {
      sbmlCompartment = dynamic_cast<Compartment*>(pos->second);
      assert(sbmlCompartment);
    }
  else
    {
      sbmlCompartment = new Compartment();
      copasi2sbmlmap[copasiCompartment] = sbmlCompartment;

      std::string id = copasiCompartment->getSBMLId();
      if (id == "")
        {
          id = SBMLExporter::createUniqueId(this->mpIdSet, "compartment_");
          copasiCompartment->setSBMLId(id);
          this->mpIdSet->insert(id);
        }
      sbmlCompartment->setId(id);
      sbmlCompartment->setSpatialDimensions(3);
      sbmlCompartment->setConstant(true);
    }
  this->mHandledSBMLObjects.insert(sbmlCompartment);
  sbmlCompartment->setName(copasiCompartment->getObjectName().c_str());
  double value = copasiCompartment->getInitialValue();
  // if the value is NaN, unset the initial volume
  if (!isnan(value))
    {
      sbmlCompartment->setVolume(value);
    }
  else
    {
      sbmlCompartment->unsetVolume();
    }
  return sbmlCompartment;
}

/**
 ** This method takes a pointer to a copasi CMetab object and creates a SBML
 ** Species object. The pointer to the species object is returned.
 */
Species* SBMLExporter::createSBMLSpeciesFromCMetab(CMetab* copasiMetabolite, CCopasiDataModel* pDataModel)
{
  Species* sbmlSpecies = NULL;
  std::map<CCopasiObject*, SBase*>& copasi2sbmlmap = pDataModel->getCopasi2SBMLMap();
  std::map<CCopasiObject*, SBase*>::iterator pos = copasi2sbmlmap.find(copasiMetabolite);
  if (pos != copasi2sbmlmap.end())
    {
      sbmlSpecies = dynamic_cast<Species*>(pos->second);
      assert(sbmlSpecies);
    }
  else
    {
      sbmlSpecies = new Species();
      copasi2sbmlmap[copasiMetabolite] = sbmlSpecies;
      std::string id = copasiMetabolite->getSBMLId();
      if (id == "")
        {
          id = SBMLExporter::createUniqueId(this->mpIdSet, "species_");
          copasiMetabolite->setSBMLId(id);
          this->mpIdSet->insert(id);
        }
      sbmlSpecies->setId(id);
    }
  this->mHandledSBMLObjects.insert(sbmlSpecies);
  sbmlSpecies->setName(copasiMetabolite->getObjectName().c_str());
  sbmlSpecies->setBoundaryCondition(copasiMetabolite->getStatus() == CModelEntity::FIXED);
  sbmlSpecies->setConstant(copasiMetabolite->getStatus() == CModelEntity::FIXED);

  Compartment* sbmlCompartment = dynamic_cast<Compartment*>(copasi2sbmlmap[const_cast<CCompartment*>(copasiMetabolite->getCompartment())]);
  assert(sbmlCompartment);
  sbmlSpecies->setCompartment(sbmlCompartment->getId());
  double value = copasiMetabolite->getInitialConcentration();
  // if the value is NaN, unset the initial amount
  if (!isnan(value))
    {
      if (sbmlSpecies->isSetInitialAmount())
        {
          sbmlSpecies->setInitialAmount(value*copasiMetabolite->getCompartment()->getInitialValue());
        }
      else
        {
          sbmlSpecies->setInitialConcentration(value);
        }
    }
  else
    {
      sbmlSpecies->unsetInitialConcentration();
      sbmlSpecies->unsetInitialAmount();
    }
  return sbmlSpecies;
}

/**
 ** This method takes a pointer to a copasi CModelValue object and creates a SBML
 ** Parameter object. The pointer to the parameter object is returned.
 */
Parameter* SBMLExporter::createSBMLParameterFromCModelValue(CModelValue* pModelValue, CCopasiDataModel* pDataModel)
{
  Parameter* pParameter = NULL;
  std::map<CCopasiObject*, SBase*>& copasi2sbmlmap = pDataModel->getCopasi2SBMLMap();
  std::map<CCopasiObject*, SBase*>::iterator pos = copasi2sbmlmap.find(pModelValue);
  if (pos != copasi2sbmlmap.end())
    {
      pParameter = dynamic_cast<Parameter*>(pos->second);
      assert(pParameter);
    }
  else
    {
      pParameter = new Parameter();
      copasi2sbmlmap[pModelValue] = pParameter;
      std::string id = pModelValue->getSBMLId();
      if (id == "")
        {
          id = SBMLExporter::createUniqueId(this->mpIdSet, "parameter_");
          pModelValue->setSBMLId(id);
          this->mpIdSet->insert(id);
        }
      pParameter->setId(id);
    }
  this->mHandledSBMLObjects.insert(pParameter);
  pParameter->setName(pModelValue->getObjectName());
  double value = pModelValue->getInitialValue();
  // if the value is NaN, unset the parameters value
  if (!isnan(value))
    {
      pParameter->setValue(value);
    }
  else
    {
      pParameter->unsetValue();
    }
  return pParameter;
}

/**
 ** This method takes a pointer to a copasi CReaction object and creates an
 ** SBML Reaction object. The pointer to the created reaction object is
 ** returned.
 */
Reaction* SBMLExporter::createSBMLReactionFromCReaction(CReaction* copasiReaction, CCopasiDataModel* pDataModel)
{
  Reaction* sbmlReaction = NULL;
  std::map<CCopasiObject*, SBase*>& copasi2sbmlmap = pDataModel->getCopasi2SBMLMap();
  std::map<CCopasiObject*, SBase*>::iterator pos = copasi2sbmlmap.find(copasiReaction);
  if (pos != copasi2sbmlmap.end())
    {
      sbmlReaction = dynamic_cast<Reaction*>(pos->second);
      assert(sbmlReaction);
    }
  else
    {
      /* create a new reaction object */
      sbmlReaction = new Reaction();
      copasi2sbmlmap[copasiReaction] = sbmlReaction;
      std::string id = copasiReaction->getSBMLId();
      if (id == "")
        {
          id = SBMLExporter::createUniqueId(this->mpIdSet, "reaction_");
          copasiReaction->setSBMLId(id);
          this->mpIdSet->insert(id);
        }
      sbmlReaction->setId(id);
    }
  this->mHandledSBMLObjects.insert(sbmlReaction);
  sbmlReaction->setName(copasiReaction->getObjectName().c_str());
  sbmlReaction->setReversible(copasiReaction->isReversible());
  const CChemEq chemicalEquation = copasiReaction->getChemEq();
  /* Add all substrates */
  unsigned int counter;
  std::set<std::string> usedReferences;
  for (counter = 0; counter < chemicalEquation.getSubstrates().size(); counter++)
    {
      CChemEqElement* element = chemicalEquation.getSubstrates()[counter];
      const CMetab* pMetabolite = element->getMetabolite(); assert(pMetabolite);
      SpeciesReference* sRef = NULL;
      if (!(sRef = sbmlReaction->getReactant(pMetabolite->getSBMLId())))
        {
          sRef = new SpeciesReference();
          sRef->setSpecies(pMetabolite->getSBMLId().c_str());
          sbmlReaction->addReactant(sRef);
          delete sRef;
        }
      sRef->setStoichiometry(element->getMultiplicity());
      sRef->setDenominator(1);
      usedReferences.insert(sRef->getSpecies());
    }
  ListOf* l = sbmlReaction->getListOfReactants();
  for (counter = l->size(); counter > 0;--counter)
    {
      if (usedReferences.find(static_cast<SimpleSpeciesReference*>(l->get(counter - 1))->getSpecies()) == usedReferences.end())
        {
          l->remove(counter - 1);
        }
    }
  /* Add all products */
  usedReferences.clear();
  for (counter = 0; counter < chemicalEquation.getProducts().size(); counter++)
    {
      CChemEqElement* element = chemicalEquation.getProducts()[counter];
      const CMetab* pMetabolite = element->getMetabolite(); assert(pMetabolite);
      SpeciesReference* sRef = NULL;
      if (!(sRef = sbmlReaction->getProduct(pMetabolite->getSBMLId())))
        {
          sRef = new SpeciesReference();
          sRef->setSpecies(pMetabolite->getSBMLId().c_str());
          sbmlReaction->addProduct(sRef);
          delete sRef;
        }
      sRef->setStoichiometry(element->getMultiplicity());
      sRef->setDenominator(1);
      usedReferences.insert(sRef->getSpecies());
    }
  l = sbmlReaction->getListOfProducts();
  for (counter = l->size(); counter > 0;--counter)
    {
      if (usedReferences.find(static_cast<SimpleSpeciesReference*>(l->get(counter - 1))->getSpecies()) == usedReferences.end())
        {
          l->remove(counter - 1);
        }
    }
  /* Add all modifiers */
  usedReferences.clear();
  for (counter = 0; counter < chemicalEquation.getModifiers().size(); counter++)
    {
      CChemEqElement* element = chemicalEquation.getModifiers()[counter];
      const CMetab* pMetabolite = element->getMetabolite(); assert(pMetabolite);
      ModifierSpeciesReference* sRef = NULL;
      if (!(sRef = sbmlReaction->getModifier(pMetabolite->getSBMLId())))
        {
          sRef = new ModifierSpeciesReference();
          sRef->setSpecies(pMetabolite->getSBMLId().c_str());
          sbmlReaction->addModifier(sRef);
          delete sRef;
        }
      usedReferences.insert(sRef->getSpecies());
    }
  l = sbmlReaction->getListOfModifiers();
  for (counter = l->size(); counter > 0;--counter)
    {
      if (usedReferences.find(static_cast<SimpleSpeciesReference*>(l->get(counter - 1))->getSpecies()) == usedReferences.end())
        {
          l->remove(counter - 1);
        }
    }
  /* create the kinetic law */
  /* if there is one on copasi */
  if ((copasiReaction->getFunction()) != pDataModel->mpUndefined)
    {
      KineticLaw* kLaw = this->createSBMLKineticLawFromCReaction(copasiReaction, pDataModel);
      sbmlReaction->setKineticLaw(kLaw);
      delete kLaw;
    }
  else
    {
      sbmlReaction->unsetKineticLaw();
    }
  return sbmlReaction;
}

/**
 ** This method takes a pointer to a copasi CReation object and creates a
 ** SBML KineticLaw object from the kintik function of the copasi reaction
 ** object. The pointer to the created KineticLaw is returned.
 */
KineticLaw* SBMLExporter::createSBMLKineticLawFromCReaction(CReaction* copasiReaction, CCopasiDataModel* pDataModel)
{
  KineticLaw* kLaw = NULL;
  /*
   * This code can not be activated yet since roundtripping of kinetcs is not so trivial.
   *
  std::map<CCopasiObject*,SBase*>& copasi2sbmlmap=pDataModel->getCopasi2SBMLMap();
  std::map<CCopasiObject*,SBase*>::iterator pos=copasi2sbmlmap.find(copasiReaction);
  if(pos!=copasi2sbmlmap.end())
  {
    Reaction* pTmpReaction=dynamic_cast<Reaction*>(pos->second);
    assert(pTmpReaction);
    kLaw=pTMpReaction->getKineticLaw();
  }
  */
  if (!kLaw)
    {
      /* create a new KineticLaw */
      kLaw = new KineticLaw();
    }
  /* if the copasi CFunction specifies a mass-action kinetic */
  ASTNode* node = NULL;
  if (copasiReaction->getFunction()->getType() == CFunction::MassAction)
    {
      const CMassAction cMassAction = *static_cast<const CMassAction*>(copasiReaction->getFunction());
      /* create the ASTNode that multiplies all substrates with the first
      ** kinetic constant.
      */
      node = new ASTNode(AST_TIMES);

      ASTNode* parameterNode1 = new ASTNode(AST_NAME);
      std::string parameterName1;
      if (copasiReaction->isLocalParameter(0))
        {
          parameterName1 = cMassAction.getVariables()[0]->getObjectName();
        }
      else
        {
          parameterName1 = copasiReaction->getParameterMappings()[0][0];
          const CModelValue* pMV = dynamic_cast<const CModelValue*>(GlobalKeys.get(parameterName1));
          if (!pMV) fatalError();
          parameterName1 = pMV->getSBMLId();
        }
      parameterNode1->setName(parameterName1.c_str());
      node->addChild(parameterNode1);
      node->addChild(this->createTimesTree(copasiReaction->getChemEq().getSubstrates()));
      /* if the reaction is reversible, create the ASTNode tree that
      ** multiplies all products with the second kinetic constant and
      ** subtract this tree from the tree of the forward reaction.
      */
      if (cMassAction.isReversible() == TriTrue)
        {
          ASTNode* backwardNode = new ASTNode(AST_TIMES);

          ASTNode* parameterNode2 = new ASTNode(AST_NAME);
          std::string parameterName2;
          if (copasiReaction->isLocalParameter(2))
            {
              parameterName2 = cMassAction.getVariables()[2]->getObjectName();
            }
          else
            {
              parameterName2 = copasiReaction->getParameterMappings()[2][0];
              const CModelValue* pMV = dynamic_cast<const CModelValue*>(GlobalKeys.get(parameterName2));
              if (!pMV) fatalError();
              parameterName2 = pMV->getSBMLId();
            }
          parameterNode2->setName(parameterName2.c_str());
          backwardNode->addChild(parameterNode2);

          backwardNode->addChild(this->createTimesTree(copasiReaction->getChemEq().getProducts()));
          ASTNode* tempNode = new ASTNode(AST_MINUS);
          tempNode->addChild(node);
          tempNode->addChild(backwardNode);
          node = tempNode;
        }
    }
  /* if the copasi CFunction does not specify a mass-action kinetic, it is a
  ** CKinFunction */
  else
    {
      // walk the tree and look for function calls
      if (!this->mExportExpressions)
        {
          CEvaluationTree* pTree = new CEvaluationTree("NoName", NULL, CEvaluationTree::Expression);
          CEvaluationNodeCall* pTmpRoot = new CEvaluationNodeCall(CEvaluationNodeCall::FUNCTION, copasiReaction->getFunction()->getObjectName());
          const std::vector< std::vector<std::string> >& parameterMappings = copasiReaction->getParameterMappings();
          unsigned int i, iMax = parameterMappings.size();
          for (i = 0;i < iMax;++i)
            {
              CEvaluationNodeObject* pObjectNode = new CEvaluationNodeObject(CEvaluationNodeObject::ANY, "<" + GlobalKeys.get(parameterMappings[i][0])->getCN() + ">");
              pTmpRoot->addChild(pObjectNode);
            }
          pTree->setRoot(pTmpRoot);
          pTree->compile();
          std::list<const CEvaluationTree*>* usedFunctionList = new std::list<const CEvaluationTree*>;
          findUsedFunctions(pTmpRoot, usedFunctionList, pDataModel, this->mpUsedFunctions, this->mpIdSet);
          pdelete(usedFunctionList);
          node = pTmpRoot->toAST();
          pdelete(pTree);
        }
      else
        {
          CEvaluationNode* pExpressionRoot = this->createExpressionTree(copasiReaction->getFunction(), copasiReaction->getParameterMappings(), pDataModel);
          if (!pExpressionRoot) fatalError();
          node = pExpressionRoot->toAST();
          if (!node) fatalError();
        }
    }

  /*
   ** If the reaction takes place in a single compartment, the rate law has
   ** to be converted from concentration/time to substance/time by
   ** multiplying the rate law with the volume of the compartment.
   */
  if (copasiReaction->getCompartmentNumber() == 1)
    {
      //TODO: the whole next paragraph (finding the compartment)
      //should rather be a method of CReaction (or CChemEq)
      const CCompartment* compartment = NULL;
      if (copasiReaction->getChemEq().getSubstrates().size() != 0)
        {
          assert(copasiReaction->getChemEq().getSubstrates()[0]->getMetabolite());
          compartment = copasiReaction->getChemEq().getSubstrates()[0]->getMetabolite()->getCompartment();
        }
      else
        {
          assert(copasiReaction->getChemEq().getProducts().size());
          assert(copasiReaction->getChemEq().getProducts()[0]->getMetabolite());
          compartment = copasiReaction->getChemEq().getProducts()[0]->getMetabolite()->getCompartment();
        }

      // check if the importer has added a division by the volume
      // if so remove it instead of multiplying again
      ASTNode* tNode = this->isDividedByVolume(node, compartment->getSBMLId());
      if (tNode)
        {
          if (tNode->getNumChildren() == 0)
            {
              fatalError();
            }
          if (tNode->getNumChildren() == 1)
            {
              ASTNode* tmp = static_cast<ConverterASTNode*>(tNode)->removeChild(0);
              delete tNode;
              tNode = tmp;
            }
          delete node;
          node = tNode;
        }
      else
        {
          tNode = new ASTNode(AST_TIMES);
          ASTNode* vNode = new ASTNode(AST_NAME);
          vNode->setName(compartment->getSBMLId().c_str());
          tNode->addChild(vNode);
          tNode->addChild(node);
          node = tNode;
        }
    }

  kLaw->setMath(node);
  /* add the parameters */
  unsigned int counter;
  for (counter = 0; counter < copasiReaction->getFunctionParameters().size(); counter++)
    {
      const CFunctionParameter* para = copasiReaction->getFunctionParameters()[counter];
      if (para->getUsage() == CFunctionParameter::PARAMETER)
        {
          // only create a parameter if it is a local parameter,
          // otherwise the parameter already has been created
          if (copasiReaction->isLocalParameter(counter))
            {
              Parameter sbmlPara = Parameter();

              std::string parameterKey = copasiReaction->getParameterMappings()[counter][0];

              sbmlPara.setId(para->getObjectName().c_str());
              double value = copasiReaction->getParameterValue(para->getObjectName());
              // if the value is NaN, leave the parameter value unset.
              if (!isnan(value))
                {
                  sbmlPara.setValue(value);
                }
              kLaw->addParameter(&sbmlPara);
            }
          /* the code below is responsible for bug 646, that's why I put it in comments
             Currently I don't know why it is there at all. RG
          else
            {
              // the corresponding node has to be changed
              std::string modelValueKey = copasiReaction->getParameterMappings()[counter][0];
              this->replaceNodeName(node, para->getObjectName(), modelValueKey);
            }
            */
        }
    }
  return kLaw;
}

/**
 ** This method creates an ASTNode tree where all the species specified in
 ** the given vector are multiplied. This is used to create the mass action
 ** kinetic law.
 */
ASTNode* SBMLExporter::createTimesTree(const CCopasiVector<CChemEqElement >& vect, unsigned int pos)
{
  ASTNode* node = NULL;
  double multiplicity = vect[pos]->getMultiplicity();
  if (pos == vect.size() - 1)
    {
      node = new ASTNode(AST_NAME);
      const CMetab* pMetab = vect[pos]->getMetabolite();
      assert(pMetab);

      node->setName(pMetab->getSBMLId().c_str());
      /* if the stoichiometry is not 1.0, we have to add it to the exponent */
      if (multiplicity != 1.0)
        {
          ASTNode* tmpNode1 = new ASTNode(AST_POWER);
          ASTNode* tmpNode2 = new ASTNode(AST_REAL);
          tmpNode2->setValue(multiplicity);
          tmpNode1->addChild(node);
          tmpNode1->addChild(tmpNode2);
          node = tmpNode1;
        }
    }
  else
    {
      node = new ASTNode(AST_TIMES);
      ASTNode* child = new ASTNode(AST_NAME);
      const CMetab* pMetab = vect[pos]->getMetabolite();
      assert(pMetab);

      child->setName(pMetab->getSBMLId().c_str());

      /* if the stoichiometry is not 1.0, we have to add it to the exponent */
      if (multiplicity != 1.0)
        {
          ASTNode* tmpNode1 = new ASTNode(AST_POWER);
          ASTNode* tmpNode2 = new ASTNode(AST_REAL);
          tmpNode2->setValue(multiplicity);
          tmpNode1->addChild(child);
          tmpNode1->addChild(tmpNode2);
          child = tmpNode1;
        }

      node->addChild(child);
      node->addChild(this->createTimesTree(vect, pos + 1));
    }
  return node;
}

/**
 ** This method tests if a string only consists of whitespace characters
 */
bool SBMLExporter::isEmptyString(const std::string& str)
{
  return (str.find_first_not_of(" \n\t\r") == std::string::npos);
}

void SBMLExporter::replaceNodeName(ASTNode* pNode, const std::string& oldName, const std::string& newName)
{
  if (pNode->isName() && pNode->getName() == oldName)
    {
      pNode->setName(newName.c_str());
    }
  unsigned int i;
  unsigned int maxI = pNode->getNumChildren();
  for (i = 0; i < maxI;++i)
    {
      replaceNodeName(pNode->getChild(i), oldName, newName);
    }
}

void SBMLExporter::removeUnusedObjects(CCopasiDataModel* pDataModel)
{
  // go through the sbmlDocument and remove all species, compartments, parameters and reactions
  // that are not in mHandledSBMLObjects because this means they are no longer in the copasi model.
  // also remove all rules where the corresponding species, parameter or
  // compartment is no longer in the copasi model

  // first we need to turn around the copasi2sbmlmap in order to remove entries
  // of objects that are no longer there
  std::map<SBase*, CCopasiObject*> sbml2copasimap;
  std::map<CCopasiObject*, SBase*>& copasi2sbmlmap = pDataModel->getCopasi2SBMLMap();
  std::map<CCopasiObject*, SBase*>::const_iterator it = copasi2sbmlmap.begin(), endit = copasi2sbmlmap.end();
  while (it != endit)
    {
      sbml2copasimap.insert(std::make_pair(it->second, it->first));
      ++it;
    }
  std::map<SBase*, CCopasiObject*>::const_iterator sbml2copasimap_pos , sbml2copasimap_endpos = sbml2copasimap.end();
  Model* model = this->sbmlDocument->getModel();
  assert(model);
  unsigned int i;
  std::set<SBase*>::iterator pos;
  std::set<SBase*>::iterator endPos = this->mHandledSBMLObjects.end();
  ListOf* list = model->getListOfCompartments();
  for (i = list->size(); i > 0;--i)
    {
      SBase* object = list->get(i - 1);
      pos = this->mHandledSBMLObjects.find(object);
      if (pos == endPos)
        {
          list->remove(i - 1);
          // clean the entry from the copasi2sbmlmap
          sbml2copasimap_pos = sbml2copasimap.find(object);
          if (sbml2copasimap_pos != sbml2copasimap_endpos)
            {
              copasi2sbmlmap.erase(copasi2sbmlmap.find(sbml2copasimap_pos->second));
            }
          pdelete(object);
        }
    }
  list = model->getListOfSpecies();
  for (i = list->size(); i > 0;--i)
    {
      SBase* object = list->get(i - 1);
      pos = this->mHandledSBMLObjects.find(object);
      if (pos == endPos)
        {
          list->remove(i - 1);
          // clean the entry from the copasi2sbmlmap
          sbml2copasimap_pos = sbml2copasimap.find(object);
          if (sbml2copasimap_pos != sbml2copasimap_endpos)
            {
              copasi2sbmlmap.erase(copasi2sbmlmap.find(sbml2copasimap_pos->second));
            }
          pdelete(object);
        }
    }
  list = model->getListOfParameters();
  for (i = list->size(); i > 0;--i)
    {
      SBase* object = list->get(i - 1);
      pos = this->mHandledSBMLObjects.find(object);
      if (pos == endPos)
        {
          list->remove(i - 1);
          // clean the entry from the copasi2sbmlmap
          sbml2copasimap_pos = sbml2copasimap.find(object);
          if (sbml2copasimap_pos != sbml2copasimap_endpos)
            {
              copasi2sbmlmap.erase(copasi2sbmlmap.find(sbml2copasimap_pos->second));
            }
          pdelete(object);
        }
    }
  list = model->getListOfReactions();
  for (i = list->size(); i > 0;--i)
    {
      SBase* object = list->get(i - 1);
      pos = this->mHandledSBMLObjects.find(object);
      if (pos == endPos)
        {
          list->remove(i - 1);
          // clean the entry from the copasi2sbmlmap
          sbml2copasimap_pos = sbml2copasimap.find(object);
          if (sbml2copasimap_pos != sbml2copasimap_endpos)
            {
              copasi2sbmlmap.erase(copasi2sbmlmap.find(sbml2copasimap_pos->second));
            }
          pdelete(object);
        }
    }
  this->removeUnusedFunctionDefinitions();
  this->removeUnusedUnitDefinitions();
}

void SBMLExporter::removeUnusedFunctionDefinitions()
{
  // does nothing yet
  // this can only be done one we understand events and rules and function calls
}

void SBMLExporter::removeUnusedUnitDefinitions()
{
  // does nothing yet
  // this can only be done one we understand events and rules
}

std::set<std::string>* SBMLExporter::createIdSet(const Model* pSBMLModel, CCopasiDataModel* pDataModel)
{
  // go through all function definitions, compartments, species, reactions,
  // parameters and events and create a set with all used ids.
  // Later on, we need to add the species references as well.

  std::set<std::string>* pIdSet = NULL;
  if (pDataModel != NULL)
    {
      if (pSBMLModel != NULL)
        {
          pIdSet = new std::set<std::string>();
          unsigned int i, iMax;
          iMax = pSBMLModel->getNumFunctionDefinitions();
          for (i = 0; i < iMax;++i)
            {
              pIdSet->insert(pSBMLModel->getFunctionDefinition(i)->getId());
            }
          iMax = pSBMLModel->getNumCompartments();
          for (i = 0; i < iMax;++i)
            {
              pIdSet->insert(pSBMLModel->getCompartment(i)->getId());
            }
          iMax = pSBMLModel->getNumSpecies();
          for (i = 0; i < iMax;++i)
            {
              pIdSet->insert(pSBMLModel->getSpecies(i)->getId());
            }
          iMax = pSBMLModel->getNumParameters();
          for (i = 0; i < iMax;++i)
            {
              pIdSet->insert(pSBMLModel->getParameter(i)->getId());
            }
          iMax = pSBMLModel->getNumReactions();
          for (i = 0; i < iMax;++i)
            {
              pIdSet->insert(pSBMLModel->getReaction(i)->getId());
            }
          iMax = pSBMLModel->getNumEvents();
          for (i = 0; i < iMax;++i)
            {
              pIdSet->insert(pSBMLModel->getEvent(i)->getId());
            }
        }
      else
        {
          pIdSet = new std::set<std::string>();
          std::string id;
          unsigned int i, iMax;
          CCopasiVectorN<CEvaluationTree>& loadedFunctions = pDataModel->getFunctionList()->loadedFunctions();
          iMax = loadedFunctions.size();
          for (i = 0;i < iMax;++i)
            {
              id = loadedFunctions[i]->getSBMLId();
              if (id != "") pIdSet->insert(id);
            }
          const CModel* pModel = pDataModel->getModel();
          if (pModel != NULL)
            {
              iMax = pModel->getNumMetabs();
              for (i = 0; i < iMax;++i)
                {
                  id = pModel->getMetabolites()[i]->getSBMLId();
                  if (id != "") pIdSet->insert(id);
                }
              iMax = pModel->getNumModelValues();
              for (i = 0; i < iMax;++i)
                {
                  id = pModel->getModelValues()[i]->getSBMLId();
                  if (id != "") pIdSet->insert(id);
                }
              iMax = pModel->getCompartments().size();
              for (i = 0; i < iMax;++i)
                {
                  id = pModel->getCompartments()[i]->getSBMLId();
                  if (id != "") pIdSet->insert(id);
                }
              iMax = pModel->getReactions().size();
              for (i = 0; i < iMax;++i)
                {
                  id = pModel->getReactions()[i]->getSBMLId();
                  if (id != "") pIdSet->insert(id);
                }
            }
        }
    }
  return pIdSet;
}

std::string SBMLExporter::createUniqueId(const std::set<std::string>* idSet, const std::string& prefix)
{
  unsigned int i = 0;
  std::ostringstream numberStream;
  numberStream << i;
  std::string id = prefix + numberStream.str();
  if (idSet)
    {
      while (idSet->find(id) != idSet->end())
        {
          ++i;
          numberStream.str("");
          numberStream << i;
          id = prefix + numberStream.str();
        }
    }
  return id;
}

ASTNode* SBMLExporter::isDividedByVolume(const ASTNode* node, const std::string& compartmentSBMLId)
{
  ASTNode* result = NULL;
  if (node->getType() == AST_DIVIDE || node->getType() == AST_TIMES)
    {
      ASTNode* pTmpResultNode = new ConverterASTNode(node->getType());
      unsigned int i, iMax = node->getNumChildren();
      bool found = false;
      for (i = 0; i < iMax;++i)
        {
          const ASTNode* child = node->getChild(i);
          if (node->getType() == AST_DIVIDE && child->getType() == AST_NAME && child->getName() == compartmentSBMLId)
            {
              found = true;
            }
          else if ((!found) && (child->getType() == AST_DIVIDE || child->getType() == AST_TIMES))
            {
              ASTNode* pSubResult = this->isDividedByVolume(child, compartmentSBMLId);
              if (pSubResult)
                {
                  found = true;
                  if (pSubResult->getNumChildren() > 1)
                    {
                      pTmpResultNode->addChild(pSubResult);
                    }
                  else if (pSubResult->getNumChildren() == 1)
                    {
                      pTmpResultNode->addChild(static_cast<ASTNode*>(static_cast<ConverterASTNode*>(pSubResult)->removeChild(0)));
                      delete pSubResult;
                    }
                  else
                    {
                      delete pSubResult;
                    }
                }
              else
                {
                  pTmpResultNode->addChild(new ConverterASTNode(*child));
                }
            }
          else
            {
              pTmpResultNode->addChild(new ConverterASTNode(*child));
            }
        }
      if (found)
        {
          result = pTmpResultNode;
        }
      else
        {
          delete pTmpResultNode;
        }
    }
  return result;
}

void SBMLExporter::findUsedFunctions(const CEvaluationNode* pNode, std::list<const CEvaluationTree*>* usedFunctionList, CCopasiDataModel* pDataModel, std::list<const CEvaluationTree*>* pKnownUsedFunctions, std::set<std::string>* pIdSet)
{
  CFunctionDB* pFunDB = pDataModel->getFunctionList();
  CCopasiTree<CEvaluationNode>::const_iterator treeIt = pNode;
  CCopasiTree<CEvaluationNode>::const_iterator treeEndIt = NULL;

  while (treeIt != treeEndIt)
    {
      if (CEvaluationNode::type((*treeIt).getType()) == CEvaluationNode::CALL)
        {
          // find used function used in the corresponding function
          CEvaluationTree* pFun = pFunDB->findFunction((*treeIt).getData());
          if (!pFun)
            {
              CCopasiMessage(CCopasiMessage::EXCEPTION, MCSBML + 15, (*treeIt).getData().c_str());
            }
          if (existsInList(pFun, usedFunctionList))
            {
              // we have a loop
              CCopasiMessage(CCopasiMessage::EXCEPTION, MCSBML + 16);
            }
          // if the function is already in the list of used functions, we don't have
          // to go through it again.
          usedFunctionList->push_back(pFun);
          if (!existsInList(pFun, pKnownUsedFunctions))
            {
              std::list<const CEvaluationTree*>::iterator pos = usedFunctionList->end();
              --pos;
              findUsedFunctions(pFun->getRoot(), usedFunctionList, pDataModel, pKnownUsedFunctions, pIdSet);
              // add this function to mpUsedFunctions after the last function in usedFunctionList
              // if usedFunctionList does not have any entries after pos, just insert the function
              // at the beginning
              if (usedFunctionList->back() == (*pos))
                {
                  if (pFun->getSBMLId().empty())
                    {
                      if (isValidSId(pFun->getObjectName()) && pIdSet->find(pFun->getObjectName()) == pIdSet->end())
                        {
                          pFun->setSBMLId(pFun->getObjectName());
                        }
                      else
                        {
                          std::string id = createUniqueId(pIdSet, "function_");
                          pFun->setSBMLId(id);
                          pIdSet->insert(id);
                        }
                    }
                  pKnownUsedFunctions->push_front(pFun);
                }
              else
                {
                  std::list<const CEvaluationTree*>::iterator it = pKnownUsedFunctions->begin();
                  std::list<const CEvaluationTree*>::iterator endIt = pKnownUsedFunctions->end();
                  std::list<const CEvaluationTree*>::iterator afterPos = pKnownUsedFunctions->end();
                  while ((it != endIt) && (usedFunctionList->back() != (*pos)))
                    {
                      // pos is the position of the current function in usedFunctionList
                      // so find the function after pos in usedFunctions that has the position
                      // furthest to the back of this->mpUsedFunctions
                      std::list<const CEvaluationTree*>::iterator it2 = pos;
                      std::list<const CEvaluationTree*>::iterator endIt2 = usedFunctionList->end();
                      while (it2 != endIt2)
                        {
                          if ((*it) == (*it2))
                            {
                              usedFunctionList->erase(it2);
                              afterPos = it;
                              break;
                            }
                          ++it2;
                        }
                      ++it;
                    }
                  if (afterPos == pKnownUsedFunctions->end())
                    {
                      fatalError();
                    }
                  ++afterPos;
                  if (pFun->getSBMLId().empty())
                    {
                      if (isValidSId(pFun->getObjectName()) && pIdSet->find(pFun->getObjectName()) == pIdSet->end())
                        {
                          pFun->setSBMLId(pFun->getObjectName());
                        }
                      else
                        {
                          std::string id = createUniqueId(pIdSet, "function_");
                          pFun->setSBMLId(id);
                          pIdSet->insert(id);
                        }
                    }
                  pKnownUsedFunctions->insert(afterPos, pFun);
                }
            }
        }
      ++treeIt;
    }
}

void SBMLExporter::createFunctionDefinitions()
{
  ListOf* listOfFunctionDefinitions = this->sbmlDocument->getModel()->getListOfFunctionDefinitions();
  std::list<const CEvaluationTree*>::const_iterator it = this->mpUsedFunctions->begin();
  std::list<const CEvaluationTree*>::const_iterator endIt = this->mpUsedFunctions->end();
  while (it != endIt)
    {
      // delete an existing function definition with this name
      // add the new function definition
      unsigned int i, iMax = listOfFunctionDefinitions->size();
      for (i = 0; i < iMax;++i)
        {
          if (static_cast<FunctionDefinition*>(listOfFunctionDefinitions->get(i))->getId() == (*it)->getSBMLId())
            {
              listOfFunctionDefinitions->remove(i);
              break;
            }
        }
      const CEvaluationTree* tree = (*it);
      std::string on = tree->getObjectName();
      std::string id = tree->getSBMLId();
      this->createSBMLFunctionDefinitionFromCEvaluationTree((*it));
      ++it;
    }
}

FunctionDefinition* SBMLExporter::createSBMLFunctionDefinitionFromCEvaluationTree(const CEvaluationTree* tree)
{
  // convert the tree root to an AST tree.
  FunctionDefinition* pFunDef = this->sbmlDocument->getModel()->createFunctionDefinition();
  pFunDef->setId(tree->getSBMLId());
  pFunDef->setName(tree->getObjectName());
  if (!tree->getRoot())
    {
      std::string errorMessage = std::string("Can not export function");
      errorMessage += tree->getObjectName();
      errorMessage += std::string(". Function does not have a valid root node.");
      CCopasiMessage(CCopasiMessage::EXCEPTION, errorMessage.c_str());
    }
  ASTNode* pFunNode = tree->getRoot()->toAST();
  // go through the AST tree and replace all function call nodes with with a call to the sbml id
  ASTNode* pLambda = new ASTNode(AST_LAMBDA);
  // add the parameters to the function definition
  const CFunctionParameters& funParams = static_cast<const CFunction*>(tree)->getVariables();
  unsigned int i, iMax = funParams.size();
  ASTNode* pParamNode = NULL;
  for (i = 0; i < iMax;++i)
    {
      pParamNode = new ASTNode(AST_NAME);
      pParamNode->setName(funParams[i]->getObjectName().c_str());
      pLambda->addChild(pParamNode);
    }
  pLambda->addChild(pFunNode);
  pFunDef->setMath(pLambda);
  return pFunDef;
}

bool SBMLExporter::existsInList(const CEvaluationTree* tree, const std::list<const CEvaluationTree*>* pList)
{
  std::list<const CEvaluationTree*>::const_iterator it = pList->begin();
  std::list<const CEvaluationTree*>::const_iterator endIt = pList->end();
  while (it != endIt)
    {
      if ((*it) == tree) break;
      ++it;
    }
  return it != endIt;
}

SBMLDocument* SBMLExporter::getSBMLDocument() const
  {
    return this->sbmlDocument;
  }

bool SBMLExporter::isValidSId(const std::string& id)
{
  bool result = true;
  if (id.length() > 0)
    {
      char c = id[0];
      result = (c == '_' || (c > 64 && c < 91) || (c > 96 && c < 123));
      unsigned int i, iMax = id.length();
      for (i = 1;(i < iMax) && result;++i)
        {
          c = id[i];
          result = (c == '_' || (c > 64 && c < 91) || (c > 96 && c < 123) || (c > 47 && c < 58));
        }
    }
  else
    {
      result = false;
    }
  return result;
}

CProcessReport* SBMLExporter::getExportHandler()
{
  return this->mpExportHandler;
}

void SBMLExporter::setExportHandler(CProcessReport* pExportHandler)
{
  this->mpExportHandler = pExportHandler;
}

void SBMLExporter::removeFromList(ListOf * pList, SBase* pObject)
{
  unsigned i, iMax = pList->size();
  for (i = 0;i < iMax;++i)
    {
      if (pList->get(i) == pObject)
        {
          pList->remove(i);
          pdelete(pObject);
          break;
        }
    }
}

CEvaluationNode* SBMLExporter::createExpressionTree(const CFunction* const pFun, const std::vector<std::vector<std::string> >& arguments, CCopasiDataModel* pDataModel)
{
  if (!pFun || pFun->getVariables().size() != arguments.size()) fatalError();
  std::map< std::string, std::string > parameterMap;
  unsigned int i, iMax = arguments.size();
  for (i = 0;i < iMax;++i)
    {
      if (arguments[i].size() != 1) fatalError(); // we can't have arrays here.
      const CCopasiObject* pObject = GlobalKeys.get(arguments[i][0]);
      if (!pObject) fatalError();
      parameterMap[pFun->getVariables()[i]->getObjectName()] = "<" + pObject->getCN() + ">";
    }
  return this->createExpressionTree(pFun, parameterMap, pDataModel);
}

CEvaluationNode* SBMLExporter::createExpressionTree(const CFunction* const pFun, const std::map<std::string, std::string>& parameterMap, CCopasiDataModel* pDataModel)
{
  if (!pFun) fatalError();
  return this->createExpressionTree(pFun->getRoot(), parameterMap, pDataModel);
}

CEvaluationNode* SBMLExporter::createExpressionTree(const CEvaluationNode* const pNode, const std::map<std::string, std::string>& parameterMap, CCopasiDataModel* pDataModel)
{
  if (!pNode) fatalError();
  CEvaluationNode* pResultNode = NULL;
  const CFunction* pFun = NULL;
  std::map<std::string, std::string>::const_iterator pos;
  std::vector<std::vector<std::string> > arguments;
  const CEvaluationNode* pChildNode = NULL;
  std::vector<CCopasiContainer*> containerList;
  containerList.push_back(this->mpCopasiModel);
  const CCopasiObject* pObject = NULL;
  switch (CEvaluationNode::type(pNode->getType()))
    {
    case CEvaluationNode::CALL:
      pFun = dynamic_cast<const CFunction*>(pDataModel->getFunctionList()->findFunction(pNode->getData()));
      assert(pFun);
      pChildNode = static_cast<const CEvaluationNode*>(pNode->getChild());
      while (pChildNode)
        {
          pos = parameterMap.find(pChildNode->getData());
          if (pos == parameterMap.end()) fatalError();
          pObject = CCopasiContainer::ObjectFromName(containerList, pos->second.substr(1, pos->second.size() - 2));
          assert(pObject);
          arguments.push_back(std::vector<std::string>());
          if (pObject->isReference())
            {
              pObject = pObject->getObjectParent();
              assert(pObject);
            }
          arguments[arguments.size() - 1].push_back(pObject->getKey());
          pChildNode = static_cast<const CEvaluationNode*>(pChildNode->getSibling());
        }
      pResultNode = this->createExpressionTree(pFun, arguments, pDataModel);
      break;
    case CEvaluationNode::VARIABLE:
      // replace the variable node with an object node
      pos = parameterMap.find(pNode->getData());
      if (pos == parameterMap.end()) fatalError();
      pResultNode = new CEvaluationNodeObject(CEvaluationNodeObject::ANY, pos->second);
      break;
    case CEvaluationNode::NUMBER:
      pResultNode = new CEvaluationNodeNumber((CEvaluationNodeNumber::SubType)CEvaluationNode::subType(pNode->getType()), pNode->getData());
      break;
    case CEvaluationNode::CONSTANT:
      pResultNode = new CEvaluationNodeConstant((CEvaluationNodeConstant::SubType)CEvaluationNode::subType(pNode->getType()), pNode->getData());
      break;
    case CEvaluationNode::OPERATOR:
      pResultNode = new CEvaluationNodeOperator((CEvaluationNodeOperator::SubType)CEvaluationNode::subType(pNode->getType()), pNode->getData());
      break;
    case CEvaluationNode::CHOICE:
      pResultNode = new CEvaluationNodeChoice((CEvaluationNodeChoice::SubType)CEvaluationNode::subType(pNode->getType()), pNode->getData());
      break;
    case CEvaluationNode::LOGICAL:
      pResultNode = new CEvaluationNodeLogical((CEvaluationNodeLogical::SubType)CEvaluationNode::subType(pNode->getType()), pNode->getData());
      break;
    default:
      fatalError();
      break;
    }
  pChildNode = static_cast<const CEvaluationNode*>(pNode->getChild());
  while (pChildNode)
    {
      pResultNode->addChild(this->createExpressionTree(pChildNode, parameterMap, pDataModel));
      pChildNode = static_cast<const CEvaluationNode*>(pChildNode->getSibling());
    }
  assert(pResultNode);
  return pResultNode;
}

bool SBMLExporter::isSetExportExpressions() const
  {
    return this->mExportExpressions;
  }

void SBMLExporter::setExportExpressions(bool value)
{
  this->mExportExpressions = value;
}

const std::list<const CEvaluationTree*>* SBMLExporter::getUsedFunctionList() const
  {
    return this->mpUsedFunctions;
  }

Rule* SBMLExporter::createRuleFromCModelEntity(CModelEntity* pME, CCopasiDataModel* pDataModel, int sbmlLevel, int sbmlVersion, bool incompleteExport)
{
  Rule* pRule = NULL;
  Model* pModel = sbmlDocument->getModel();
  if (!pModel) fatalError();
  if (pME)
    {
      Rule* pOldRule = this->findExistingRuleForModelEntity(pME, pDataModel);
      std::map<CCopasiObject*, SBase*>& copasi2sbmlmap = pDataModel->getCopasi2SBMLMap();
      std::map<CCopasiObject*, SBase*>::iterator pos = copasi2sbmlmap.find(const_cast<CModelEntity*>(pME));
      if (pos == copasi2sbmlmap.end()) fatalError();
      SBase* pSBase = pos->second;
      Species* pSpecies = NULL;
      if (pME->getStatus() == CModelEntity::ODE)
        {
          // create a rate rule
          RateRule* pRateRule = NULL;
          if (pOldRule)
            {
              pRateRule = dynamic_cast<RateRule*>(pOldRule);
              if (pRateRule)
                {
                  RateRule* pTmpRule = new RateRule();
                  // copy the attributes from pAssignmentRule
                  // that we want to keep
                  if (pRateRule->isSetMetaId())
                    {
                      pTmpRule->setMetaId(pRateRule->getMetaId());
                    }
                  if (pRateRule->isSetNotes())
                    {
                      pTmpRule->setNotes(pRateRule->getNotes());
                    }
                  if (pRateRule->isSetAnnotation())
                    {
                      pTmpRule->setAnnotation(pRateRule->getAnnotation());
                    }
                  pTmpRule->setVariable(pRateRule->getVariable());
                  pRateRule = pTmpRule;
                }
              // the rule needs to be removed because the rules need to be
              // ordered in the list and reordering of list in SBML is not
              // possible
              this->removeFromList(pModel->getListOfRules(), pOldRule);
            }
          if (!pRateRule)
            {
              pRateRule = new RateRule();
              pRateRule->setVariable(pME->getSBMLId());
            }
          // check if the expression contains references to objects that are not allowed
          // only object nodes with references to Species, Compartments and global parameters and Time
          // are allowed at the moment
          if (incompleteExport)
            {
              if (!this->isRuleSBMLCompatible(pME, pDataModel, sbmlLevel, sbmlVersion).empty())
                {
                  // reset some attributes
                  switch (pSBase->getTypeCode())
                    {
                    case SBML_PARAMETER:
                      dynamic_cast<Parameter*>(pSBase)->setConstant(true);
                      break;
                    case SBML_COMPARTMENT:
                      dynamic_cast<Compartment*>(pSBase)->setConstant(true);
                      break;
                    case SBML_SPECIES:
                      // boundary condition should remain true
                      pSpecies = dynamic_cast<Species*>(pSBase);
                      pSpecies->setConstant(false);
                      break;
                    default:
                      fatalError();
                      break;
                    }
                  pdelete(pRateRule);
                  return NULL;
                }
            }
          this->checkExpressionObjects(pME->getExpressionPtr()->getRoot());
          std::list<const CEvaluationTree*>* usedFunctionList = new std::list<const CEvaluationTree*>;
          findUsedFunctions(pME->getExpressionPtr()->getRoot(), usedFunctionList, pDataModel, this->mpUsedFunctions, this->mpIdSet);
          pdelete(usedFunctionList);
          // now we set the new expression
          ASTNode* pRootNode = pME->getExpressionPtr()->getRoot()->toAST();
          pRateRule->setMath(pRootNode);
          pRule = pRateRule;
        }
      else if (pME->getStatus() == CModelEntity::ASSIGNMENT)
        {
          // create an assignment rule
          AssignmentRule* pAssignmentRule = NULL;
          if (pOldRule)
            {
              pAssignmentRule = dynamic_cast<AssignmentRule*>(pOldRule);
              if (pAssignmentRule)
                {
                  AssignmentRule* pTmpRule = new AssignmentRule();
                  // copy the attributes from pAssignmentRule
                  // that we want to keep
                  if (pAssignmentRule->isSetMetaId())
                    {
                      pTmpRule->setMetaId(pAssignmentRule->getMetaId());
                    }
                  if (pAssignmentRule->isSetNotes())
                    {
                      pTmpRule->setNotes(pAssignmentRule->getNotes());
                    }
                  if (pAssignmentRule->isSetAnnotation())
                    {
                      pTmpRule->setAnnotation(pAssignmentRule->getAnnotation());
                    }
                  pTmpRule->setVariable(pAssignmentRule->getVariable());
                  pAssignmentRule = pTmpRule;
                }
              // the rule needs to be removed because the rules need to be
              // ordered in the list and reordering of list in SBML is not
              // possible
              this->removeFromList(pModel->getListOfRules(), pOldRule);
            }
          if (!pAssignmentRule)
            {
              pAssignmentRule = new AssignmentRule();
              pAssignmentRule->setVariable(pME->getSBMLId());
            }
          // check if the expression contains references to objects that are not allowed
          // only object nodes with references to Species, Compartments and global parameters and Time
          // are allowed at the moment
          if (incompleteExport)
            {
              if (!this->isRuleSBMLCompatible(pME, pDataModel, sbmlLevel, sbmlVersion).empty())
                {
                  // reset some attributes
                  switch (pSBase->getTypeCode())
                    {
                    case SBML_PARAMETER:
                      dynamic_cast<Parameter*>(pSBase)->setConstant(true);
                      break;
                    case SBML_COMPARTMENT:
                      dynamic_cast<Compartment*>(pSBase)->setConstant(true);
                      break;
                    case SBML_SPECIES:
                      pSpecies = dynamic_cast<Species*>(pSBase);
                      pSpecies->setConstant(false);
                      // boundary condition should remain true
                      pSpecies->setBoundaryCondition(true);
                      break;
                    default:
                      fatalError();
                      break;
                    }
                  pdelete(pAssignmentRule);
                  return NULL;
                }
            }
          this->checkExpressionObjects(pME->getExpressionPtr()->getRoot());
          std::list<const CEvaluationTree*>* usedFunctionList = new std::list<const CEvaluationTree*>;
          findUsedFunctions(pME->getExpressionPtr()->getRoot(), usedFunctionList, pDataModel, this->mpUsedFunctions, this->mpIdSet);
          pdelete(usedFunctionList);
          // now we set the new expression
          ASTNode* pRootNode = pME->getExpressionPtr()->getRoot()->toAST();
          pAssignmentRule->setMath(pRootNode);
          pRule = pAssignmentRule;
        }
      else
        {
          // remove and delete the old rule
          if (pOldRule)
            {
              this->removeFromList(pModel->getListOfRules(), pOldRule);
            }
        }
      if (pRule)
        {
          // set the corresponding SBML entity to non constant
          switch (pSBase->getTypeCode())
            {
            case SBML_PARAMETER:
              dynamic_cast<Parameter*>(pSBase)->setConstant(false);
              break;
            case SBML_SPECIES:
              pSpecies = dynamic_cast<Species*>(pSBase);
              pSpecies->setConstant(false);
              // if it is a species and it has a rule, we need to set boundary condition to true
              // even if the species is not part of any reaction, this should not be incorrect
              pSpecies->setBoundaryCondition(true);
              break;
            case SBML_COMPARTMENT:
              dynamic_cast<Compartment*>(pSBase)->setConstant(false);
              break;
            default:
              fatalError();
              break;
            }
        }
    }
  return pRule;
}

void SBMLExporter::exportRules(std::vector<Rule*>& rules)
{
  // the rules first have to be ordered before they can be added to the model
  // before adding a Rule, all other rules have to be removed and possibly
  // deleted
  std::vector<Rule*> sortedAssignmentRules;
  std::map<Rule*, std::vector<Rule*> > dependencyMap;
  std::vector<Rule*>::iterator it = rules.begin();
  std::vector<Rule*>::iterator endIt = rules.end();
  while (it != endIt)
    {
      if ((*it)->getTypeCode() == SBML_ASSIGNMENT_RULE)
        {
          std::vector<Rule*> tmpRules2 = this->findDependenciesForRule(*it, rules);
          if (tmpRules2.empty())
            {
              sortedAssignmentRules.push_back(*it);
            }
          else
            {
              dependencyMap[*it] = tmpRules2;
            }
        }
      ++it;
    }
  while (!dependencyMap.empty())
    {
      bool removedItem = false;
      std::map<Rule*, std::vector<Rule*> >::iterator it2 = dependencyMap.begin();
      std::map<Rule*, std::vector<Rule*> >::iterator endIt2 = dependencyMap.end();
      while (it2 != endIt2)
        {
          it = it2->second.begin();
          endIt = it2->second.end();
          while (it != endIt)
            {
              if (std::find(sortedAssignmentRules.begin(), sortedAssignmentRules.end(), *it) == sortedAssignmentRules.end())
                {
                  break;
                }
              ++it;
            }
          if (it == endIt)
            {
              // all rules are already in sortedRules, so we can add this
              // rule as well and remove it from the map
              sortedAssignmentRules.push_back(it2->first);
              dependencyMap.erase(it2);
              removedItem = true;
              break;
            }
          ++it2;
        }
      if (!removedItem)
        {
          break;
        }
    }
  if (!dependencyMap.empty())
    {
      // there must be some dependency loops
      fatalError();
    }
  // since we create all rules from scratch and existing rules are deleted when
  Model* pModel = sbmlDocument->getModel();
  // the new rule is created, we now have to clean the remaining rules
  while (pModel->getListOfRules()->size() > 0)
    {
      SBase* pTmpObject = pModel->getListOfRules()->remove(0);
      delete pTmpObject;
    }
  // now we can add all rules in sorted Rules to the model
  it = sortedAssignmentRules.begin();
  endIt = sortedAssignmentRules.end();
  while (it != endIt)
    {
      pModel->addRule((*it));
      delete (*it);
      ++it;
    }
  // now we add all the RateRules in the order they appear in rules
  it = rules.begin();
  endIt = rules.end();
  while (it != endIt)
    {
      if ((*it)->getTypeCode() != SBML_ASSIGNMENT_RULE)
        {
          pModel->addRule((*it));
        }
      delete (*it);
      ++it;
    }
}

std::vector<Rule*> SBMLExporter::findDependenciesForRule(Rule* pRule, const std::vector<Rule*>& rules)
{
  const ASTNode* pNode = pRule->getMath();
  std::vector<Rule*> dependencies;
  std::set<std::string> nodeIds = this->getObjectNodeIds(pNode);
  std::set<std::string>::iterator it = nodeIds.begin();
  std::set<std::string>::iterator endIt = nodeIds.end();
  while (it != endIt)
    {
      std::vector<Rule*>::const_iterator it2 = rules.begin();
      std::vector<Rule*>::const_iterator endIt2 = rules.end();
      while (it2 != endIt2)
        {
          SBMLTypeCode_t type = (*it2)->getTypeCode();
          if (type == SBML_ASSIGNMENT_RULE)
            {
              if (dynamic_cast<const AssignmentRule*>(*it2)->getVariable() == *it)
                {
                  // the rules depends on itself, this is an error
                  if ((*it2) == pRule)
                    {
                      fatalError();
                    }
                  break;
                }
            }
          else if (type == SBML_RATE_RULE)
            {
              // do nothing
            }
          else
            {
              fatalError();
            }
          ++it2;
        }
      if (it2 != endIt2)
        {
          dependencies.push_back(*it2);
        }
      ++it;
    }
  return dependencies;
}

std::set<std::string> SBMLExporter::getObjectNodeIds(const ASTNode* pNode)
{
  std::set<std::string> ids;
  if (pNode)
    {
      if (pNode->getType() == AST_NAME)
        {
          ids.insert(pNode->getName());
        }
      else
        {
          unsigned int i, iMax = pNode->getNumChildren();
          for (i = 0;i < iMax;++i)
            {
              std::set<std::string> tmpSet = this->getObjectNodeIds(pNode->getChild(i));
              std::set<std::string>::iterator it = tmpSet.begin();
              std::set<std::string>::iterator endIt = tmpSet.end();
              while (it != endIt)
                {
                  ids.insert(*it);
                  ++it;
                }
            }
        }
    }
  return ids;
}

Rule* SBMLExporter::findExistingRuleForModelEntity(const CModelEntity* pME, CCopasiDataModel* pDataModel)
{
  Rule* pOldRule = NULL;
  Model* pModel = sbmlDocument->getModel();
  if (!pModel) fatalError();
  unsigned int i, iMax = pModel->getNumRules();
  SBase* pObject = NULL;
  const std::map<CCopasiObject*, SBase*>& copasi2sbmlmap = pDataModel->getCopasi2SBMLMap();
  std::map<CCopasiObject*, SBase*>::const_iterator pos = copasi2sbmlmap.find(const_cast<CModelEntity*>(pME));
  if (pos != copasi2sbmlmap.end())
    {
      pObject = pos->second;
    }
  else
    {
      fatalError();
    }
  for (i = 0;i < iMax;++i)
    {
      Rule* pR = pModel->getRule(i);
      std::string ruleVariable;
      if (pR->getTypeCode() == SBML_ASSIGNMENT_RULE)
        {
          ruleVariable = dynamic_cast<AssignmentRule*>(pR)->getVariable();
        }
      else if (pR->getTypeCode() == SBML_RATE_RULE)
        {
          ruleVariable = dynamic_cast<RateRule*>(pR)->getVariable();
        }
      bool found = false;
      switch (pObject->getTypeCode())
        {
        case SBML_COMPARTMENT:
          found = (dynamic_cast<Compartment*>(pObject)->getId() == ruleVariable);
          break;
        case SBML_SPECIES:
          found = (dynamic_cast<Species*>(pObject)->getId() == ruleVariable);
          break;
        case SBML_PARAMETER:
          found = (dynamic_cast<Parameter*>(pObject)->getId() == ruleVariable);
          break;
        default:
          fatalError();
          break;
        }
      if (found)
        {
          pOldRule = pR;
          break;
        }
    }
  return pOldRule;
}

bool SBMLExporter::checkExpressionObjects(const CEvaluationNode* pNode) const
  {
    bool result = true;
    if (!pNode) return false;
    if (CEvaluationNode::type(pNode->getType()) == CEvaluationNode::OBJECT)
      {
        std::vector<CCopasiContainer*> containerList;
        containerList.push_back(this->mpCopasiModel);
        CCopasiObjectName cn = dynamic_cast<const CEvaluationNodeObject*>(pNode)->getObjectCN();
        const CCopasiObject* pObject = CCopasiContainer::ObjectFromName(containerList, cn);
        assert(pObject);
        if (pObject->isReference()) pObject = pObject->getObjectParent();
        std::string objectType = pObject->getObjectType();
        if (objectType != "ModelValue" && objectType != "Metabolite" && objectType != "Compartment" && objectType != "Model")
          {
            result = false;
            CCopasiMessage(CCopasiMessage::EXCEPTION, MCSBML + 38);
          }
      }
    else
      {
        const CEvaluationNode* pChildNode = static_cast<const CEvaluationNode*>(pNode->getChild());
        while (result && pChildNode)
          {
            result = this->checkExpressionObjects(pChildNode);
            pChildNode = static_cast<const CEvaluationNode*>(pChildNode->getSibling());
          }
      }
    return result;
  }

std::vector<SBMLIncompatibility> SBMLExporter::isModelSBMLCompatible(CCopasiDataModel* pDataModel, int sbmlLevel, int sbmlVersion)
{
  std::vector<SBMLIncompatibility> result;
  //std::ostringstream ss;
  // general checks
  // check if there is a species with an ode rule that is in a nonfixed
  // compartment
  checkForODESpeciesInNonfixedCompartment(pDataModel, result);

  // check if the model contains references to normal distributions

  // check if the model contains references to model entities that can not be
  // represented in SBML like the inital value of something as opposed to the
  // transient value

  // level dependent checks
  switch (sbmlLevel)
    {
    case 2:
      switch (sbmlVersion)
        {
        case 1:
          result = SBMLExporter::isModelSBMLL2V1Compatible(pDataModel);
          break;
        default:
          //ss << "Export to SBML Level " << sbmlLevel << " Version " << sbmlVersion << " is currently not supported in COPASI.";
          //result.push_back(ss.str());
          break;
        }
      break;
    default:
      //ss << "Export to SBML Level " << sbmlLevel << " Version " << sbmlVersion << " is currently not supported in COPASI.";
      //result.push_back(ss.str());
      break;
    }
  return result;
}

std::vector<SBMLIncompatibility> SBMLExporter::isModelSBMLL2V1Compatible(CCopasiDataModel* pDataModel)
{
  std::vector<SBMLIncompatibility> result;
  // go through all assignment expressions and check if object nodes only contain
  // either a transient species concentration, a (transient) compartment volume,
  // a transient parameter value or the model time
  assert(pDataModel);
  const CModel* pModel = pDataModel->getModel();
  assert(pModel);
  const CCopasiVectorN<CModelValue>& modelValues = pModel->getModelValues();
  unsigned int i, iMax = modelValues.size();
  for (i = 0;i < iMax;++i)
    {
      const CModelValue* pModelValue = modelValues[i];
      assert(pModelValue);
      CModelEntity::Status status = pModelValue->getStatus();
      if (status == CModelEntity::ASSIGNMENT || status == CModelEntity::ODE)
        {
          std::vector<SBMLIncompatibility> tmpVect = isRuleSBMLL2V1Compatible(pModelValue, pDataModel);
          if (tmpVect.size() != 0)
            {
              std::vector<SBMLIncompatibility>::iterator it = tmpVect.begin();
              std::vector<SBMLIncompatibility>::iterator endit = tmpVect.end();
              while (it != endit)
                {
                  result.push_back(*it);
                  ++it;
                }
            }
        }
    }
  return result;
}

std::vector<SBMLIncompatibility> SBMLExporter::isRuleSBMLCompatible(const CModelEntity* pME, const CCopasiDataModel* pDataModel, int sbmlLevel, int sbmlVersion)
{
  std::vector<SBMLIncompatibility> result;
  std::ostringstream ss;
  switch (sbmlLevel)
    {
    case 2:
      switch (sbmlVersion)
        {
        case 1:
          result = SBMLExporter::isRuleSBMLL2V1Compatible(pME, pDataModel);
          break;
        default:
          fatalError();
          break;
        }
      break;
    default:
      fatalError();
      break;
    }
  return result;
}

std::vector<SBMLIncompatibility> SBMLExporter::isRuleSBMLL2V1Compatible(const CModelEntity* pME, const CCopasiDataModel* pDataModel)
{
  std::vector<SBMLIncompatibility> result;
  const CExpression* pExpression = pME->getExpressionPtr();
  assert(pExpression);
  const std::vector<CEvaluationNode*>& objectNodes = pExpression->getNodeList();
  unsigned j, jMax = objectNodes.size();
  for (j = 0;j < jMax;++j)
    {
      if (CEvaluationNode::type(objectNodes[j]->getType()) == CEvaluationNode::OBJECT)
        {
          const CEvaluationNodeObject* pObjectNode = dynamic_cast<const CEvaluationNodeObject*>(objectNodes[j]);
          assert(pObjectNode);
          std::vector<CCopasiContainer*> containers;
          containers.push_back(const_cast<CModel*>(pDataModel->getModel()));
          const CCopasiObject* pObject = CCopasiContainer::ObjectFromName(containers, pObjectNode->getObjectCN());
          assert(pObject);
          if (pObject->isReference())
            {
              const CCopasiObject* pObjectParent = pObject->getObjectParent();
              assert(pObjectParent);
              std::string typeString = pObjectParent->getObjectType();
              if (typeString == "Compartment")
                {
                  // must be a reference to the (transient) or initial volume
                  if (pObject->getObjectName() != "Volume" /*&& pObject->getObjectName() != "InitialVolume"*/)
                    {
                      result.push_back(SBMLIncompatibility(1, pObject->getObjectName().c_str(), "compartment", pObjectParent->getObjectName().c_str()));
                    }
                }
              else if (typeString == "Metabolite")
                {
                  // must be a reference to the transient or initial concentration
                  if (pObject->getObjectName() != "Concentration" /*&& pObject->getObjectName() != "InitialConcentration"*/)
                    {
                      result.push_back(SBMLIncompatibility(1, pObject->getObjectName().c_str(), "metabolite", pObjectParent->getObjectName().c_str()));
                    }
                }
              else if (typeString == "ModelValue")
                {
                  // must be a reference to the transient or initial value
                  if (pObject->getObjectName() != "Value" /* && pObject->getObjectName() != "InitialValue"*/)
                    {
                      result.push_back(SBMLIncompatibility(1, pObject->getObjectName().c_str(), "parameter", pObjectParent->getObjectName().c_str()));
                    }
                }
              else if (typeString == "Model")
                {
                  // must be a reference to the model time
                  if (pObject->getObjectName() != "Time")
                    {
                      result.push_back(SBMLIncompatibility(1, pObject->getObjectName().c_str(), "model", pObjectParent->getObjectName().c_str()));
                    }
                }
              else
                {
                  result.push_back(SBMLIncompatibility(1, pObject->getObjectName().c_str(), typeString.c_str(), pObjectParent->getObjectName().c_str()));
                }
            }
          else
            {
              result.push_back(SBMLIncompatibility(1, "value", pME->getObjectType().c_str(), pObject->getObjectName().c_str()));
            }
        }
    }
  return result;
}

#ifdef WITH_LAYOUT
void SBMLExporter::addLayoutsToSBMLDocument(const CListOfLayouts * copasiLayouts, CCopasiDataModel* pDataModel)
{
  if (this->sbmlDocument && sbmlDocument->getModel())
    copasiLayouts->exportToSBML(this->sbmlDocument->getModel()->getListOfLayouts(),
                                pDataModel->getCopasi2SBMLMap());
}
#endif //WITH_LAYOUT

void SBMLExporter::checkForODESpeciesInNonfixedCompartment(const CCopasiDataModel* pDataModel, std::vector<SBMLIncompatibility> result)
{
  const CModel* pModel = pDataModel->getModel();
  const CCopasiVector<CMetab>& metabolites = pModel->getMetabolites();
  CCopasiVector<CMetab>::const_iterator it = metabolites.begin(), endit = metabolites.end();
  while (it != endit)
    {
      if ((*it)->getStatus() == CModelValue::ODE)
        {
          const CCompartment* pCompartment = (*it)->getCompartment();
          assert(pCompartment != NULL);
          if (pCompartment->getStatus() != CModelValue::FIXED)
            {
              result.push_back(SBMLIncompatibility(3, (*it)->getObjectName().c_str(), pCompartment->getObjectName().c_str()));
              CCopasiMessage::CCopasiMessage(CCopasiMessage::ERROR, MCSBML + 52, (*it)->getObjectName().c_str());
            }
        }
      ++it;
    }
}

/**
 * This method checks wether the given model contains any assignment rules, ode
 * rules or initial assignments.
std::vector<SBMLIncompatibility> SBMLExporter::checkForRulesOrAssignments(const CCopasiDataModel* pDataModel)
{
  std::vector<SBMLIncompatibility> messages;
  if (pDataModel != NULL)
    {
      const CModel* pModel = pDataModel->getModel();
      if (pModel != NULL)
        {
          // check for rules
          const CCopasiVectorNS<CCompartment>& compartments = pModel->getCompartments();
          CCopasiVectorNS<CCompartment>::const_iterator compIt = compartments.begin(), compEndit = compartments.end();
          while (compIt != compEndit)
            {
              CModelValue::Status status = (*compIt)->getStatus();
              if (status == CModelValue::ASSIGNMENT || status == CModelValue::ODE)
                {
                  messages.push_back(SBMLIncompatibility(4, "Compartment", (*compIt)->getObjectName().c_str()));
                }
              ++compIt;
            }

          const CCopasiVector<CMetab>& metabs = pModel->getMetabolites();
          CCopasiVector<CMetab>::const_iterator metabIt = metabs.begin(), metabEndit = metabs.end();
          while (metabIt != metabEndit)
            {
              CModelValue::Status status = (*metabIt)->getStatus();
              if (status == CModelValue::ASSIGNMENT || status == CModelValue::ODE)
                {
                  messages.push_back(SBMLIncompatibility(4, "Metabolite", (*metabIt)->getObjectName().c_str()));
                }
              ++metabIt;
            }

          const CCopasiVectorN<CModelValue>& parameters = pModel->getModelValues();
          CCopasiVectorN<CModelValue>::const_iterator mvIt = parameters.begin(), mvEndit = parameters.end();
          while (mvIt != mvEndit)
            {
              CModelValue::Status status = (*mvIt)->getStatus();
              if (status == CModelValue::ASSIGNMENT || status == CModelValue::ODE)
                {
                  messages.push_back(SBMLIncompatibility(4, "Parameter", (*mvIt)->getObjectName().c_str()));
                }
              ++mvIt;
            }
          // check for initial assignments
          std::vector<SBMLIncompatibility> tmpMessages = checkForInitialAssignments(pDataModel);
          std::vector<SBMLIncompatibility>::iterator it = tmpMessages.begin(), endit = tmpMessages.end();
          while (it != endit)
            {
              messages.push_back(*it);
              ++it;
            }
        }
    }
  return messages;
}
 */

/**
 * This method checks wether the given model contains any initial assignments.
 */
std::vector<SBMLIncompatibility> SBMLExporter::checkForInitialAssignments(const CCopasiDataModel* pDataModel)
{
  std::vector<SBMLIncompatibility> messages;
  if (pDataModel != NULL)
    {
      const CModel* pModel = pDataModel->getModel();
      if (pModel != NULL)
        {
          // check for rules
          const CCopasiVectorNS<CCompartment>& compartments = pModel->getCompartments();
          CCopasiVectorNS<CCompartment>::const_iterator compIt = compartments.begin(), compEndit = compartments.end();
          while (compIt != compEndit)
            {
              if ((*compIt)->getInitialExpression() == "")
                {
                  messages.push_back(SBMLIncompatibility(5, "Compartment", (*compIt)->getObjectName().c_str()));
                }
              ++compIt;
            }

          const CCopasiVector<CMetab>& metabs = pModel->getMetabolites();
          CCopasiVector<CMetab>::const_iterator metabIt = metabs.begin(), metabEndit = metabs.end();
          while (metabIt != metabEndit)
            {
              if ((*metabIt)->getInitialExpression() == "")
                {
                  messages.push_back(SBMLIncompatibility(5, "Metabolite", (*metabIt)->getObjectName().c_str()));
                }
              ++metabIt;
            }

          const CCopasiVectorN<CModelValue>& parameters = pModel->getModelValues();
          CCopasiVectorN<CModelValue>::const_iterator mvIt = parameters.begin(), mvEndit = parameters.end();
          while (mvIt != mvEndit)
            {
              if ((*mvIt)->getInitialExpression() == "")
                {
                  messages.push_back(SBMLIncompatibility(5, "Parameter", (*mvIt)->getObjectName().c_str()));
                }
              ++mvIt;
            }
        }
    }
  return messages;
}

/**
 * This static methods checks, wether the model uses any function calls
 * that can not be expressed in SBML like the random distribution
 * functions.
 */
std::vector<SBMLIncompatibility> SBMLExporter::checkForUnsupportedFunctionCalls(CCopasiDataModel* pDataModel, const std::set<CEvaluationNodeFunction::SubType>& unsupportedFunctions, std::set<std::string>* pIdSet)
{
  std::vector<SBMLIncompatibility> messages;
  std::list<const CEvaluationTree*> usedFunctions;
  std::list<const CEvaluationTree*> knownUsedFunctions;
  if (pDataModel != NULL)
    {
      const CModel* pModel = pDataModel->getModel();
      if (pModel != NULL)
        {
          // check function calls in all rules and initial assignments
          const CCopasiVectorNS<CCompartment>& compartments = pModel->getCompartments();
          CCopasiVectorNS<CCompartment>::const_iterator compIt = compartments.begin(), compEndit = compartments.end();
          CModelValue::Status status;
          while (compIt != compEndit)
            {
              status = (*compIt)->getStatus();
              if (status == CModelValue::ODE || status == CModelValue::ASSIGNMENT)
                {
                  const CEvaluationTree* pTree = (*compIt)->getExpressionPtr();
                  if (pTree != NULL)
                    {
                      std::string objectType = (status == CModelValue::ODE) ? "ODE rule compartment" : "assignment for compartment";
                      checkForUnsupportedFunctionCalls(pTree, messages, unsupportedFunctions, objectType.c_str(), (*compIt)->getObjectName().c_str());
                      findUsedFunctions(pTree->getRoot(), &usedFunctions, pDataModel, &knownUsedFunctions, pIdSet);
                    }
                }
              if ((*compIt)->getInitialExpression() != "")
                {
                  const CEvaluationTree* pTree = (*compIt)->getInitialExpressionPtr();
                  if (pTree != NULL)
                    {
                      checkForUnsupportedFunctionCalls(pTree, messages, unsupportedFunctions, "initial assignment for compartment", (*compIt)->getObjectName().c_str());
                      findUsedFunctions(pTree->getRoot(), &usedFunctions, pDataModel, &knownUsedFunctions, pIdSet);
                    }
                }
              ++compIt;
            }

          const CCopasiVector<CMetab>& metabs = pModel->getMetabolites();
          CCopasiVector<CMetab>::const_iterator metabIt = metabs.begin(), metabEndit = metabs.end();
          while (metabIt != metabEndit)
            {
              status = (*metabIt)->getStatus();
              if (status == CModelValue::ODE || status == CModelValue::ASSIGNMENT)
                {
                  const CEvaluationTree* pTree = (*metabIt)->getExpressionPtr();
                  if (pTree != NULL)
                    {
                      std::string objectType = (status == CModelValue::ODE) ? "ODE rule metabolite" : "assignment for metabolite";
                      checkForUnsupportedFunctionCalls(pTree, messages, unsupportedFunctions, objectType.c_str(), (*metabIt)->getObjectName().c_str());
                      findUsedFunctions(pTree->getRoot(), &usedFunctions, pDataModel, &knownUsedFunctions, pIdSet);
                    }
                }
              if ((*metabIt)->getInitialExpression() != "")
                {
                  const CEvaluationTree* pTree = (*metabIt)->getInitialExpressionPtr();
                  if (pTree != NULL)
                    {
                      checkForUnsupportedFunctionCalls(pTree, messages, unsupportedFunctions, "initial assignment for metabolite", (*metabIt)->getObjectName().c_str());
                      findUsedFunctions(pTree->getRoot(), &usedFunctions, pDataModel, &knownUsedFunctions, pIdSet);
                    }
                }
              ++metabIt;
            }

          const CCopasiVectorN<CModelValue>& parameters = pModel->getModelValues();
          CCopasiVectorN<CModelValue>::const_iterator mvIt = parameters.begin(), mvEndit = parameters.end();
          while (mvIt != mvEndit)
            {
              status = (*mvIt)->getStatus();
              if (status == CModelValue::ODE || status == CModelValue::ASSIGNMENT)
                {
                  const CEvaluationTree* pTree = (*mvIt)->getExpressionPtr();
                  if (pTree != NULL)
                    {
                      std::string objectType = (status == CModelValue::ODE) ? "ODE rule parameter" : "assignment for parameter";
                      checkForUnsupportedFunctionCalls(pTree, messages, unsupportedFunctions, objectType.c_str(), (*mvIt)->getObjectName().c_str());
                      findUsedFunctions(pTree->getRoot(), &usedFunctions, pDataModel, &knownUsedFunctions, pIdSet);
                    }
                }
              if ((*mvIt)->getInitialExpression() != "")
                {
                  const CEvaluationTree* pTree = (*mvIt)->getInitialExpressionPtr();
                  if (pTree != NULL)
                    {
                      checkForUnsupportedFunctionCalls(pTree, messages, unsupportedFunctions, "initial assignment for parameter", (*mvIt)->getObjectName().c_str());
                      findUsedFunctions(pTree->getRoot(), &usedFunctions, pDataModel, &knownUsedFunctions, pIdSet);
                    }
                }
              ++mvIt;
            }

          // check function calls in all kinetic laws
          const CCopasiVectorNS<CReaction>& reactions = pModel->getReactions();
          CCopasiVectorNS<CReaction>::const_iterator reactIt = reactions.begin(), reactEndIt = reactions.end();
          while (reactIt != reactEndIt)
            {
              const CFunction* pFun = (*reactIt)->getFunction();
              if (pFun != NULL)
                {
                  checkForUnsupportedFunctionCalls(pFun, messages, unsupportedFunctions, "kinetic law for reaction", (*reactIt)->getObjectName().c_str());
                  findUsedFunctions(pFun->getRoot(), &usedFunctions, pDataModel, &knownUsedFunctions, pIdSet);
                }
              ++reactIt;
            }
          // check function calls in all functions used by the model
          std::list<const CEvaluationTree*>::const_iterator funIt = knownUsedFunctions.begin(), funEndit = knownUsedFunctions.end();
          while (funIt != funEndit)
            {
              checkForUnsupportedFunctionCalls(*funIt, messages, unsupportedFunctions, "function definition", (*funIt)->getObjectName().c_str());
              ++funIt;
            }
        }
    }
  return messages;
}

/**
 * This static methods checks, wether the given CEvaluationTree uses any function calls
 * that can not be expressed in SBML like the random distribution
 * functions.
 */
void SBMLExporter::checkForUnsupportedFunctionCalls(const CEvaluationTree* pTree,
    std::vector<SBMLIncompatibility>& messages,
    const std::set<CEvaluationNodeFunction::SubType>& unsupportedFunctions,
    const char* objectType, const char* objectName)
{
  if (pTree != NULL)
    {
      const CEvaluationNode* pNode = pTree->getRoot();
      if (pNode != NULL)
        {
          checkForUnsupportedFunctionCalls(pNode, messages, unsupportedFunctions, objectType, objectName);
        }
    }
}

/**
 * This static methods checks recursively, whether the given CEvaluationNode constains any function calls
 * that can not be expressed in SBML like the random distribution
 * functions.
 */
void SBMLExporter::checkForUnsupportedFunctionCalls(const CEvaluationNode* pNode,
    std::vector<SBMLIncompatibility>& messages, const std::set<CEvaluationNodeFunction::SubType>& unsupportedFunctions,
    const char* objectType, const char* objectName)
{
  if (CEvaluationNode::type(pNode->getType()) == CEvaluationNode::FUNCTION)
    {
      CEvaluationNodeFunction::SubType subtype = (CEvaluationNodeFunction::SubType)CEvaluationNode::subType(pNode->getType());
      if (unsupportedFunctions.find(subtype) != unsupportedFunctions.end())
        {
          messages.push_back(SBMLIncompatibility(2, pNode->getData().c_str(), objectType, objectName));
        }
    }
  const CEvaluationNode* pChild = dynamic_cast<const CEvaluationNode*>(pNode->getChild());
  while (pChild != NULL)
    {
      checkForUnsupportedFunctionCalls(pChild, messages, unsupportedFunctions, objectType, objectName);
      pChild = dynamic_cast<const CEvaluationNode*>(pChild->getSibling());
    }
}
