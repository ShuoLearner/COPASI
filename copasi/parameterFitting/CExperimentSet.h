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

#ifndef COPASI_CExperimentSet
#define COPASI_CExperimentSet

#include <string>

#include "utilities/CCopasiParameterGroup.h"
#include "utilities/CCopasiTask.h"
#include "utilities/CMatrix.h"

class CExperiment;

class CExperimentSet: public CCopasiParameterGroup
{
public:
  /**
   * Default constructor
   * @param const CCopasiContainer * pParent
   * @param const std::string & name (default: Experiment Set)
   */
  CExperimentSet(const CCopasiContainer * pParent,
                 const std::string & name = "Experiment Set");

  /**
   * Copy constructor
   * @param const CExperimentSet & src
   * @param const CCopasiContainer * pParent (default: src data model)
   */
  CExperimentSet(const CExperimentSet & src,
                 const CCopasiContainer * pParent = NULL);

  /**
   * Specific constructor used for reading COPASI files
   * @param const CCopasiParameterGroup & group
   * @param const CCopasiContainer * pParent (default: group data model)
   */
  CExperimentSet(const CCopasiParameterGroup & group,
                 const CCopasiContainer * pParent = NULL);

  /**
   * Destructor
   */
  virtual ~CExperimentSet();

  /**
   * This methods must be called to elevate subgroups to
   * derived objects. The default implementation does nothing.
   * @return bool success
   */
  virtual bool elevateChildren();

  /**
   * Compile the experiment set. This function must be called
   * before any evaluations can be performed.
   * @param const std::vector< CCopasiContainer * > listOfContainer
   * @return bool success
   */
  bool compile(const std::vector< CCopasiContainer * > listOfContainer =
                 CCopasiContainer::EmptyList);

  /**
   * Calculate statistics across all experiments.
   * @return bool success
   */
  bool calculateStatistics();

  /**
   * Retreive the number of the experiments in the set
   * @return size_t experimentCount
   */
  size_t getExperimentCount() const;

public:
  /**
   * Add an experiment
   * @return CExperiment * experiment
   */
  CExperiment * addExperiment(const CExperiment & experiment);

  /**
   * Remove an experiment from the set
   * @param const size_t & index
   */
  void removeExperiment(const size_t & index);

  /**
   * Retrieve the indexed experiment
   * @param const size_t & index,
   * @return CExperiment * experiment
   */
  CExperiment * getExperiment(const size_t & index);

  /**
   * Retrieve the indexed experiment
   * @param const size_t & index,
   * @return const CExperiment * experiment
   */
  const CExperiment * getExperiment(const size_t & index) const;

  /**
   * Retrieve the indexed experiment
   * @param const std::string & name,
   * @return CExperiment * experiment
   */
  CExperiment * getExperiment(const std::string & name);

  /**
   * Retrieve the indexed experiment
   * @param const std::string & name,
   * @return const CExperiment * experiment
   */
  const CExperiment * getExperiment(const std::string & name) const;

  /**
   * Check whether the set contains data for the the given task type
   * @param const CCopasiTask::Type & type
   * @return bool hasDataForTaskType
   */
  bool hasDataForTaskType(const CCopasiTask::Type & type) const;

  /**
   * Retrieve the indexed experiment type
   * @param const size_t & index,
   * @return const CCopasiTasK::Type & experimentType
   */
  const CCopasiTask::Type & getExperimentType(const size_t & index) const;

  /**
   * Retrieve the independent data of the indexed experiment.
   * @param const size_t & index,
   * @return const CMatrix< C_FLOAT64 > & independentData
   */
  const CMatrix< C_FLOAT64 > & getIndependentData(const size_t & index) const;

  /**
   * Retrieve the dependent data of the indexed experiment.
   * @param const size_t & index,
   * @return const CMatrix< C_FLOAT64 > & dependentData
   */
  const CMatrix< C_FLOAT64 > & getDependentData(const size_t & index) const;

  /**
   * Convert an experiment key to an index
   * @param const std::string & key
   * @return size_t index
   */
  size_t keyToIndex(const std::string & key) const;

  /**
   * Sort the experiments in the set
   */
  void sort();

  /**
   * Retrieve the list of file names
   * @return std::vector< std::string > fileNames
   */
  std::vector< std::string > getFileNames() const;

  /**
   * Retrieve the number of data points of all experiments combined.
   * @return size_t dataPointCount
   */
  size_t getDataPointCount() const;

  /**
   * Retrieve the count of valid data points of all experiments combined.
   * @return size_t count
   */
  const size_t & getValidValueCount() const;

