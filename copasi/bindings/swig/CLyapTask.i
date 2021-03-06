// Copyright (C) 2010 - 2013 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc., University of Heidelberg, and The University 
// of Manchester. 
// All rights reserved. 

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc., EML Research, gGmbH, University of Heidelberg, 
// and The University of Manchester. 
// All rights reserved. 

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc. and EML Research, gGmbH. 
// All rights reserved. 


%{

#include "lyap/CLyapTask.h"

%}

// process is ignored because I have written extension code in CCopasiTask
// that calls the task specific process, so this new process should be
// used for all tasks
%ignore CLyapTask::process(const bool& useInitialValues);
%ignore CLyapTask::initialize;
%ignore CLyapTask::printResult;

#ifdef SWIGR
// we ignore the method that takes an int and create a new method that takes
// the enum from CCopasiTask
%ignore CLyapTask::setMethodType(const int& type);
#endif // SWIGR



%include "lyap/CLyapTask.h"

   
#ifdef SWIGR
%extend CLyapTask{
    bool setMethodType(const CCopasiMethod::SubType& type)
   {
      return $self->setMethodType(type);
   }
}
#endif // SWIGR


