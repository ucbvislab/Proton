/*
 *  AttributeGenerator.h
 *  Proton
 *
 *  Created by Kenrick Kin on 6/21/12.
 *
 */
 
#ifndef PT_ATTRIBUTEGENERATOR_H
#define PT_ATTRIBUTEGENERATOR_H
 
#include "TouchSymbol.h"

/// PtAttributeGenerator is an abstract class. To create a custom attribute generator, subclass
/// PtAttributeGenerator and define the attributeValue() method to return the desired attribute value
/// for a given touch event symbol.
class PtAttributeGenerator {
    std::string _name;
    
public:
    virtual ~PtAttributeGenerator() {}

    /// subclass should implement this method to return the attribute value (as a std::string) for the given "touchSymbol"
    /// "touchStream" is a vector of PtTouchSymbols, which is a PtTouchSymbol vector
    virtual std::string attributeValue(const PtTouchSymbol &touchSymbol, std::vector<PtTouchSymbols *> *touchStream) const = 0;
    
    /// returns name of attribute
    std::string name() const {return _name;}

    /// set attribute name    
    void setName(std::string name) {_name = name;}
};

#endif