  /**
   * Retrieve the list of dependent data objects
   * @return const std::vector< CCopasiObject * > & dependentObjects
   */
  const CVector< CCopasiObject * > & getDependentObjects() const;

  /**
   * Retrieve all dependent data objective values.
   * @return const CVector< C_FLOAT64 > & dependentObjectiveValues
   */
  const CVector< C_FLOAT64 > & getDependentObjectiveValues() const;

  /**
   * Retrieve all dependent data objective values.
   * @return const CVector< C_FLOAT64 > & dependentRMS
   */
  const CVector< C_FLOAT64 > & getDependentRMS() const;

  /**
   * Retrieve all dependent data error mean values.
   * @return const CVector< C_FLOAT64 > & dependentErrorMean
   */
  const CVector< C_FLOAT64 > & getDependentErrorMean() const;

  /**
   * Retrieve all dependent data error mean std. deviations.
   * @return const CVector< C_FLOAT64 > & dependentErrorMeanSD
   */
  const CVector< C_FLOAT64 > & getDependentErrorMeanSD() const;

private:
  /**
   * Allocates all group parameters and assures that they are
   * properly initialized.
   */
  void initializeParameter();

private:
  // Attributes
  /**
   * This is realized a CCopasiParameter type GROUP
   */
  std::vector< CExperiment * > * mpExperiments;

  /**
   * The number of parameters which are not of type CExperiment
   */
  size_t mNonExperiments;

  /**
   * A set of all dependent data objects;
   */
  CVector< CCopasiObject * > mDependentObjects;

  /**
   * A list of all dependent data objective values;
   */
  CVector< C_FLOAT64 > mDependentObjectiveValues;

  /**
   * A list of all dependent data objective values;
   */
  CVector< C_FLOAT64 > mDependentRMS;

  /**
   * A list of all dependent data error mean values;
   */
  CVector< C_FLOAT64 > mDependentErrorMean;

  /**
   * A list of all dependent data error mean std. deviations;
   */
  CVector< C_FLOAT64 > mDependentErrorMeanSD;

  /**
   * A list of the number of data points for each dependent object;
   */
  CVector< size_t > mDependentDataCount;

  /**
   * The total count of valid data values.
   */
  size_t mValidValueCount;
};

#ifdef COPASI_CROSSVALIDATION
class CCrossValidationSet : public CExperimentSet
{
public:
  /**
   * Default constructor
   * @param const std::string & name (default: Cross Validation Set)
   * @param const CCopasiContainer * pParent (default: NULL)
   */
  CCrossValidationSet(const CCopasiContainer * pParent,
                      const std::string & name = "Validation Set");

  /**
   * Copy constructor
   * @param const CCrossValidationSet & src
   * @param const CCopasiContainer * pParent (default: src data model)
   */
  CCrossValidationSet(const CCrossValidationSet & src,
                      const CCopasiContainer * pParent = NULL);

  /**
   * Specific constructor used for reading COPASI files
   * @param const CCopasiParameterGroup & group
   * @param const CCopasiContainer * pParent (default: group data model)
   */
  CCrossValidationSet(const CCopasiParameterGroup & group,
                      const CCopasiContainer * pParent = NULL);

  /**
   * Destructor
   */
  virtual ~CCrossValidationSet();

  /**
   * Set the weight
   * @param const C_FLOAT64 & weight
   */
  void setWeight(const C_FLOAT64 & weight);

  /**
   * Retreive the weight
   * @return const C_FLOAT64 & weight
   */
  const C_FLOAT64 & getWeight() const;

  /**
   * Set the threshold
   * @param const unsigned C_INT32 & threshold
   */
  void setThreshold(const unsigned C_INT32 & threshold);

  /**
   * Retreive the threshold
   * @return const unsigned C_INT32 & threshold
   */
  const unsigned C_INT32 & getThreshold() const;

private:
  /**
   * Allocates all group parameters and assures that they are
   * properly initialized.
   */
  void initializeParameter();

private:
  // Attributes
  /**
   * The weight of each datapoint relative to a data point of the
   * experiment set, i.e. 1 gives each point the same weight
   */
  C_FLOAT64 * mpWeight;

  /**
   * The threshold indicates the number of failed successive checks for which
   * the objective function has not improved needed to terminate the
   * parameter estimation.
   */
  unsigned C_INT32 * mpThreshold;
};
#endif // COPASI_CROSSVALIDATION

#endif // COPASI_CExperimentSet
