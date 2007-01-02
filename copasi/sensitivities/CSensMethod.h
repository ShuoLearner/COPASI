/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/sensitivities/CSensMethod.h,v $
   $Revision: 1.7 $
   $Name:  $
   $Author: ssahle $
   $Date: 2007/01/02 12:03:21 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#ifndef COPASI_SENSMethod
#define COPASI_SENSMethod

#include <string>

#include "utilities/CCopasiMethod.h"
#include "utilities/CAnnotatedMatrix.h"

class CSensProblem;
//class CProcessReport;

class CSensMethodLocalData
  {
  public:
    CCopasiArray tmp1;
    CCopasiArray tmp2;

    std::vector<CCopasiObject*> variables;

    /**
     * holds the index in the index array corresponding to the given level.
     * I.e. which dimension of the result array the variables correspond to.
     */
    C_INT32 index;
  };

class CSensMethod : public CCopasiMethod
  {
  protected:
    /**
     *  A pointer to the sensitivities problem.
     */
    CSensProblem * mpProblem;

    /**
     * A pointer to the progress bar handler
     */
    //CProcessReport * mpProgressHandler;

    // Operations
  private:
    /**
     * Default constructor.
     */
    CSensMethod();

  protected:
    /**
     * Specific constructor.
     * @param CCopasiMethod::SubType subType
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    explicit CSensMethod(CCopasiMethod::SubType subType,
                         const CCopasiContainer * pParent = NULL);

  public:
    /**
     * Create a trajectory method.
     * Note: the returned object has to be released after use with delete
     */
    static CSensMethod *
    createSensMethod(CCopasiMethod::SubType subType
                     = CCopasiMethod::sensMethod);

    /**
     * Copy constructor.
     * @param "const CTSSMethod &" src
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    CSensMethod(const CSensMethod & src,
                const CCopasiContainer * pParent = NULL);

    /**
     *  Destructor.
     */
    ~CSensMethod();

    /**
     */
    bool process(CProcessReport * handler);

    bool initialize(CSensProblem* problem);

    /**
     * Check if the method is suitable for this problem
     * @return bool suitability of the method
     */
    virtual bool isValidProblem(const CCopasiProblem * pProblem);

  protected:

    std::vector<CSensMethodLocalData> mLocalData;

    std::vector<CCopasiObject*> mTargetfunctionPointers;

    CCopasiTask * mpSubTask;

    void calculate_one_level(unsigned C_INT32 level, CCopasiArray & result);
    void do_target_calculation(CCopasiArray & result);

    C_FLOAT64 do_variation(CCopasiObject* variable);

    void calculate_difference(unsigned C_INT32 level, const C_FLOAT64 & delta,
                              CCopasiArray & result, CCopasiArray::index_type & resultindex);

    /**
     * calculate the scaled sensitivities. Calls scaling_variables().
     * It is assumed that the scaled result array is already of the correct size
     * (this is done in initialize()).
     */
    void do_scaling();

    /**
     * This contains the loop over the target functions for scaling the sensitivities
     */
    void scaling_targetfunction(const C_FLOAT64 & factor,
                                CCopasiArray::index_type & resultindex);

    /**
     * This contains the loop over the variables for scaling the sensitivities. Works recursively
     * and calls scaling_targetfunction() in the end.
     */
    void scaling_variables(C_INT32 level, const C_FLOAT64 & factor,
                           CCopasiArray::index_type & resultindex);
  };

#endif
