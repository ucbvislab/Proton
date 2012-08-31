/*
 *  HitTestAttributeGenerator.h
 *  Proton
 *
 *  Created by Kenrick Kin on 6/21/12.
 *
 */

#ifndef HITTESTATTRIBUTEGENERATOR_H
#define HITTESTATTRIBUTEGENERATOR_H

#include "Pt/AttributeGenerator.h"

#include "Pt/TuioTouchSymbol.h"

#include "Scene.h"

/// This attibute generator requires that a scene object that provides hit-testing of the a touch position
class HitTestAttributeGenerator : public PtAttributeGenerator {
    Scene           *_scene;

public:
    HitTestAttributeGenerator(Scene *scene) {
        _scene = scene;
    }

    std::string attributeValue(const PtTouchSymbol &touchSymbol, std::vector<PtTouchSymbols *> *touchStream) const {
        const PtTuioTouchSymbol *tS = static_cast<const PtTuioTouchSymbol*>(&touchSymbol);
        double x = tS->tuioCursor()->getX();
        double y = tS->tuioCursor()->getY();
        
        // query the object type hit by the touch and return a string representation of the objeccct
        SceneObjectType objectType = _scene->hitTest(x,y);
        switch (objectType) {
            case SOT_CIRCLE:
                return std::string("C");
                break;
            case SOT_SQUARE:
                return std::string("S");
                break;
            default:
                return std::string("O");
                break;
        }
    }
};

#endif
