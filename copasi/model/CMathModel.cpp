// Copyright (C) 2010 - 2013 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include <cmath>

#include "copasi.h"

#include "CMathModel.h"
#include "CModel.h"

#include "blaswrap.h"
#include "clapackwrap.h"

CMathModel::CMathModel(const CCopasiContainer * pParent) :
  CCopasiContainer("MathModel", pParent, "CMathModel"),
  mpModel(NULL),
  mProcessQueue(),
  mEvents("ListOfMathEvents", this),
  mRootValues(),
  mRootDiscrete(),
  mRootRefreshes(),
  mRootIndex2Event(),
  mRootIndex2RootFinder()
{}

CMathModel::CMathModel(const CMathModel & src,
                       const CCopasiContainer * pParent) :
  CCopasiContainer(src, pParent),
  mpModel(src.mpModel),
  mProcessQueue(src.mProcessQueue),
  mEvents("ListOfMathEvents", this),
  mRootValues(),
  mRootDiscrete(),
  mRootRefreshes(),
  mRootIndex2Event(),
  mRootIndex2RootFinder()
{
  // Compile the math model.
  compile(mpModel);

  // TODO
  // The problem is that the pointers to events in the process queue point
  // to events in the source model. We can construct a mapping of source event pointers
  // to event pointers and update each process queue entry.

  // For the moment we enforce that the process queue is empty.
  assert(mProcessQueue.isEmpty());
}

// virtual
CMathModel::~CMathModel()
{}

bool CMathModel::compile(CModel * pModel)
{
  // For each CEvent we need to create a CMathEvent
  mEvents.clear();

  // We clear the sequence of root refreshes to avoid accessing invalid pointers.
  mRootRefreshes.clear();

  // We clear the map to avoid accessing no longer existing events.
  mRootIndex2Event.resize(0);

  // We clear the map to avoid accessing no longer existing root finders.
  mRootIndex2RootFinder.resize(0);

  mpModel = pModel;

  if (mpModel == NULL)
    return false;

  // Now we are ready to start the compilations.
  bool success = true;

  // The context in which this model is compiled.
  // TODO When the math model is the only thing required for simulation this should be changed
  // to this.
  std::vector< CCopasiContainer * > Context;
  Context.push_back(mpModel);

  CCopasiVectorN< CEvent >::const_iterator itEvent = mpModel->getEvents().begin();
  CCopasiVectorN< CEvent >::const_iterator endEvent = mpModel->getEvents().end();

  size_t RootFinderCount = 0;

  for (; itEvent != endEvent; ++itEvent)
    {
      CMathEvent * pEvent = new CMathEvent();
      mEvents.add(pEvent, true);
      success &= pEvent->compile(*itEvent, Context);

      RootFinderCount += pEvent->getMathTrigger().getRootFinders().size();
    }

  // We need to create a CVector of pointers to the current root finder values
  mRootValues.resize(RootFinderCount);
  C_FLOAT64 ** ppRootValue = mRootValues.array();

  // We need to create a CVector indicating whether the roots only change
  // during discrete events.
  mRootDiscrete.resize(RootFinderCount);
  bool * pRootDiscrete = mRootDiscrete.array();

  // We need create a map of root indexes to events.
  mRootIndex2Event.resize(RootFinderCount);
  CMathEvent ** ppEvent = mRootIndex2Event.array();

  // We need create a map of root indexes to root finders.
  mRootIndex2RootFinder.resize(RootFinderCount);
  CMathTrigger::CRootFinder ** ppRootFinder = mRootIndex2RootFinder.array();

  // We need to list all state variables to find out whether the roots
  // only change during discrete events.
  std::set< const CCopasiObject * > StateVariables;
  CModelEntity *const*ppEntity = mpModel->getStateTemplate().getEntities();
  CModelEntity *const*ppEntityEnd = mpModel->getStateTemplate().endDependent();

  for (; ppEntity != ppEntityEnd; ++ppEntity)
    {
      StateVariables.insert((*ppEntity)->getValueReference());
    }

  std::set< const CCopasiObject * > RootValuesDependencies;

  CCopasiVector< CMathEvent >::const_iterator itMathEvent = mEvents.begin();
  CCopasiVector< CMathEvent >::const_iterator endMathEvent = mEvents.end();

  // for each event
  for (; itMathEvent != endMathEvent; ++itMathEvent)
    {
      CCopasiVector< CMathTrigger::CRootFinder >::const_iterator itRootFinder =
        (*itMathEvent)->getMathTrigger().getRootFinders().begin();
      CCopasiVector< CMathTrigger::CRootFinder >::const_iterator endRootFinder =
        (*itMathEvent)->getMathTrigger().getRootFinders().end();

      // for each root finder
      for (; itRootFinder != endRootFinder;
           ++itRootFinder, ++ppRootValue, ++pRootDiscrete, ++ppEvent, ++ppRootFinder)
        {
          // Update the vector of pointers to current root values.
          *ppRootValue = (*itRootFinder)->getRootValuePtr();

          // Store whether a root changes only during discrete events.
          (*itRootFinder)->determineDiscrete(StateVariables);
          *pRootDiscrete = (*itRootFinder)->isDiscrete();

          // Build the mapping from root values indexes to CMathEvents
          *ppEvent = *itMathEvent;

          // Build the mapping from root values indexes to root finders
          *ppRootFinder = *itRootFinder;

          // The root finder needs to be up to date
          RootValuesDependencies.insert(*itRootFinder);
        }
    }

  // We need to compile a refresh sequence which calculates all root values.
  mRootRefreshes = buildRequiredRefreshList(RootValuesDependencies);

  return success;
}

