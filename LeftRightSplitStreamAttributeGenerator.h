/*
 *  LeftRightSplitStreamAttributeGenerator.h
 *  Proton
 *
 *  Created by Kenrick Kin on 7/26/12.
 *
 */

#ifndef LEFTRIGHTSPLITSTREAMATTRIBUTEGENERATOR_H
#define LEFTRIGHTSPLITSTREAMATTRIBUTEGENERATOR_H

#include "Pt/SplitStreamAttributeGenerator.h"

class LeftRightSplitStreamAttributeGenerator : public PtSplitStreamAttributeGenerator {
    virtual std::string attributeValue(const PtTouchSymbol &touchSymbol) const {
        const PtTuioTouchSymbol *tS = static_cast<const PtTuioTouchSymbol*>(&touchSymbol);
        if(tS->tuioCursor()->getX() < 0.5)
            return std::string("L");
        else
            return std::string("R");
    }
    
    virtual int gestureMatcherForAttributeValue(std::string attributeValue) const {
        // left ("L") value goes to the gesture matcher at index 0
        if(attributeValue.compare(std::string("L")) == 0)
            return 0;
        // right ("R") value goes to the gesture matcher at index 1
        else
            return 1;
    }
};

#endif
