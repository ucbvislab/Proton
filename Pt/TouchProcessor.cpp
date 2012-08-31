/*
 *  PtTouchProcessor.cpp
 *  Proton
 *
 *  Created by Kenrick Kin on 2/24/12.
 *
 */

#include "TouchProcessor.h"

#include "SplitStreamAttributeGenerator.h"

#include <iostream>

PtTouchProcessor::PtTouchProcessor()
{
    PtGestureMatcher *matcher = new PtGestureMatcher();
    _gestureMatchers.push_back(matcher);
    _splitStreamAttributeGenerator = NULL;
}

PtTouchProcessor::PtTouchProcessor(unsigned int numMatchers)
{
    if(numMatchers < 1) numMatchers = 1;
    for(unsigned int i = 0; i < numMatchers; i++) {
        PtGestureMatcher *matcher = new PtGestureMatcher();
        _gestureMatchers.push_back(matcher);
    }
}

PtTouchProcessor::~PtTouchProcessor()
{
    for(unsigned int i = 0; i < _gestureMatchers.size(); i++) {
        delete _gestureMatchers[i];
    }
    delete _splitStreamAttributeGenerator;
}

void PtTouchProcessor::addAttributeGenerator(PtAttributeGenerator *attributeGenerator, unsigned int index)
{
    //_gestureMatcher.addAttributeGenerator(attributeGenerator);
    if(index < _gestureMatchers.size())
        _gestureMatchers[index]->addAttributeGenerator(attributeGenerator);
}

void PtTouchProcessor::addGesture(PtGesture *gesture, unsigned int index)
{
    //_gestureMatcher.addGesture(gesture);
    if(index < _gestureMatchers.size())
        _gestureMatchers[index]->addGesture(gesture);
}

void PtTouchProcessor::processTouchSymbols(PtTouchSymbols *touchSymbols)
{
    // 1. create a vector of PtTouchSymbols, one for each gesture matcher
    std::vector<PtTouchSymbols*> touchSymbolVectors;
    for(unsigned int i = 0; i < _gestureMatchers.size(); i++) {
        touchSymbolVectors.push_back(new PtTouchSymbols());
    }
    
    // 2a. if there's no split stream generator, just send all touches to the first gesture matcher
    if(_splitStreamAttributeGenerator == NULL) {
        //_gestureMatchers[0].processTouchSymbols(touchSymbols);
        for(unsigned int i = 1; i < touchSymbolVectors.size(); i++) {
            delete touchSymbolVectors[i];
        }
        for(unsigned int i = 0; i < touchSymbols->size(); i++) {
            PtTouchSymbol *touchSymbol = (*touchSymbols)[i];
            if(touchSymbol->touchAction() == PT_DOWN) {
                int remappedTouchId = _gestureMatchers[0]->assignTouchId(touchSymbol->touchId());
                touchSymbol->setTouchId(remappedTouchId);
                touchSymbolVectors[0]->push_back(touchSymbol);
            } else {
                int remappedTouchId = _gestureMatchers[0]->touchId(touchSymbol->touchId());
                if(remappedTouchId >= 0) {
                    touchSymbol->setTouchId(remappedTouchId);
                    touchSymbolVectors[0]->push_back(touchSymbol);
                } else {
                    // TODO: delete touch symbol?
                    delete touchSymbol;
                }
            }
        }
        
        if(touchSymbolVectors[0]->size() > 0) {
            _gestureMatchers[0]->processTouchSymbols(touchSymbolVectors[0]);
        } else {
            delete touchSymbolVectors[0];
        }
    // 2b. otherwise add each touch symbol to the proper PtTouchSymbols
    } else {
        for(unsigned int i = 0; i < touchSymbols->size(); i++) {
            PtTouchSymbol *touchSymbol = (*touchSymbols)[i];
            // 2b-1. if it's a touch down, calculate the split stream attribute value
            if(touchSymbol->touchAction() == PT_DOWN) {
                std::string attributeValue = _splitStreamAttributeGenerator->attributeValue(*touchSymbol);
                int index = _splitStreamAttributeGenerator->gestureMatcherForAttributeValue(attributeValue);
                if(index >= 0 && (unsigned int)index < touchSymbolVectors.size()) {
                    touchSymbolVectors[index]->push_back(touchSymbol);
                    int remappedTouchId = _gestureMatchers[index]->assignTouchId(touchSymbol->touchId());
                    touchSymbol->setTouchId(remappedTouchId);
                } else {
                    // TODO: delete touch symbol?
                    delete touchSymbol;
                }
            // 2b-2. otherwise add the touch symbol to gesture matcher that originally claimed it
            } else {
                bool foundOwner = false;
                for(unsigned int j = 0; j < _gestureMatchers.size(); j++) {
                    int remappedTouchId = _gestureMatchers[j]->touchId(touchSymbol->touchId());
                    if(remappedTouchId >= 0) {
                        touchSymbol->setTouchId(remappedTouchId);
                        touchSymbolVectors[j]->push_back(touchSymbol);
                        foundOwner = true;
                        break;
                    }
                }
                if(!foundOwner) {
                    // TODO: delete touch symbol?
                    delete touchSymbol;
                }
            }
        }
        
        // 2b-3. have each gesture matcher process its PtTouchSymbols
        for(unsigned int i = 0; i < touchSymbolVectors.size(); i++) {
            if(touchSymbolVectors[i]->size() > 0) {
                _gestureMatchers[i]->processTouchSymbols(touchSymbolVectors[i]);
            } else {
                delete touchSymbolVectors[i];
            }
        }
    }
    delete touchSymbols;

    //for(int i = 0; i < touchSymbols->size(); i++) {
    //    std::cout << (*touchSymbols)[i]->symbolString() << std::endl;
    //}
    //_gestureMatcher.processTouchSymbols(touchSymbols);
}

void PtTouchProcessor::setExecuteBestGesture(bool executeBest, unsigned int index)
{
    if(index < _gestureMatchers.size())
        _gestureMatchers[index]->setExecuteBestGesture(executeBest);
}

void PtTouchProcessor::setSplitStreamAttributeGenerator(PtSplitStreamAttributeGenerator *attributeGenerator)
{
    _splitStreamAttributeGenerator = attributeGenerator;
}
