// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/compareExpressions/CNormalLogical.cpp,v $
//   $Revision: 1.2 $
//   $Name:  $
//   $Author: gauges $
//   $Date: 2007/08/09 05:13:46 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "CNormalLogical.h"
#include "CNormalChoiceLogical.h"
#include "CNormalLogicalItem.h"
#include <sstream>
#include <assert.h>

CNormalLogical::CNormalLogical(): CNormalBase(), mNot(false)
{}

CNormalLogical::CNormalLogical(const CNormalLogical& src): CNormalBase(src), mNot(src.mNot)
{
  ChoiceSetOfSets::const_iterator it = this->mChoices.begin(), endit = this->mChoices.end();
  while (it != endit)
    {
      ChoiceSet::const_iterator it4 = it->first.begin(), endit4 = it->first.end();
      while (it4 != endit4)
        {
          delete it4->first;
          ++it4;
        }
      ++it;
    }
  this->mChoices.clear();
  it = src.mChoices.begin();
  endit = src.mChoices.end();
  while (it != endit)
    {
      ChoiceSet tmp;
      ChoiceSet::const_iterator it4 = (*it).first.begin(), endit4 = (*it).first.end();
      while (it4 != endit4)
        {
          tmp.insert(std::make_pair(new CNormalChoiceLogical(*it4->first), it4->second));
          ++it4;
        }
      this->mChoices.insert(std::make_pair(tmp, it->second));
      ++it;
    }
  ItemSetOfSets::const_iterator it2 = this->mAndSets.begin(), endit2 = this->mAndSets.end();
  while (it2 != endit2)
    {
      ItemSet::const_iterator it3 = it2->first.begin(), endit3 = it2->first.end();
      while (it3 != endit3)
        {
          delete it3->first;
          ++it3;
        }
      ++it2;
    }
  this->mAndSets.clear();
  it2 = src.mAndSets.begin();
  endit2 = src.mAndSets.end();
  while (it2 != endit2)
    {
      ItemSet tmp;
      ItemSet::const_iterator it3 = it2->first.begin(), endit3 = it2->first.end();
      while (it3 != endit3)
        {
          tmp.insert(std::make_pair(new CNormalLogicalItem(*it3->first), it3->second));
          ++it3;
        }
      this->mAndSets.insert(std::make_pair(tmp, it2->second));
      ++it2;
    }
}

CNormalLogical& CNormalLogical::operator=(const CNormalLogical& src)
{
  this->mNot = src.mNot;
  ChoiceSetOfSets::const_iterator it = this->mChoices.begin(), endit = this->mChoices.end();
  while (it != endit)
    {
      ChoiceSet::const_iterator it4 = it->first.begin(), endit4 = it->first.end();
      while (it4 != endit4)
        {
          delete it4->first;
          ++it4;
        }
      ++it;
    }
  this->mChoices.clear();
  it = src.mChoices.begin();
  endit = src.mChoices.end();
  while (it != endit)
    {
      ChoiceSet tmp;
      ChoiceSet::const_iterator it4 = it->first.begin(), endit4 = it->first.end();
      while (it4 != endit4)
        {
          tmp.insert(std::make_pair(new CNormalChoiceLogical(*it4->first), it4->second));
          ++it4;
        }
      this->mChoices.insert(std::make_pair(tmp, it->second));
      ++it;
    }
  ItemSetOfSets::const_iterator it2 = this->mAndSets.begin(), endit2 = this->mAndSets.end();
  while (it2 != endit2)
    {
      ItemSet::const_iterator it3 = it2->first.begin(), endit3 = it2->first.end();
      while (it3 != endit3)
        {
          delete it3->first;
          ++it3;
        }
      ++it2;
    }
  this->mAndSets.clear();
  it2 = src.mAndSets.begin();
  endit2 = src.mAndSets.end();
  while (it2 != endit2)
    {
      ItemSet tmp;
      ItemSet::const_iterator it3 = it2->first.begin(), endit3 = it2->first.end();
      while (it3 != endit3)
        {
          tmp.insert(std::make_pair(new CNormalLogicalItem(*it3->first), it3->second));
          ++it3;
        }
      this->mAndSets.insert(std::make_pair(tmp, it2->second));
      ++it2;
    }
  return *this;
}

