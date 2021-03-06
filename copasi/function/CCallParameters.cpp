/* Begin CVS Header
 $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/function/CCallParameters.cpp,v $
 $Revision: 1.23 $
 $Name:  $
 $Author: shoops $
 $Date: 2011/07/05 19:24:03 $
 End CVS Header */

// Copyright (C) 2011 - 2010 by Pedro Mendes, Virginia Tech Intellectual
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

#include "copasi.h"
#include "CCallParameters.h"
#include "CFunction.h"

#include "utilities/CCopasiParameter.h"

// static
CCopasiObject * CFunctionParameterMap::pUnmappedObject = NULL;

//TODO: modify the constructors so that CFunctionParameterMap behaves like a CCopasiObject

CFunctionParameterMap::CFunctionParameterMap():
    mPointers(),
    mObjects(),
    mpFunctionParameters(NULL)
{
  if (pUnmappedObject == NULL)
    {
      C_FLOAT64 InvalidValue = std::numeric_limits< C_FLOAT64 >::quiet_NaN();

      pUnmappedObject =
        new CCopasiParameter("NaN", CCopasiParameter::DOUBLE, & InvalidValue);
    }
};

CFunctionParameterMap::CFunctionParameterMap(const CFunctionParameterMap & src):
    mPointers(src.mPointers),
    mObjects(src.mObjects),
    mpFunctionParameters(new CFunctionParameters(*src.mpFunctionParameters))
{
  size_t i, imax = mpFunctionParameters->size();

  for (i = 0; i < imax; ++i)
    if ((*mpFunctionParameters)[i]->getType() >= CFunctionParameter::VINT32)
      {
        mPointers[i].vector = new CCallParameters<C_FLOAT64>(*src.mPointers[i].vector);
        mObjects[i].vector = new CCallParameters<CCopasiObject>(*src.mObjects[i].vector);
      }
}

CFunctionParameterMap::~CFunctionParameterMap()
{
  clearCallParameters();
  pdelete(mpFunctionParameters)
};

void CFunctionParameterMap::initializeFromFunctionParameters(const CFunctionParameters & src)
{
  clearCallParameters();

  pdelete(mpFunctionParameters)
  mpFunctionParameters = new CFunctionParameters(src);

  initCallParameters();
}

void CFunctionParameterMap::clearCallParameters()
{
  if (mpFunctionParameters)
    {
      size_t i, imax = mpFunctionParameters->size();

      for (i = 0; i < imax; i++)
        {
          if ((*mpFunctionParameters)[i]->getType() >= CFunctionParameter::VINT32)
            {
              if (mObjects[i].vector)
                delete mObjects[i].vector;

              if (mPointers[i].vector)
                delete mPointers[i].vector;
            }
        }
    }

  mPointers.clear();
  mObjects.clear();
}

void CFunctionParameterMap::initCallParameters()
{
  size_t i, imax = mpFunctionParameters->size();

  // Resizing call the CCallParameters clear() method and initializes
  // all values to NULL.
  mPointers.resize(imax);
  mObjects.resize(imax);

  for (i = 0; i < imax; i++)
    {
      if ((*mpFunctionParameters)[i]->getType() >= CFunctionParameter::VINT32)
        {
          mObjects[i].vector = new CCallParameters<CCopasiObject>;
          mPointers[i].vector = new CCallParameters<C_FLOAT64>;
        }
    }
}

void CFunctionParameterMap::checkCallParameters() const
{
  size_t i, imax = mpFunctionParameters->size();
  size_t j, jmax;

  for (i = 0; i < imax; i++)
    {
      if (mPointers[i].vector == NULL)
        fatalError();

      if (mObjects[i].vector == NULL)
        fatalError();

      if ((*mpFunctionParameters)[i]->getType() < CFunctionParameter::VINT32)
        continue;

      jmax = mPointers[i].vector->size();

      for (j = 0; j < jmax; j++)
        if ((*mPointers[i].vector)[j].value == NULL)
          fatalError();

      if (jmax != mObjects[i].vector->size()) fatalError();

      for (j = 0; j < jmax; j++)
        if ((*mObjects[i].vector)[j].value == NULL)
          fatalError();
    }
}

void CFunctionParameterMap::setCallParameter(const std::string paramName, const CCopasiObject* obj)
{
  CFunctionParameter::DataType type;
  size_t index = findParameterByName(paramName, type);

  if (type >= CFunctionParameter::VINT32) fatalError(); // is a vector

  // TODO: check type of object
  mObjects[index].value = obj;

  assert(obj->getValuePointer());
  assert(obj->isValueDbl());
  mPointers[index].value = (const C_FLOAT64*) obj->getValuePointer();
}

void CFunctionParameterMap::addCallParameter(const std::string paramName, const CCopasiObject* obj)
{
  CFunctionParameter::DataType type;
  size_t index = findParameterByName(paramName, type);

  if (type < CFunctionParameter::VINT32) fatalError(); // is not a vector

  assert(obj->getValuePointer());
  assert(obj->isValueDbl());

  mObjects[index].vector->push_back(obj);
  mPointers[index].vector->push_back((const C_FLOAT64*) obj->getValuePointer());
}

void CFunctionParameterMap::clearCallParameter(const std::string paramName)
{
  CFunctionParameter::DataType type;
  size_t index = findParameterByName(paramName, type);

  if (type < CFunctionParameter::VINT32) fatalError(); // is not a vector

  // TODO: check type of object
  mObjects[index].vector->clear();

  mPointers[index].vector->clear();
}

size_t CFunctionParameterMap::findParameterByName(const std::string & name,
    CFunctionParameter::DataType & dataType) const
{
  return mpFunctionParameters->findParameterByName(name, dataType);
}

CCallParameters<C_FLOAT64> & CFunctionParameterMap::getPointers()
{
  return mPointers;
}

const CCallParameters<C_FLOAT64> & CFunctionParameterMap::getPointers() const
{
  return mPointers;
}

std::vector< const CCopasiObject * > CFunctionParameterMap::getObjects(const size_t & index) const
{
  std::vector< const CCopasiObject * > Objects;

  if (index != C_INVALID_INDEX)
    {
      if ((*mpFunctionParameters)[index]->getType() < CFunctionParameter::VINT32)
        Objects.push_back(mObjects[index].value);
      else
        {
          size_t i, imax = mObjects[index].vector->size();

          for (i = 0; i < imax; i++)
            Objects.push_back((*mObjects[index].vector)[i].value);
        }
    }

  return Objects;
}

// CCallParameters<CCopasiObject> & CFunctionParameterMap::getObjects() {return mObjects;};

const CCallParameters<CCopasiObject> & CFunctionParameterMap::getObjects() const
{return mObjects;};

const CFunctionParameters & CFunctionParameterMap::getFunctionParameters() const
{return * mpFunctionParameters;};