void CMathModel::evaluateRoots(CVectorCore< C_FLOAT64 > & rootValues,
                               const bool & ignoreDiscrete)
{
  // Apply all needed refresh calls to calculate the current root values.
  std::vector< Refresh * >::const_iterator itRefresh = mRootRefreshes.begin();
  std::vector< Refresh * >::const_iterator endRefresh = mRootRefreshes.end();

  while (itRefresh != endRefresh)
    (**itRefresh++)();

  // Copy the current values to the output vector.
  assert(rootValues.size() == mRootValues.size());

  C_FLOAT64 *pTarget = rootValues.array();
  C_FLOAT64 **pSrc = mRootValues.array();
  C_FLOAT64 **pSrcEnd = pSrc + mRootValues.size();

  if (ignoreDiscrete)
    {
      bool *pDiscrete = mRootDiscrete.array();

      for (; pSrc != pSrcEnd; ++pSrc, ++pDiscrete, ++pTarget)
        {
          *pTarget = (*pDiscrete) ? 1.0 : **pSrc;
        }
    }
  else
    {
      for (; pSrc != pSrcEnd; ++pSrc, ++pTarget)
        {
          *pTarget = **pSrc;
        }
    }

  return;
}

const C_FLOAT64 & CMathModel::getInitialTime() const
{
  return mpModel->getInitialTime();
}

bool CMathModel::processQueue(const C_FLOAT64 & time,
                              const bool & equality,
                              CProcessQueue::resolveSimultaneousAssignments pResolveSimultaneousAssignments)
{
  return mProcessQueue.process(time, equality, pResolveSimultaneousAssignments);
}