CNormalLogical::~CNormalLogical()
{
  ChoiceSetOfSets::const_iterator it = this->mChoices.begin(), endit = this->mChoices.end();
  while (it != endit)
    {
      ChoiceSet::const_iterator it4 = it->first.begin(), endit4 = it->first.end();
      while (it4 != endit4)
        {
          delete it4->first;
          ++it4;
        }
      ++it;
    }
  this->mChoices.clear();
  ItemSetOfSets::const_iterator it2 = this->mAndSets.begin(), endit2 = this->mAndSets.end();
  while (it2 != endit2)
    {
      ItemSet::const_iterator it3 = it2->first.begin(), endit3 = it2->first.end();
      while (it3 != endit3)
        {
          delete it3->first;
          ++it3;
        }
      ++it2;
    }
  this->mAndSets.clear();
}

CNormalLogical* CNormalLogical::copy() const
  {
    return new CNormalLogical(*this);
  }

std::string CNormalLogical::toString() const
  {
    std::ostringstream str;
    ChoiceSetOfSets::const_iterator it = this->mChoices.begin(), endit = this->mChoices.end();
    if (this->mNot == true)
      {
        str << "NOT";
      }
    str << "(";
    while (it != endit)
      {
        if (it->second == true)
          {
            str << "NOT";
          }
        str << "(";
        ChoiceSet::const_iterator inner = it->first.begin(), innerend = it->first.end();
        while (inner != innerend)
          {
            if (inner->second == true)
              {
                str << "NOT(" << inner->first->toString() << ")";
              }
            else
              {
                str << inner->first->toString();
              }
            ++inner;
            if (inner != innerend)
              {
                str << " & ";
              }
          }
        str << ")";
        ++it;
        if (it != endit)
          {
            str << " | ";
          }
      }
    ItemSetOfSets::const_iterator it2 = this->mAndSets.begin(), endit2 = this->mAndSets.end();
    while (it2 != endit2)
      {
        if (it->second == true)
          {
            str << "NOT";
          }
        str << "(";
        ItemSet::const_iterator inner = it2->first.begin(), innerend = it2->first.end();
        while (inner != innerend)
          {
            if (inner->second == true)
              {
                str << "NOT(" << inner->first->toString() << ")";
              }
            else
              {
                str << inner->first->toString();
              }
            ++inner;
            if (inner != innerend)
              {
                str << " & ";
              }
          }
        str << ")";
        ++it2;
        if (it2 != endit2)
          {
            str << " | ";
          }
      }
    str << ")";
    return str.str();
  }

bool CNormalLogical::isNegated() const
  {
    return this->mNot;
  }

void CNormalLogical::setIsNegated(bool negate)
{
  this->mNot = negate;
}

void CNormalLogical::negate()
{
  this->mNot = !this->mNot;
}

std::ostream& operator<<(std::ostream& os, const CNormalLogical& logical)
{
  os << logical.toString();
  return os;
}

CNormalLogical::ChoiceSetOfSets& CNormalLogical::getChoices()
{
  return this->mChoices;
}

const CNormalLogical::ChoiceSetOfSets& CNormalLogical::getChoices() const
  {
    return this->mChoices;
  }

CNormalLogical::ItemSetOfSets& CNormalLogical::getAndSets()
{
  return this->mAndSets;
}

const CNormalLogical::ItemSetOfSets& CNormalLogical::getAndSets() const
  {
    return this->mAndSets;
  }

