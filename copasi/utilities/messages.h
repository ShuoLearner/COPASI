/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/utilities/messages.h,v $
   $Revision: 1.30 $
   $Name:  $
   $Author: shoops $ 
   $Date: 2005/05/23 17:22:07 $
   End CVS Header */

const MESSAGES Messages[] =
  {
    // CReadConfig
    {MCReadConfig + 1, "CReadConfig (1): Variable '%s' not found in '%s(%d)'."},
    {MCReadConfig + 2, "CReadConfig (2): Cannot open file '%s'."},
    {MCReadConfig + 3, "CReadConfig (3): Cannot read file '%s'."},
    {MCReadConfig + 4, "CReadConfig (4): Cannot close file '%s'."},
    {MCReadConfig + 5, "CReadConfig (5): Invalid type '%s' for  Variable '%s'."},

    // CKinFunction
    {MCKinFunction + 1, "CKinFunction (1): Cannot find identifier '%s'"
     " in function '%s'."},
    {MCKinFunction + 2, "CKinFunction (2): Missing operant in function '%s'."},

    // CRange
    {MCRange + 1, "CRange (1): Invalid range ('%d', '%d')."},

    // CCopasiVector
    {MCCopasiVector + 1, "CCopasiVector (1): Object '%s' not found."},
    {MCCopasiVector + 2, "CCopasiVector (2): Object '%s' already exists."},

    // CFunctionParameters
    {MCFunctionParameters + 1, "CFunctionParameters (1): The usage '%s' "
     "is not unique for a vector type parameter ('%d')."},
    {MCFunctionParameters + 2, "CFunctionParameters (2): No parameter with "
     "usage '%s' with index >= '%ld'."},

    // CMassAction
    {MCMassAction + 1, "CMassAction (1): The function Mass Action reversibility "
     "must be either TRUE or FALSE."},

    // CCopasiMethod
    {MCCopasiMethod + 1, "CCopasiMethod (1): No parameter list "
     "found for name '%s' and type '%s'."},

    // CReaction
    {MCReaction + 1, "CReaction (1): Function '%s' not found."},
    {MCReaction + 2, "CReaction (2): In Reaction '%s' the compartment could not be guessed."},
    {MCReaction + 3, "CReaction (3): Reaction '%s' has no substrates and no products."},

    // CChemEq
    {MCChemEq + 1, "CChemEq (1): No Substrates and no Products."},
    {MCChemEq + 2, "CChemEq (2): Substrates in different Compartments."},
    {MCChemEq + 3, "CChemEq (3): No Substrates and Products are in different compartments."},

    // CMethodParameter
    {MCParameter + 1, "CMethodParameter (1): Invalid value '%f' for '%s' "
     "which is of type '%s'."},

    // CTrajectoryMethod
    {MCTrajectoryMethod + 1, "CTrajectoryMethod (1): Hybrid simulation not "
     "applicable, since the stoichiometry contains a non-integer."},
    {MCTrajectoryMethod + 2, "CTrajectoryMethod (2): Hybrid simulation not "
     "applicable, since reversible reactions exists."},
    {MCTrajectoryMethod + 3, "CTrajectoryMethod (3): Hybrid simulation not "
     "applicable, since more than one compartment is involved."},
    {MCTrajectoryMethod + 4, "CTrajectoryMethod (4): mLowerStochLimit ('%.2f') is greater than mUpperStochLimit ('%.2f')."},
    {MCTrajectoryMethod + 6, "CTrajectoryMethod (6): Deterministic integration failed. "
     "LSODA reported: %s \n\nPlease see result for indications of numerical instability."},
    {MCTrajectoryMethod + 7, "CTrajectoryMethod (7): pProblem == NULL"},
    {MCTrajectoryMethod + 8, "CTrajectoryMethod (8): Problem is not a trajectory problem."},
    {MCTrajectoryMethod + 9, "CTrajectoryMethod (9): Negative time steps not possible with stochastic simulation."},
    {MCTrajectoryMethod + 10, "CTrajectoryMethod (10): The tau-Leap Method encountered numerical problems.\nYou can try to reduce the tau-value."},
    {MCTrajectoryMethod + 11, "CTrajectoryMethod (11): Invalid tau-value ('%.2f'). Tau must have a positive value."},

    // XML Package
    {MCXML + 1, "XML (1): Required attribute '%s' not found (line: '%d')."},
    {MCXML + 2, "XML (2): XML error (line: '%d', column: '%d'): '%s'."},
    {MCXML + 3, "XML (3): Unknown element '%s' encountered at line '%d'."},
    {MCXML + 4, "XML (4): Unknown parameter '%s' encountered at line '%d'."},
    {MCXML + 5, "XML (5): Unknown task type '%s' encountered at line '%d'."},

    // CCopasiMessage
    {MCCopasiMessage + 1, "Message (1): No more messages."},

    // Configuration
    {MCConfiguration + 1, "Configuration (1): COPASI directory is not set. Some features might not be working correctly.\n"
     "                   Please set the environment variable COPASIDIR or use the\n"
     "                   commandline options -c COPASIDIR or --copasidir COPASIDIR\n"
     "                   to point to the COPASI installation directory."},

    // Optimization
    {MCOptimization + 1, "Optimization (1): Object '%s' not found."},
    {MCOptimization + 2, "Optimization (2): Lower Bound '%s' not found."},
    {MCOptimization + 3, "Optimization (3): Upper Bound '%s' not found."},
    {MCOptimization + 4, "Optimization (3): Invalid Relationship: '%s'."},

    // SBML
    // {MCSBML + 1, "SBML (1): Available"},
    {MCSBML + 2, "SBML (2): The SBML document contains no model."},
    {MCSBML + 3, "SBML (3): The SBML document contains rules that were ignored."},
    {MCSBML + 4, "SBML (4): The SBML document contains events that were ignored."},
    {MCSBML + 5, "SBML (5): Compartment %s referenced by species %s does not exist."},
    {MCSBML + 6, "SBML (6): Annotations in SBase element of SBML Level 1 files, this is invalid and has been ignored."},

    {MCTrajectoryProblem + 1, "CTrajectoryProblem (1): Invalid step size = '%f'."},
    {MCTrajectoryProblem + 2, "CTrajectoryProblem (2): The step number '%f' "
     "exceeds the limit. The step size has been adjusted to avoid over flow."},

    // SBML
    {MCDirEntry + 1, "DirEntry (1): Directory entry '%s' already exists."},
    {MCDirEntry + 2, "DirEntry (2): Directory entry '%s' is read-only."},

    // This must be the last element of the message list! Do not delete!
    {0, NULL}
  };