void CMathModel::processRoots(const C_FLOAT64 & time,
                              const bool & equality,
                              const bool & correct,
                              const CVector< C_INT > & foundRoots)
{
  // Apply all needed refresh calls to calculate the current root values.
  std::vector< Refresh * >::const_iterator itRefresh = mRootRefreshes.begin();
  std::vector< Refresh * >::const_iterator endRefresh = mRootRefreshes.end();

  while (itRefresh != endRefresh)
    (**itRefresh++)();

  assert(foundRoots.size() == mRootIndex2Event.size());

  // All events associated with the found roots need to be evaluated whether they fire.
  // In case one fires the corresponding event needs to be scheduled in the process queue.

  const C_INT *pFoundRoot = foundRoots.array();
  const C_INT *pFoundRootEnd = pFoundRoot + foundRoots.size();

  CMathEvent ** ppEvent = mRootIndex2Event.array();
  CMathEvent * pProcessEvent = NULL;

  CMathTrigger::CRootFinder **ppRootFinder = mRootIndex2RootFinder.array();

  if (correct)
    {
      while (pFoundRoot != pFoundRootEnd)
        {
          // We reevaluate the state of the non found roots, which should be save.
          if (*pFoundRoot < 1 && (*ppRootFinder)->isEquality() == equality)
            {
              (*ppRootFinder)->calculateTrueValue();
            }

          ++pFoundRoot; ++ppRootFinder;
        }
    }

  pFoundRoot = foundRoots.array();
  ppRootFinder = mRootIndex2RootFinder.array();

  // We go through the list of roots and process the events
  // which need to be checked whether they fire.
  bool TriggerBefore = true;

  while (pFoundRoot != pFoundRootEnd)
    {
      pProcessEvent = *ppEvent;
      TriggerBefore = pProcessEvent->getMathTrigger().calculate();

      // Process the events for which we have found a root.
      // A found root is indicated by roots[i] = 1 or 0 otherwise.
      while (pFoundRoot != pFoundRootEnd &&
             *ppEvent == pProcessEvent)
        {
          // We must only toggle the roots which are marked.
          if (*pFoundRoot > 0)
            {
              (*ppRootFinder)->toggle(time, equality, correct);
            }

          ++pFoundRoot; ++ppEvent; ++ppRootFinder;
        }

      bool TriggerAfter = pProcessEvent->getMathTrigger().calculate();

      // Check whether the event fires
      if (TriggerAfter == true &&
          TriggerBefore == false)
        {
          pProcessEvent->fire(time, equality, mProcessQueue);
        }
    }

  return;
}

void CMathModel::processRoots(const C_FLOAT64 & time,
                              const CVector< C_INT > & foundRoots)
{
  // Apply all needed refresh calls to calculate the current root values.
  std::vector< Refresh * >::const_iterator itRefresh = mRootRefreshes.begin();
  std::vector< Refresh * >::const_iterator endRefresh = mRootRefreshes.end();

  while (itRefresh != endRefresh)
    (**itRefresh++)();

  assert(foundRoots.size() == mRootIndex2Event.size());

  // All events associated with the found roots need to be evaluated whether they fire.
  // In case one fires the corresponding event needs to be scheduled in the process queue.

  const C_INT *pFoundRoot = foundRoots.array();
  const C_INT *pFoundRootEnd = pFoundRoot + foundRoots.size();

  CMathEvent ** ppEvent = mRootIndex2Event.array();
  CMathEvent * pProcessEvent = NULL;

  CMathTrigger::CRootFinder **ppRootFinder = mRootIndex2RootFinder.array();

  while (pFoundRoot != pFoundRootEnd)
    {
      // We reevaluate the state of the non found roots, which should be save.
      if (*pFoundRoot < 1)
        {
          (*ppRootFinder)->calculateTrueValue();
        }

      ++pFoundRoot; ++ppRootFinder;
    }

  pFoundRoot = foundRoots.array();
  ppRootFinder = mRootIndex2RootFinder.array();

  // We go through the list of roots and process the events
  // which need to be checked whether they fire.
  bool TriggerBefore = true;

  while (pFoundRoot != pFoundRootEnd)
    {
      pProcessEvent = *ppEvent;
      TriggerBefore = pProcessEvent->getMathTrigger().calculate();

      // Process the events for which we have found a root.
      // A found root is indicated by roots[i] = 1 or 0 otherwise.
      while (pFoundRoot != pFoundRootEnd &&
             *ppEvent == pProcessEvent)
        {
          // We must only toggle the roots which are marked.
          if (*pFoundRoot > 0)
            {
              (*ppRootFinder)->toggle(time);
            }

          ++pFoundRoot; ++ppEvent; ++ppRootFinder;
        }

      bool TriggerAfter = pProcessEvent->getMathTrigger().calculate();

      // Check whether the event fires
      if (TriggerAfter == true &&
          TriggerBefore == false)
        {
          // We are dealing with discrete root processing and can not distinguish
          // between equality and inequality. Thus we just pick equality = true
          pProcessEvent->fire(time, true, mProcessQueue);
        }
    }

  return;
}