bool CNormalLogical::simplify()
{
  bool result = true;
  // first get rid of all choices
  // if A then B else C is replaced by (A AND B) OR (NOT(A) AND C)
  // to save some memory and computing power, we can already make one more
  // transformation here and convert (A AND B) OR (NOT(A) AND C) to (B OR
  // NOT(A)) AND (A OR C) AND (B OR C)
  // we we have a set of AND combined set of OR combined elements which we
  // have to convert to a set of OR combined sets of AND combined elements.
  std::set<std::pair<std::set<std::pair<CNormalLogical*, bool>, CNormalLogical::SetSorter<CNormalLogical> >, bool>, CNormalLogical::SetOfSetsSorter<CNormalLogical> > tmpAndItems;
  std::set<std::pair<std::set<std::pair<CNormalLogical*, bool>, CNormalLogical::SetSorter<CNormalLogical> >, bool>, CNormalLogical::SetOfSetsSorter<CNormalLogical> > tmpOrItems;
  ChoiceSetOfSets::const_iterator it = this->mChoices.begin(), endit = this->mChoices.end();
  while (it != endit && result == true)
    {
      ChoiceSet tmpSet;
      if ((*it).second == true)
        {
          result = negateSets((*it).first, tmpSet);
        }
      else
        {
          copySet((*it).first, tmpSet);
        }
      ChoiceSet::iterator it2 = tmpSet.begin(), endit2 = tmpSet.end();
      while (it2 != endit2 && result == true)
        {
          if ((*it2).second == true)
            {
              (*it2).first->negate();
            }
          (*it2).first->simplify();
          std::set<std::pair<CNormalLogical*, bool>, CNormalLogical::SetSorter<CNormalLogical> > tmpSet2;
          tmpSet2.insert(std::make_pair(new CNormalLogical(*(*it2).first->getTrueExpression()), false));
          CNormalLogical* pLogical = new CNormalLogical(*(*it2).first->getCondition());
          pLogical->negate();
          tmpSet2.insert(std::make_pair(pLogical, true));
          tmpAndItems.insert(std::make_pair(tmpSet2, false));
          tmpSet2.clear();
          tmpSet2.insert(std::make_pair(new CNormalLogical(*(*it2).first->getFalseExpression()), false));
          tmpSet2.insert(std::make_pair(new CNormalLogical(*(*it2).first->getCondition()), false));
          tmpAndItems.insert(std::make_pair(tmpSet2, false));
          tmpSet2.clear();
          tmpSet2.insert(std::make_pair(new CNormalLogical(*(*it2).first->getTrueExpression()), false));
          tmpSet2.insert(std::make_pair(new CNormalLogical(*(*it2).first->getFalseExpression()), false));
          tmpAndItems.insert(std::make_pair(tmpSet2, false));
          ++it2;
        }
      if (result == false) cleanSet(tmpSet);
      ++it;
    }
  if (result == false)
    {
      cleanSetOfSets(tmpAndItems);
    }
  else
    {
      result = convertAndOrToOrAnd(tmpAndItems, tmpOrItems);
      cleanSetOfSets(tmpAndItems);
      if (result == false)
        {
          cleanSetOfSets(tmpOrItems);
        }
      else
        {
          // now we have a set of OR combined sets of AND combined
          // CNormalLogical items which themselves are OR combined sets of
          // AND combined CNormalLogicalItem elements
          // so we have to get rid of two more levels
          ItemSetOfSets tmpAndItems2;
          ItemSetOfSets tmpOrItems2;
          std::set<std::pair<std::set<std::pair<CNormalLogical*, bool>, CNormalLogical::SetSorter<CNormalLogical> >, bool>, CNormalLogical::SetOfSetsSorter<CNormalLogical> >::iterator it2 = tmpOrItems.begin(), endit2 = tmpOrItems.end();
          while (it2 != endit2)
            {
              std::set<std::pair<CNormalLogical*, bool> >::const_iterator it3 = (*it2).first.begin(), endit3 = (*it2).first.end();
              assert(it2->second != true);
              while (it3 != endit3 && result == true)
                {
                  assert(it3->second != true);
                  assert(it3->first->getChoices().size() == 0);
                  ItemSetOfSets tmpSet;
                  copySetOfSets((*it3).first->getAndSets(), tmpSet);
                  if ((*it3).first->isNegated())
                    {
                      result = negateSetOfSets((*it3).first->getAndSets(), tmpSet);
                    }
                  else
                    {
                      copySetOfSets((*it3).first->getAndSets(), tmpSet);
                    }
                  if (result == true)
                    {
                      // new entries are appended with each call to
                      // convertAndOrToOrAnd
                      result = convertAndOrToOrAnd(tmpSet, tmpAndItems2);
                      cleanSetOfSets(tmpSet);
                    }
                  else
                    {
                      cleanSetOfSets(tmpSet);
                    }
                  ++it3;
                }
              ++it2;
            }
          cleanSetOfSets(tmpOrItems);
          if (result == true)
            {
              result = convertAndOrToOrAnd(tmpAndItems2, tmpOrItems2);
              cleanSetOfSets(tmpAndItems2);
              if (result == true)
                {
                  // now we can add all sets in tmpOrItems to this->mAndSets
                  copySetOfSets(tmpOrItems2, this->mAndSets);
                  // cleanup tmpOrItems2
                  cleanSetOfSets(tmpOrItems2);
                }
            }
        }
    }
  if (result == true)
    {
      // get rid of the mNot if it is set
      cleanSetOfSets(this->mChoices);
      if (this->mNot == true)
        {
          ItemSetOfSets tmpSet;
          result = negateSetOfSets(this->mAndSets, tmpSet);
          if (result == true)
            {
              cleanSetOfSets(this->mAndSets);
              this->mAndSets = tmpSet;
            }
        }
      ItemSetOfSets::iterator it2 = this->mAndSets.begin(), endit2 = this->mAndSets.end();
      ItemSetOfSets tmpAndSets;
      while (it2 != endit2)
        {
          // get rid of all not flags within the items.
          // and simplify all items
          assert((*it2).second == false);
          ItemSet::iterator it3 = (*it2).first.begin(), endit3 = (*it2).first.end();
          while (it3 != endit3)
            {
              assert((*it3).second == false);
              (*it3).first->simplify();
              ++it3;
            }
          // check if one item is equal to negating another item
          // if this is the case, the whole set can be replaced by false
          // because B AND NOT(B) is always false
          // also if we find a false item, we can eliminate all others
          bool eliminate = false;
          it3 = (*it2).first.begin();
          --endit3;
          CNormalLogicalItem* pItem1;
          CNormalLogicalItem* pItem2;
          while (it3 != endit3 && eliminate == false)
            {
              pItem1 = new CNormalLogicalItem(*(*it3).first);
              if (pItem1->getType() == CNormalLogicalItem::FALSE)
                {
                  eliminate = true;
                  break;
                }
              pItem1->negate();
              ItemSet::iterator it4 = ++it3, endit4 = (*it2).first.end();
              while (it4 != endit4 && eliminate == false)
                {
                  pItem2 = (*it4).first;
                  if (pItem2->getType() == CNormalLogicalItem::FALSE)
                    {
                      eliminate = true;
                      break;
                    }
                  if ((*pItem2) == (*pItem1))
                    {
                      eliminate = true;
                      break;
                    }
                  ++it4;
                }
              delete pItem1;
              ++it3;
            }
          if (eliminate == true)
            {
              ItemSet tmpSet;
              CNormalLogicalItem* pLogical = new CNormalLogicalItem();
              pLogical->setType(CNormalLogicalItem::FALSE);
              tmpSet.insert(std::make_pair(pLogical, false));
              tmpAndSets.insert(std::make_pair(tmpSet, false));
            }
          else
            {
              ItemSet tmpSet;
              copySet((*it2).first, tmpSet);
              tmpAndSets.insert(std::make_pair(tmpSet, false));
            }
          ++it2;
        }
      cleanSetOfSets(this->mAndSets);
      this->mAndSets = tmpAndSets;
      // simplify the sets, e.g. if one item in an AND combination is false, the
      // whole set can be replaced by one FALSE item
      // likewise if one item in an OR set is TRUE, the whole set can be replaced
      // by one TRUE item
      it2 = this->mAndSets.begin(), endit2 = --(this->mAndSets.end());
      bool eliminate = false;
      CNormalLogicalItem* pLogicalItem1, *pLogicalItem2;
      while (it2 != endit2 && eliminate == false)
        {
          ItemSetOfSets::iterator it3 = ++it2, endit3 = this->mAndSets.end();
          // if the set in it2 contains only one item and that item is a true
          // item, we can eliminate all other sets
          if ((*it2).first.size() == 1)
            {
              pLogicalItem1 = new CNormalLogicalItem(*(*(*it2).first.begin()).first);
              if (pLogicalItem1->getType() == CNormalLogicalItem::TRUE)
                {
                  eliminate = true;
                  break;
                }
              pLogicalItem1->negate();
              while (it3 != endit3 && eliminate == false)
                {
                  if ((*it3).first.size() == 1)
                    {
                      pLogicalItem2 = (*(*it3).first.begin()).first;
                      if ((*pLogicalItem1) == (*pLogicalItem2))
                        {
                          eliminate = true;
                          break;
                        }
                    }
                  ++it3;
                }
              delete pLogicalItem1;
            }
          ++it2;
        }
      if (eliminate == true)
        {
          cleanSetOfSets(this->mAndSets);
          ItemSet tmpSet;
          CNormalLogicalItem* pLogical = new CNormalLogicalItem();
          pLogical->setType(CNormalLogicalItem::TRUE);
          tmpSet.insert(std::make_pair(pLogical, false));
          this->mAndSets.insert(std::make_pair(tmpSet, false));
        }
    }
  return result;
}

