// Begin git Header 
//   Commit: 28d5663ff3fc99993d3b249dec626841cb5247ab 
//   Author: Frank T. Bergmann fbergman@caltech.edu 
//   Date: 2012-08-29 10:43:00 +0200 
// End git Header 


// Copyright (C) 2010 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc., University of Heidelberg, and The University 
// of Manchester. 
// All rights reserved. 

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc., EML Research, gGmbH, University of Heidelberg, 
// and The University of Manchester. 
// All rights reserved. 

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc. and EML Research, gGmbH. 
// All rights reserved. 

// Copyright © 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

%{

#include "scan/CScanTask.h"

%}

// process is ignored because I have written extension code in CCopasiTask
// that calls the task specific process, so this new process should be
// used for all tasks
%ignore CScanTask::process(const bool& useInitialValues);
%ignore CScanTask::initialize;
%ignore CScanTask::load;

#ifdef SWIGR
// we ignore the method that takes an int and create a new method that takes
// the enum from CCopasiTask
%ignore CScanTask::setMethodType(const int& type);
#endif // SWIGR


%include "scan/CScanTask.h"

   
#ifdef SWIGR
%extend CScanTask{
   bool setMethodType(const CCopasiMethod::SubType& type)
   {
      return $self->setMethodType(type);
   }
}
#endif // SWIGR