const C_FLOAT64 & CMathModel::getProcessQueueExecutionTime() const
{
  return mProcessQueue.getProcessQueueExecutionTime();
}

const CProcessQueue & CMathModel::getProcessQueue() const
{
  return mProcessQueue;
}

CProcessQueue & CMathModel::getProcessQueue()
{
  return mProcessQueue;
}

void CMathModel::applyInitialValues()
{
  // Clear the process queue.
  mProcessQueue.initialize(this);

  // Prepare the roots, i.e., evaluate them
  // Apply all needed refresh calls to calculate the current root values.
  std::vector< Refresh * >::const_iterator itRefresh = mRootRefreshes.begin();
  std::vector< Refresh * >::const_iterator endRefresh = mRootRefreshes.end();

  while (itRefresh != endRefresh)
    (**itRefresh++)();

  // Now calculate the current root activities
  CCopasiVector< CMathEvent >::const_iterator itMathEvent = mEvents.begin();
  CCopasiVector< CMathEvent >::const_iterator endMathEvent = mEvents.end();

  // for each event
  for (; itMathEvent != endMathEvent; ++itMathEvent)
    {
      (*itMathEvent)->getMathTrigger().applyInitialValues();
    }

  // We need to schedule events which fire at t > t_0

  // The roots which are checked for inequality with a current root value
  // zero are the candidates. We should toggle them if their time derivative
  // is positive.

  // The time derivative of the roots can be calculated as:
  // Dr_i/Dt = sum_j (dr_i/dx_j dx_j/dt) + dr_i/dt
  // dx_j/dt are the rates of the state variables
  CVector< C_INT > InitialRoots;

  if (determineInitialRoots(InitialRoots))
    {
      processRoots(mpModel->getInitialTime(), false, false, InitialRoots);
    }
}

size_t CMathModel::getNumRoots() const
{
  return mRootValues.size();
}

void CMathModel::calculateRootDerivatives(CVector< C_FLOAT64 > & rootDerivatives)
{
  size_t NumCols =
    mpModel->getStateTemplate().getNumIndependent() + mpModel->getNumDependentReactionMetabs() + 1;

  CVector< C_FLOAT64 > Rates(NumCols);
  C_FLOAT64 * pRate = Rates.array();
  *pRate = 1.0; // for time

  mpModel->updateSimulatedValues(false);
  mpModel->calculateDerivatives(pRate + 1);

  CMatrix< C_FLOAT64 > Jacobian;
  calculateRootJacobian(Jacobian, Rates);

  rootDerivatives.resize(mRootValues.size());
  C_FLOAT64 * pDerivative = rootDerivatives.array();

  // Now calculate derivatives of all metabolites determined by reactions
  char T = 'N';
  C_INT M = 1;
  C_INT N = (C_INT) mRootValues.size();
  C_INT K = (C_INT) NumCols;
  C_FLOAT64 Alpha = 1.0;
  C_FLOAT64 Beta = 0.0;

  dgemm_(&T, &T, &M, &N, &K, &Alpha, Rates.array(), &M,
         Jacobian.array(), &K, &Beta, pDerivative, &M);
}

const CVector< CMathTrigger::CRootFinder * > & CMathModel::getRootFinders() const
{
  return mRootIndex2RootFinder;
}

std::vector< Refresh * > CMathModel::buildRequiredRefreshList(const std::set< const CCopasiObject * > & requiredObjects) const
{
  // We assume that all values determined by ODEs and reactions are current.
  // This means for reduced models moieties must have been applied.
  std::set< const CCopasiObject * > UpToDate;

  return CCopasiObject::buildUpdateSequence(requiredObjects, UpToDate);
}