/**
 * Converts a set of AND combined sets of OR combined elements into a
 * target set of OR combined sets of AND combined elements.
 */
template<typename TYPE>
bool CNormalLogical::convertAndOrToOrAnd(const std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >& source,
    std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >& target)
{
  bool result = true;
  if (source.size() > 2)
    {
      typename std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> > tmpSourceSet((++source.begin()), source.end());
      typename std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> > tmpTargetSet;
      result = ((*source.begin()).second == false);
      if (result == true)
        {
          result = convertAndOrToOrAnd(tmpSourceSet, tmpTargetSet);
          if (result == true)
            {
              // for each item in source.begin().first go through all sets in
              // tmpTarget
              typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >::const_iterator it = (*source.begin()).first.begin(), endit = (*source.begin()).first.end();
              while (it != endit)
                {
                  typename std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >::const_iterator it2 = tmpTargetSet.begin(), endit2 = tmpTargetSet.end();
                  while (it2 != endit2)
                    {
                      typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> > tmpSet;
                      tmpSet.insert(std::make_pair(new TYPE(*(*it).first), false));
                      typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >::const_iterator it3 = (*it2).first.begin(), endit3 = (*it2).first.end();
                      while (it3 != endit3)
                        {
                          tmpSet.insert(std::make_pair(new TYPE(*(*it3).first), false));
                          ++it3;
                        }
                      ++it2;
                    }
                  ++it;
                }
            }
        }
      // cleanup tmpTarget
      typename std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >::iterator it = tmpTargetSet.begin(), endit = tmpTargetSet.end();
      while (it != endit)
        {
          typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >::iterator it2 = (*it).first.begin(), endit2 = (*it).first.end();
          while (it2 != endit2)
            {
              delete (*it2).first;
              ++it2;
            }
          ++it;
        }
    }
  else if (source.size() == 1)
    {
      if ((*source.begin()).second == true)
        {
          result = false;
        }
      else
        {
          const typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> > item = (*source.begin()).first;
          typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >::const_iterator it = item.begin(), endit = item.end();
          while (it != endit && result == true)
            {
              if ((*it).second == true)
                {
                  result = false;
                }
              else
                {
                  typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> > tmpSet;
                  tmpSet.insert(std::make_pair(new TYPE(*(*it).first), false));
                  target.insert(std::make_pair(tmpSet, false));
                }
              ++it;
            }
        }
    }
  if (result == false)
    {
      // delete all elements in target
      typename std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >::iterator it = target.begin(), endit = target.end();
      while (it != endit)
        {
          typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >::iterator it2 = (*it).first.begin(), endit2 = (*it).first.end();
          while (it2 != endit2)
            {
              delete (*it2).first;
              ++it2;
            }
          ++it;
        }
      target.clear();
    }
  return result;
}

