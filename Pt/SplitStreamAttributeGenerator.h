/*
 *  SplitStreamAttributeGenerator.h
 *  Proton
 *
 *  Created by Kenrick Kin on 7/25/12.
 *
 */

#ifndef PT_SPLITSTREAMATTRIBUTEGENERATOR_H
#define PT_SPLITSTREAMATTRIBUTEGENERATOR_H

/// PtSplitStreamAttributeGenerator is an abstract class. To create a custom split stream attribute generator
/// subclass PtSplitStreamAttributeGenerator, define the attributeValue() method to return the desired
/// attribute value that assigns the given touch event symbol to one of the touch event streams. In addition,
/// define gestureMatcherForAttributeValue() that returns the index of the stream that corresponds to a given
/// attribute value. Unlike a PtAttributeGenerator, the PtSplitStreamAttributeGenerator is assigned to
/// the PtTouchProcessor and not to a PtGestureMatcher.
class PtSplitStreamAttributeGenerator : public PtAttributeGenerator {

public:
    std::string attributeValue(const PtTouchSymbol &touchSymbol, std::vector<PtTouchSymbols *> *touchStream) const {
        return attributeValue(touchSymbol);
    }
    
    /// return the attribute value for the touch event symbol (should only occur a touch down actions)
    virtual std::string attributeValue(const PtTouchSymbol &touchSymbol) const = 0;

    /// return the index of stream that corresponds to the attribute value
    virtual int gestureMatcherForAttributeValue(std::string attributeValue) const = 0;
};

#endif
