/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/report/CCopasiObjectName.h,v $
   $Revision: 1.8 $
   $Name:  $
   $Author: shoops $
   $Date: 2011/03/07 19:32:38 $
   End CVS Header */

// Copyright (C) 2011 - 2010 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/**
 * Class CCopasiObjectName
 *
 * This class is the class for handling Copasi object names.
 *
 * Copyright Stefan Hoops 2002
 */

#ifndef COPASI_CCopasiObjectName
#define COPASI_CCopasiObjectName

#include <string>
#include <set>

/** @dia:pos 21.4754,29.8662 */
class CCopasiObjectName: public std::string
{
  // Operations
public:
  CCopasiObjectName();

  CCopasiObjectName(const std::string & name);

  CCopasiObjectName(const CCopasiObjectName & src);

  ~CCopasiObjectName();

  CCopasiObjectName getPrimary() const;

  CCopasiObjectName getRemainder() const;

  std::string getObjectType() const;

  std::string getObjectName() const;

  size_t getElementIndex(const size_t & pos = 0) const;

  std::string getElementName(const size_t & pos /*= 0*/,
                             const bool & unescape = true) const;

  static std::string escape(const std::string & name);

  static std::string unescape(const std::string & name);

  // private:
  std::string::size_type findEx(const std::string & toFind,
                                const std::string::size_type & pos = 0) const;
};

class CRegisteredObjectName: public CCopasiObjectName
{
public:
  CRegisteredObjectName();

  CRegisteredObjectName(const std::string & name);

  CRegisteredObjectName(const CRegisteredObjectName & src);

  ~CRegisteredObjectName();

  static const std::set<CRegisteredObjectName*> & getSet()
  {return mSet;}

private:
  static std::set<CRegisteredObjectName*> mSet;
};

#endif // COPASI_CCopasiObjectName