std::vector< Refresh * > CMathModel::buildDependendRefreshList(const std::set< const CCopasiObject * > & changedObjects) const
{
  if (mpModel == NULL)
    return std::vector< Refresh * >();

  // We cannot use the default dependencies for a species which concentration was changed
  // by an event as the concentration is the dependent value. This must not be done for
  // species which where not changed by the event, i.e., the solution must be context specific.
  // This implies that the methods getDirectDependencies and its callers must be aware of the species
  // concentrations in the list of the changed objects.

  // CCopasiObject::mDependencies is private this means the only place where can be accessed is
  // CCopasiObject itself. We need to replace all read access the mDirectDependencies with
  // getDirectDependencies in CCopasiObject and than make this method context sensitive. This new
  // method will be virtual and must be overloaded for the concentration and particle number in species.

  // We build a list of all simulated objects which depend on the changed values.
  std::set< const CCopasiObject * > RequiredObjects;

  // The values which need to be updated are all values needed for simulation.
  // These are all objects which are required to calculate the reaction rates and
  // the right hand side of ODEs in other words all rates of entities of type reaction
  // or ODE.

  // Due to the context sensitivity, i.e., the changed concentration is no longer dependent
  // on the particle number and compartment volume, we will miss all particle numbers of changed
  // species which do not directly appear in any calculation.

  CModelEntity *const* ppEntity = mpModel->getStateTemplate().getEntities();
  CModelEntity *const* ppEndEntity = ppEntity + mpModel->getStateTemplate().size();

  const CMetab * pSpecies;

  for (; ppEntity != ppEndEntity; ++ppEntity)
    {
      switch ((*ppEntity)->getStatus())
        {
            // First rates of all entities depending on ODEs and species depending on reactions.
          case CModelEntity::ODE:
          case CModelEntity::REACTIONS:

            if ((*ppEntity)->getRateReference()->dependsOn(changedObjects, changedObjects))
              {
                RequiredObjects.insert((*ppEntity)->getRateReference());
              }

            // The break statement is intentionally missing since we need to check
            // the particle values of species for the above and for fixed.
          case CModelEntity::FIXED:
            pSpecies = dynamic_cast< const CMetab * >(*ppEntity);

            if (pSpecies != NULL &&
                pSpecies->getValueReference()->dependsOn(changedObjects, changedObjects))
              {
                RequiredObjects.insert(pSpecies->getValueReference());
              }

            break;

          case CModelEntity::ASSIGNMENT:
            // Assignments may have to be recalculated but only if an ODE or a
            // reaction flux depends on it.
            break;

          case CModelEntity::TIME:
            // The model time will not advance during an event. For discrete time
            // simulation the users should add a global quantity representing the time.
            break;
        }
    }

  // Since we have discontinuous change we have to evaluate the total amount of the moieties
  // which participant changed concentration.

  CCopasiVector< CMoiety >::const_iterator itMoiety = mpModel->getMoieties().begin();
  CCopasiVector< CMoiety >::const_iterator endMoiety = mpModel->getMoieties().end();

  for (; itMoiety != endMoiety; ++itMoiety)
    {
      if ((*itMoiety)->getValueReference()->dependsOn(changedObjects, changedObjects))
        {
          RequiredObjects.insert((*itMoiety)->getValueReference());
        }
    }

  std::set< const CCopasiObject * > UpToDate;
  return CCopasiObject::buildUpdateSequence(RequiredObjects, UpToDate, changedObjects);
}