/**
 * Negates a set of elements.
 * The result of the operation is returned in target.
 * The type of result depends on the source. If the source was a set of AND
 * combined elements, the result is a set of OR combined elements and vice versa.
 * target set.
 */
template<typename TYPE>
bool CNormalLogical::negateSets(const std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >& source, std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >& target)
{
  bool result = true;
  typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >::const_iterator it = source.begin(), endit = source.end();
  while (it != endit)
    {
      if ((*it).second == false)
        {
          TYPE* pItem = new TYPE(*(*it).first);
          pItem->negate();
          target.insert(std::make_pair(pItem, false));
        }
      else
        {
          target.insert(std::make_pair(new TYPE(*(*it).first), false));
        }
      ++it;
    }
  return result;
}

/**
 * Negates a set of sets with elements.
 * The result of the operation is returned in target.
 * The type of the result depends on the source.
 * If the source was a set of AND combined sets of OR combined
 * elements, the rersult will be a set of OR combined sets with AND combined
 * elements.
 */
template<typename TYPE>
bool CNormalLogical::negateSetOfSets(const std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >& source, std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >& target)
{
  bool result = true;
  typename std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >::const_iterator it = source.begin(), endit = source.end();
  while (it != endit && result == true)
    {
      std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> > tmpTarget;
      if ((*it).second == false)
        {
          result = negateSets((*it).first, tmpTarget);
        }
      else
        {
          typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >::const_iterator it2 = (*it).first.begin(), endit2 = (*it).first.end();
          while (it2 != endit2)
            {
              tmpTarget.insert(std::make_pair(new TYPE(*(*it2).first), (*it2).second));
              ++it2;
            }
        }
      target.insert(std::make_pair(tmpTarget, false));
      ++it;
    }
  if (result == false)
    {
      // cleanup target
      it = target.begin(), endit = target.end();
      while (it != endit)
        {
          typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >::const_iterator it2 = (*it).first.begin(), endit2 = (*it).first.end();
          while (it2 != endit2)
            {
              delete (*it2).first;
              ++it2;
            }
          ++it;
        }
      target.clear();
    }
  return result;
}

