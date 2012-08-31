/*
 *  KKAttributeGenerator.h
 *  Proton
 *
 *  Created by Kenrick Kin on 8/13/12.
 *
 */

#ifndef KKATTRIBUTEGENERATOR_H
#define KKATTRIBUTEGENERATOR_H

#include "Pt/AttributeGenerator.h"

#include "Pt/TuioTouchSymbol.h"

class KKAttributeGenerator : public PtAttributeGenerator {
    /// returns "KK" as hit-target
    std::string attributeValue(const PtTouchSymbol &touchSymbol, std::vector<PtTouchSymbols *> *touchStream) const {
        return std::string("KK");
        
        // print touch locations at each frame
        /*for(int i = 0; i < touchStream->size(); i++) {
            PtTouchSymbols *frame = (*touchStream)[i];
            for(int j = 0; j < frame->size(); j++) {
                PtTuioTouchSymbol *tS = static_cast<PtTuioTouchSymbol*>((*frame)[j]);
                printf("frame: %d, touchSymbol: %s\n", i, tS->symbolString().c_str());
                printf("x: %f, y: %f\n", tS->tuioCursor()->getX(), tS->tuioCursor()->getY());
            }
        }*/
    }
};

#endif