bool CMathModel::determineInitialRoots(CVector< C_INT > & foundRoots)
{
  bool Found = false;

  CVector< C_FLOAT64 > RootDerivatives;
  calculateRootDerivatives(RootDerivatives);

  foundRoots.resize(mRootValues.size());
  C_INT * pFoundRoot = foundRoots.array();
  C_FLOAT64 ** pRootValue = mRootValues.array();
  C_FLOAT64 * pDerivative = RootDerivatives.array();
  CMathTrigger::CRootFinder ** ppRootFinder = mRootIndex2RootFinder.array();
  CMathTrigger::CRootFinder ** ppEndRootFinder = ppRootFinder + mRootIndex2RootFinder.size();

  for (; ppRootFinder != ppEndRootFinder;
       ++ppRootFinder, ++pRootValue, ++pDerivative, ++pFoundRoot)
    {
      if (**pRootValue == 0.0)
        {
          if (!(*ppRootFinder)->isEquality() &&
              *pDerivative > 0.0)
            {
              *pFoundRoot = 1;
              Found = true;

              continue;
            }

          if ((*ppRootFinder)->isEquality() &&
              *pDerivative < 0.0)
            {
              (*ppRootFinder)->toggle(std::numeric_limits< C_FLOAT64 >::quiet_NaN(), true, false);
            }
        }

      *pFoundRoot = 0;
    }

  return Found;
}

void CMathModel::calculateRootJacobian(CMatrix< C_FLOAT64 > & jacobian,
                                       const CVector< C_FLOAT64 > & rates)
{
  CState State = mpModel->getState();

  size_t NumRows = mRootValues.size();

  // Partial derivatives with respect to time and all variables determined by ODEs and reactions.
  size_t NumCols =
    State.getNumIndependent() + mpModel->getNumDependentReactionMetabs() + 1;

  size_t Col = 0;

  jacobian.resize(NumRows, NumCols);

  C_FLOAT64 X1 = 0.0;
  C_FLOAT64 X2 = 0.0;
  C_FLOAT64 InvDelta = 0.0;

  CVector< C_FLOAT64 > Y1(NumRows);
  CVector< C_FLOAT64 > Y2(NumRows);

  C_FLOAT64 * pX = State.beginIndependent() - 1;
  C_FLOAT64 * pXEnd = pX + NumCols;

  C_FLOAT64 * pJacobian = jacobian.array();
  C_FLOAT64 * pJacobianEnd = pJacobian + jacobian.size();

  const C_FLOAT64 * pRate = rates.array();

  for (; pX != pXEnd; ++pX, ++Col, ++pRate)
    {
      C_FLOAT64 Store = *pX;

      if (fabs(*pRate) < 1e4 * std::numeric_limits< C_FLOAT64 >::epsilon() * fabs(Store) ||
          fabs(*pRate) < 1e4 * std::numeric_limits< C_FLOAT64 >::min())
        {
          if (fabs(Store) < 100.0 * std::numeric_limits< C_FLOAT64 >::min())
            {
              X1 = 0.0;

              if (Store < 0.0)
                X2 = -200.0 * std::numeric_limits< C_FLOAT64 >::min();
              else
                X2 = 200.0 * std::numeric_limits< C_FLOAT64 >::min();

              InvDelta = X2;
            }
          else
            {
              X1 = 0.999 * Store;
              X2 = 1.001 * Store;
              InvDelta = 500.0 / Store;
            }
        }
      else
        {
          X1 = Store - 0.001 * *pRate;
          X2 = Store + 0.001 * *pRate;
          InvDelta = 500.0 / *pRate;
        }

      *pX = X1;
      mpModel->setState(State);
      evaluateRoots(Y1, true);

      *pX = X2;
      mpModel->setState(State);
      evaluateRoots(Y2, true);

      *pX = Store;

      pJacobian = jacobian.array() + Col;
      C_FLOAT64 * pY1 = Y1.array();
      C_FLOAT64 * pY2 = Y2.array();

      for (; pJacobian < pJacobianEnd; pJacobian += NumCols, ++pY1, ++pY2)
        * pJacobian = (*pY2 - *pY1) * InvDelta;
    }

  // Undo the changes from evaluateRoots.
  mpModel->setState(State);
  evaluateRoots(Y2, true);
}
