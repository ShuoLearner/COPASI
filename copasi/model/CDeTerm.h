#ifndef CDETERM_H
#define CDETERM_H

#include "CSpecLine.h"

/**
 * This class describes a term in the differential equation.
 */
class CDeTerm
{
 public:
    /** 
     * The various types of token
     */
    enum Type{NL, PLUS, MINUS, LPAREN, RPAREN, EXPONENT, MULT, DIV, IDENT, NUM};
    /**
     * The constructor
     */
    CDeTerm();
    /** 
     * The destructor
     */
    ~CDeTerm();
    /**
     * Set the rate constant
     */
    void setRateConstant(string rate) {mRateConstant = rate;}
    /**
     * Retrieve the rate constant
     */
    string getRateConstant() {return mRateConstant;}
    /**
     * Set the multiplier
     */
    void setMultiplier(C_FLOAT64 multiplier) {mMultiplier = multiplier;}
    /**
     * Get the multiplier
     */
    C_FLOAT64 getMultiplier() {return mMultiplier;}
    /**
     * Set the sign
     */
    void setSign(char sign);
    /**
     * Retrieve the sign
     */
    C_INT32 getSign() {return mSign;}
    /**
     * Add an element to the vector of tokens describing the rate
     */
    void addElement(Type type, string token);
    /**
     * Remove an element from the stack
     */
    void deleteElement(C_INT32 index);
    /**
     * Return the size of the token stack
     */
    C_INT32 size() {return mTokenStack.size();}
    /**
     * Access the stack at a given position
     */
    string operator[](unsigned C_INT32 index);
    /**
     * Return the full description string. This will have been modified by the end.
     */
    string getDescription();
    /**
     * Do things like strip out the multiplier and determine the rate constant
     */
    void compile(vector<CNameVal> &rates);
 private:
    /**
     * The stack of tokens making up the term
     */
    vector<pair<Type,string>* > mTokenStack;
    /**
     * The sign
     */
    C_INT32 mSign;
    /**
     * The multiplier
     */
    C_FLOAT64 mMultiplier;
    /**
     * The rate constant
     */
    string mRateConstant;
};

#endif // CDETERM_H
