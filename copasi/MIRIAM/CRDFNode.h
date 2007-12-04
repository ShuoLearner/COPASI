// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/MIRIAM/CRDFNode.h,v $
//   $Revision: 1.2 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2007/11/21 16:15:07 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#ifndef COPASI_CRDFNode
#define COPASI_CRDFNode

#include <string>
#include <vector>

#include "MIRIAM/CRDFEdge.h"

class CRDFSubject;
class CRDFObject;

class CRDFNode
  {
    // Operations
  public:
    /**
     * Default Constructor
     */
    CRDFNode();

    /**
     * Destructor
     */
    ~CRDFNode();

    /**
     * Set the Id of the node
     * @param const std::string & id
     */
    void setId(const std::string & id);

    /**
     * Retrieve the Id of the node
     * @return const std::string & id
     */
    const std::string & getId() const;

    /**
     * Set the subject data of the node
     * @param const CRDFSubject & subject
     */
    void setSubject(const CRDFSubject & subject);

    /**
     * Retrieve the subject data of the node. It is recommended to
     * call isSubjectNode before retrieving the subject.
     * @return const CRDFSubject & subject
     */
    const CRDFSubject & getSubject() const;

    /**
     * Set the object data of the node
     * @param const CRDFObject & object
     */
    void setObject(const CRDFObject & object);

    /**
     * Retrieve the object data of the node. It is recommended to
     * call isObjectNode before retrieving the object.
     * @return const CRDFObject & object
     */
    const CRDFObject & getObject() const;

    /**
     * Add a property edge to the node
     * @param const std::string & predicate
     * @param CRDFNode * pObject
     */
    void addEdge(const std::string & predicate,
                 CRDFNode * pObject);

    /**
     * Retrieve the property edges of the node
     * @return const std::vector< CRDFEdge > & edges
     */
    const std::vector< CRDFEdge > & getEdges() const;

    /**
     * Check whether this is a subject node
     * @return bool isSubjectNode
     */
    bool isSubjectNode() const;

    /**
     * Check whether this is a object node
     * @return bool isObjectNode
     */
    bool isObjectNode() const;

    // Attributes
  private:
    /**
     * The Id of the node
     */
    std::string mId;

    /**
     * A pointer to the subject if of type SUBJECT or BOTH
     */
    mutable CRDFSubject * mpSubject;

    /**
     * A pointer to the object if of type OBJECT or BOTH
     */
    mutable CRDFObject * mpObject;

    /**
     * A vector of property edges
     */
    std::vector< CRDFEdge > mEdges;

    /**
     * Stores whether this is a blank node
     */
    bool mIsBlankNode;
  };

#endif // COPASI_CRDFNode