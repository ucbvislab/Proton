/*
 *  TouchSymbol.h
 *  Proton
 *
 *  Created by Kenrick Kin on 2/3/12.
 *
 */

#ifndef PT_TOUCHSYMBOL_H
#define PT_TOUCHSYMBOL_H

#include <string>
#include <vector>

enum PtTouchAction {
    PT_DOWN,
    PT_MOVE,
    PT_UP,
    PT_OTHER,
};

/// PtTouchSymbol represents a touch event symbol. It contains the symbol string consisting of a touch action,
/// touch id, and a list of attribute values. For example, D0:Foo translates to touch DOWN with id 0 and attribute value "Foo".
/// When using a PtTouchSymbol to describe a gesture regular expression, the developer can add a trigger to the PtTouchSymbol.
/// The trigger is an index to the gesture's callback list. The callback should be executed when the PtTouchSymbol is matched.

class PtTouchSymbol {

private:
    std::vector<std::string> _attributeValues;
    std::string              _symbolString; // string that represents the touch symbol
    std::vector<int>         _triggers;     // list of trigger indices for the touch symbol, typically contains 0 or 1 trigger index
    
    //static int              _numAttributeValues; // intended for keeping track of max number of attribute values for each symbol
    
public:
    PtTouchSymbol(std::string symbolString);
    PtTouchSymbol(std::string symbolString, std::vector<int>);
    virtual ~PtTouchSymbol() {}
    
    /// appends "attributeValue" to end of touch symbol
    void addAttributeValue(std::string attributeValue);
    
    /// adds a trigger (index of trigger) to touch symbol
    void addTrigger(int trigger);
    
    /// adds a vector of triggers (indices of triggers) to the touch symbol
    void addTriggers(std::vector<int> triggers);
    
    /// returns number of attributes associated with touch symbol
    int attributeValueCount() const;
    
    /// returns attribute value at index i
    std::string attributeValueAtIndex(int i) const;
    
    /// compares "str" to entire touch symbol string with attribute values
    bool isEqualToString(std::string str) const;
    
    /// sets touch id to "touchId" should probably only be used by stream generator
    void setTouchId(int touchId);
    
    /// returns touch symbol string with attribute values
    std::string symbolString() const;
    
    /// returns the touch symbol's touch action
    PtTouchAction touchAction() const;

    /// returns the touch symbol's touch id
    int touchId() const;
    
    /// returns array of triggers
    std::vector<int> triggers() const;
    
private:
    void _setSymbolString(std::string symbolString);
};

/// vector of PtTouchSymbol, also used to represent a regular expression
typedef std::vector<PtTouchSymbol *> PtTouchSymbols;

/// adds trigger index to ith touch symbol in regularExpression
/// The symbols are numbered starting from 0
void addTriggerToIthTouchSymbol(PtTouchSymbols *regularExpression, int trigger, int i);

/// deletes each touch symbol and clears the expression
void clearRegexp(PtTouchSymbols *regularExpression);

#endif
