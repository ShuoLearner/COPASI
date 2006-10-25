%{

#include "utilities/CVersion.h"

%}

// add ignore statements for the setVersion function
// disable the Constructor since there is no need to create an object of type
// CVersion
%ignore CVersion::setVersion(const C_INT32 & major,
                   const C_INT32 & minor,
                   const C_INT32 & devel,
                   const std::string & comment);
%ignore CVersion::CVersion();
%ignore CVersion::~CVersion();

%include "utilities/CVersion.h"
