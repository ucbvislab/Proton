/*
 *  PtTouchProcessor.h
 *  Proton
 *
 *  Created by Kenrick Kin on 2/24/12.
 *
 */

#ifndef PT_TOUCHPROCESSOR_H
#define PT_TOUCHPROCESSOR_H

#include "GestureMatcher.h"

class PtSplitStreamAttributeGenerator;

/// The PtTouchProcessor manages one or more PtGestureMatchers. By default there is only one PtGestureMatcher
/// and PtTouchProcessor receives PtTouchSymbols and sends them to the PtGestureMatcher for gesture matching.
/// In split stream scenarios in which the developer defines a PtSplitStreamAttributeGenerator, the
/// PtTouchProcessor manages one PtGestureMatcher per stream and sends PtTouchSymbol to the PtGestureMatcher
/// corresponding to the split stream attribute value. 
class PtTouchProcessor
{
private:
    PtGestureMatcher                    _gestureMatcher; // currently just a single gesture matcher
    std::vector<PtGestureMatcher*>      _gestureMatchers;
    PtSplitStreamAttributeGenerator*    _splitStreamAttributeGenerator;

public:
    /// by default, touch processor maintains only a single gesture matcher
    PtTouchProcessor();
    PtTouchProcessor(unsigned int numMatchers);
    ~PtTouchProcessor();

    /// adds an attributeGenerator to _gestureMatcher, which becomes owner of "attributeGenerator"
    /// PtGestureMatcher is responsible for freeing "gesture"    
    void addAttributeGenerator(PtAttributeGenerator *attributeGenerator, unsigned int index = 0);
    
    /// adds gesture to _gestureMatcher, which becomes owner of "gesture"
    /// PtGestureMatcher is responsible for freeing "gesture"
    void addGesture(PtGesture *gesture, unsigned int index = 0);
    
    /// passes TouchSymbol vector to _gestureMatcher to be processed
    void processTouchSymbols(PtTouchSymbols *touchSymbols);
    
    /// executes the callback with the highest confidence score
    void setExecuteBestGesture(bool executeBest, unsigned int index = 0);

    /// splits stream by provided PtSplitStreamAttributeGenerator
    /// PtTouchProcessor becomes owner of "attributeGenerator"
    /// PtTouchProcessor is reponsible for freeing "attributeGenerator"
    void setSplitStreamAttributeGenerator(PtSplitStreamAttributeGenerator *attributeGenerator);
};

#endif
