/* Begin CVS Header
 $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CQValidator.cpp,v $
 $Revision: 1.15 $
 $Name:  $
 $Author: shoops $
 $Date: 2012/04/23 21:12:27 $
 End CVS Header */

// Copyright (C) 2012 - 2010 by Pedro Mendes, Virginia Tech Intellectual
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

#include <qlineedit.h>

#include <limits>

#include "CQValidator.h"

CQValidatorNotEmpty::CQValidatorNotEmpty(QLineEdit * parent, const char * name):
    CQValidator< QLineEdit >(parent, &QLineEdit::text, name)
{}

QValidator::State CQValidatorNotEmpty::validate(QString & input, int & pos) const
{
  if (input != "")
    return CQValidator< QLineEdit >::validate(input, pos);

  setColor(Invalid);
  return Intermediate;
}

CQValidatorBound::CQValidatorBound(QLineEdit * parent, const QString & sign):
    CQValidator< QLineEdit >(parent, &QLineEdit::text, NULL),
    mpDoubleValidator(new QDoubleValidator(-std::numeric_limits< C_FLOAT64 >::max(), std::numeric_limits< C_FLOAT64 >::max(), std::numeric_limits< C_FLOAT64 >::digits10, this)),
    mSign(sign),
    mValidBound("\t") // It is not possible to insert a tab.
{}

QValidator::State CQValidatorBound::validate(QString & input, int & pos) const
{
  QString Input;

  if (input == mValidBound ||
      mpDoubleValidator->validate(input, pos) == Acceptable ||
      (input.startsWith(mSign) &&
       input.endsWith("%") &&
       mpDoubleValidator->validate(Input = input.mid(1, input.length() - 2), pos)))
    {
      force(input);
      return Acceptable;
    }

  setColor(Invalid);
  return Intermediate;
}

void CQValidatorBound::force(const QString & input) const
{
  const_cast<CQValidatorBound *>(this)->mValidBound = input;

  CQValidator< QLineEdit >::force(input);
}

CQValidatorDouble::CQValidatorDouble(QLineEdit * parent, const char * name):
    CQValidator< QLineEdit >(parent, &QLineEdit::text, name),
    mpDoubleValidator(new QDoubleValidator(-std::numeric_limits< C_FLOAT64 >::max(), std::numeric_limits< C_FLOAT64 >::max(), std::numeric_limits< C_FLOAT64 >::digits10, this))
{}

QValidator::State CQValidatorDouble::validate(QString & input, int & pos) const
{
  if (mpDoubleValidator->validate(input, pos) == Acceptable)
    return CQValidator< QLineEdit >::validate(input, pos);

  setColor(Invalid);
  return Intermediate;
}

void CQValidatorDouble::setRange(const C_FLOAT64 & lowerBound, const C_FLOAT64 & upperBound)
{mpDoubleValidator->setRange(lowerBound, upperBound, std::numeric_limits< C_FLOAT64 >::digits10);}

CQValidatorInt::CQValidatorInt(QLineEdit * parent, const char * name):
    CQValidator< QLineEdit >(parent, &QLineEdit::text, name),
    mpIntValidator(new QIntValidator(std::numeric_limits< int >::min(),
                                     std::numeric_limits< int >::max(), this))
{}

QValidator::State CQValidatorInt::validate(QString & input, int & pos) const
{
  if (mpIntValidator->validate(input, pos) == Acceptable)
    return CQValidator< QLineEdit >::validate(input, pos);

  setColor(Invalid);
  return Intermediate;
}

void CQValidatorInt::setRange(const int & lowerBound, const int & upperBound)
{mpIntValidator->setRange(lowerBound, upperBound);}
