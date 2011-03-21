/* Begin CVS Header
  $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/function/CEvaluationNodeNumber.h,v $
  $Revision: 1.15 $
  $Name:  $
  $Author: shoops $
  $Date: 2011/03/21 15:48:19 $
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

#ifndef COPASI_CEvaluationNodeNumber
#define COPASI_CEvaluationNodeNumber

class CCopasiDataModel;

/**
 * This is class for nodes presenting numbers used in an evaluation trees
 */
class CEvaluationNodeNumber : public CEvaluationNode
{
public:
  /**
   * Enumeration of possible node types.
   */
  enum SubType
  {
    INVALID = 0x00FFFFFF,
    DOUBLE = 0x00000000,
    INTEGER = 0x00000001,
    ENOTATION = 0x00000002,
    RATIONALE = 0x00000003
  };

  // Operations
private:
  /**
   * Default constructor
   */
  CEvaluationNodeNumber();

public:
  /**
   * Specific constructor
   * @param const SubType & subType
   * @param const Data & data
   */
  CEvaluationNodeNumber(const SubType & subType,
                        const Data & data);

  /**
   * Specific constructor
   * @param const C_FLOAT64 & number
   */
  CEvaluationNodeNumber(const C_FLOAT64 & number);

  /**
   * Copy constructor
   * @param const CEvaluationNodeNumber & src
   */
  CEvaluationNodeNumber(const CEvaluationNodeNumber & src);

  /**
   * Destructor
   */
  virtual ~CEvaluationNodeNumber();

  /**
   * Creates a new CEvaluationNodeNumber from an ASTNode.
   * @param const ASTNode* node
   * @return CEvaluationNode* return a pointer to the newly created node;
   */
  static CEvaluationNode* createNodeFromASTTree(const ASTNode& node);

  /**
   * Create a new ASTNode corresponding to this NumberNode.
   * @return ASTNode* return a pointer to the newly created node;
   */
  ASTNode* toAST(const CCopasiDataModel* pDataModel) const;

  /**
   * generate display MathML recursively
   */
  virtual void writeMathML(std::ostream & out,
                           const std::vector<std::vector<std::string> > & env,
                           bool expand = true,
                           size_t l = 0) const;
};

#endif // COPASI_CEvaluationNodeNumber