template<typename TYPE>
void CNormalLogical::cleanSet(std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >& s)
{
  typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >::const_iterator it = s.begin(), endit = s.end();
  while (it != endit)
    {
      delete (*it).first;
      ++it;
    }
  s.clear();
}

template<typename TYPE>
void CNormalLogical::cleanSetOfSets(std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >& s)
{
  typename std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >::iterator it = s.begin(), endit = s.end();
  while (it != endit)
    {
      std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> > tmpSet = (*it).first;
      cleanSet(tmpSet);
      ++it;
    }
  s.clear();
}

template<typename TYPE>
void CNormalLogical::copySet(const std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >& source, std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >& target)
{
  typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >::const_iterator it = source.begin(), endit = source.end();
  while (it != endit)
    {
      target.insert(std::make_pair(new TYPE(*(*it).first), (*it).second));
      ++it;
    }
}

template<typename TYPE>
void CNormalLogical::copySetOfSets(const std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >& source,
                                   std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >& target)
{
  typename std::set<std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>, CNormalLogical::SetOfSetsSorter<TYPE> >::const_iterator it = source.begin(), endit = source.end();
  while (it != endit)
    {
      typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> > tmpSet;
      copySet((*it).first, tmpSet);
      target.insert(std::make_pair(tmpSet, (*it).second));
      ++it;
    }
}

/**
 * This routine compares a set of sets and returns true if the first
 * argument is smaller than the second.
 */
template<typename TYPE>
bool CNormalLogical::SetOfSetsSorter<TYPE>::operator()(const std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>& lhs,
    const std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>& rhs) const
  {
    bool result = true;
    if (lhs.second == rhs.second)
      {
        if (lhs.first.size() == rhs.first.size())
          {
            typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >::const_iterator it = lhs.first.begin(), endit = lhs.first.end(), it2 = rhs.first.begin();
            while (it != endit && result == true)
              {
                if ((*it).second == (*it2).second)
                  {
                    result = ((*(*it).first) < (*(*it2).first));
                  }
                else if ((*it2).second == true)
                  {
                    result = false;
                  }
                ++it;
                ++it2;
              }
          }
        else if (rhs.first.size() < lhs.first.size())
          {
            result = false;
          }
      }
    else if (rhs.second == true)
      {
        result = false;
      }
    return result;
  }

/**
 * This routine compares a set of sets and returns true if the first
 * argument is smaller than the second.
 */
template<typename TYPE>
bool CNormalLogical::SetOfSetsSorter<TYPE>::isEqual(const std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>& lhs,
    const std::pair<std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >, bool>& rhs) const
  {
    bool result = true;
    if (lhs.second == rhs.second)
      {
        if (lhs.first.size() == rhs.first.size())
          {
            typename std::set<std::pair<TYPE*, bool>, CNormalLogical::SetSorter<TYPE> >::const_iterator it = lhs.first.begin(), endit = lhs.first.end(), it2 = rhs.first.begin();
            while (it != endit && result == true)
              {
                result = (((*it).second == ((*it2).second)) && (*(*it).first) == (*(*it2).first));
                ++it;
                ++it2;
              }
          }
        else
          {
            result = false;
          }
      }
    else
      {
        result = false;
      }
    return result;
  }

/**
 * This routine compares a set and returns true if the first
 * argument is smaller than the second.
 */
template<typename TYPE>
bool CNormalLogical::SetSorter<TYPE>::operator()(const std::pair<TYPE*, bool>& lhs, const std::pair<TYPE*, bool>& rhs) const
  {
    bool result = false;
    if (lhs.second == true && rhs.second == false)
      {
        result = true;
      }
    else if (lhs.second == rhs.second)
      {
        result = ((*lhs.first) < (*rhs.first));
      }
    return result;
  }

/**
 * This routine compares a set and returns true if the first
 * argument is equal to the second.
 */
template<typename TYPE>
bool CNormalLogical::SetSorter<TYPE>::isEqual(const std::pair<TYPE*, bool>& lhs, const std::pair<TYPE*, bool>& rhs) const
  {
    bool result = true;
    if (!(lhs.second == rhs.second && (*lhs.first) == (*rhs.first)))
      {
        result = false;
      }
    return result;
  }

bool CNormalLogical::operator==(const CNormalLogical& rhs) const
  {
    bool result = true;
    if (this->mNot == rhs.mNot)
      {
        if (this->mChoices.size() != rhs.mChoices.size() || this->mAndSets.size() != rhs.mAndSets.size())
          {
            result = false;
          }
        else
          {
            ChoiceSetOfSets::const_iterator it = this->mChoices.begin(), endit = this->mChoices.end(), it2 = rhs.mChoices.begin();
            CNormalLogical::SetOfSetsSorter<CNormalChoiceLogical> comp;
            while (it != endit)
              {
                if (comp.isEqual(*it, *it2) == false)
                  {
                    result = false;
                    break;
                  }
                ++it;
                ++it2;
              }
            ItemSetOfSets::const_iterator it3 = this->mAndSets.begin(), endit3 = this->mAndSets.end(), it4 = rhs.mAndSets.begin();
            CNormalLogical::SetOfSetsSorter<CNormalLogicalItem> comp2;
            while (it3 != endit3 && result == true)
              {
                if (comp2.isEqual(*it3, *it4) == false)
                  {
                    result = false;
                    break;
                  }
                ++it3;
                ++it4;
              }
          }
      }
    else
      {
        result = false;
      }
    return result;
  }

bool CNormalLogical::operator<(const CNormalLogical& rhs) const
  {
    bool result = true;
    if (this->mNot == false && rhs.mNot == true)
      {
        result = false;
      }
    else if (this->mNot == rhs.mNot)
      {
        if (this->mChoices.size() < rhs.mChoices.size())
          {
            result = false;
          }
        else if (this->mChoices.size() == rhs.mChoices.size())
          {
            ChoiceSetOfSets::const_iterator it = this->mChoices.begin(), endit = this->mChoices.end(), it2 = rhs.mChoices.begin();
            CNormalLogical::SetOfSetsSorter<CNormalChoiceLogical> comp;
            while (it != endit)
              {
                if (comp(*it, *it2) == false)
                  {
                    result = false;
                    break;
                  }
                ++it;
                ++it2;
              }
            if (result == true)
              {
                if (this->mAndSets.size() > rhs.mAndSets.size())
                  {
                    result = false;
                  }
                else if (this->mAndSets.size() == rhs.mAndSets.size())
                  {
                    ItemSetOfSets::const_iterator it3 = this->mAndSets.begin(), endit3 = this->mAndSets.end(), it4 = rhs.mAndSets.begin();
                    CNormalLogical::SetOfSetsSorter<CNormalLogicalItem> comp2;
                    while (it3 != endit3)
                      {
                        if (comp2(*it3, *it4) == false)
                          {
                            result = false;
                            break;
                          }
                        ++it3;
                        ++it4;
                      }
                  }
              }
          }
      }
    return result;
  }
