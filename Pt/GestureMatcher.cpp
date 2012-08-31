/*
 *  GestureMatcher.cpp
 *  Proton
 *
 *  Created by Kenrick Kin on 2/24/12.
 *
 */

#include "GestureMatcher.h"

#include "Gesture.h"

#include <iostream>

static bool _verbose = true;

PtGestureMatcher::PtGestureMatcher()
{
    _executeBestGesture = false;
    _clearIds();
}

PtGestureMatcher::~PtGestureMatcher()
{
    for(unsigned int i = 0; i < _attributeGenerators.size(); i++) {
        PtAttributeGenerator *attributeGenerator = _attributeGenerators[i];
        delete attributeGenerator;
    }
    for(unsigned int i = 0; i < _gestures.size(); i++) {
        PtGesture *gesture = _gestures[i];
        delete gesture;
    }
}

void PtGestureMatcher::addAttributeGenerator(PtAttributeGenerator *attributeGenerator)
{
    _attributeGenerators.push_back(attributeGenerator);
}

void PtGestureMatcher::addGesture(PtGesture *gesture)
{
    _gestures.push_back(gesture);
}

int PtGestureMatcher::assignTouchId(int hardwareTouchId)
{
    int unused = PT_MAX_TOUCHES;
    for(int i = 0; i < PT_MAX_TOUCHES; i++) {
        if(_touchIds[i] == -1) {
            unused = i;
            break;
        }
        //else if(_touchIds[i] == hardwareTouchId)
        //    return i;
    }
    if(unused < PT_MAX_TOUCHES) {
        _touchIds[unused] = hardwareTouchId;
        
        return unused;
    }
    return -1;
}

void PtGestureMatcher::_clearIds()
{
    for(int i = 0; i < PT_MAX_TOUCHES; i++) {
        _touchIds[i] = -1;
    }
}

void PtGestureMatcher::_processFirstFrame(PtTouchSymbols *touchSymbols)
{
    for(unsigned int i = 0; i < _gestures.size(); i++) {
        _gestures[i]->reset();
    }
    _activeGestures.insert(_activeGestures.begin(), _gestures.begin(), _gestures.end());
    _processFrame(touchSymbols);
}

void PtGestureMatcher::_processFrame(PtTouchSymbols *touchSymbols)
{
    PtGesture *bestGesture = NULL;
    int bestCallbackIndex = -1;
    double bestScore = 0;

    for(int i = 0; i < (int)_activeGestures.size(); i++) {
        PtGesture *gesture = _activeGestures[i];
        if(_verbose)
            printf("gesture name: %s\n", gesture->name().c_str());
        std::vector<int> matchedTriggers;
        if(gesture->processTouchSymbols(*touchSymbols, &matchedTriggers)) {
        } else {
            //delete gesture; // TODO: double check if gesture should be deleted, methinks not
            _activeGestures.erase(_activeGestures.begin()+i);
            i--;
        }
        
        if(_executeBestGesture) { // save best gesture so far
            for(unsigned int j = 0; j < matchedTriggers.size(); j++) {
                if(_verbose)
                    printf("trig: %d\n", matchedTriggers[j]);
                double score = gesture->confidenceScore(matchedTriggers[j], _stream);
                if(score > bestScore) {
                    bestGesture = gesture;
                    bestCallbackIndex = matchedTriggers[j];
                    bestScore = score;
                }
            }
        } else { // just execute every callback
            for(unsigned int j = 0; j < matchedTriggers.size(); j++) {
                if(_verbose)
                    printf("trig: %d\n", matchedTriggers[j]);
                gesture->executeCallback(matchedTriggers[j], _stream);
            }
        }
    }
    
    if(_executeBestGesture) {
        if(bestGesture)
            bestGesture->executeCallback(bestCallbackIndex, _stream);
    }
    
    for(int i = 0; i < (int)_activeGestures.size(); i++) {
        PtGesture *gesture = _activeGestures[i];
        if(gesture->isCompleted()) {
            _activeGestures.erase(_activeGestures.begin()+i);
            i--;
        }
    }
    
    //TODO: check if gestures are in accept states... or is this done.
}

void PtGestureMatcher::processTouchSymbols(PtTouchSymbols *touchSymbols)
{
    // 1. add the new touch symbols to the saved touch stream
    _stream.push_back(touchSymbols);

    // 2. find and append attribute values for each new touch symbol
    for(unsigned int i = 0; i < touchSymbols->size(); i++) {
        PtTouchSymbol *touchSymbol = (*touchSymbols)[i];
        for(unsigned int j = 0; j < _attributeGenerators.size(); j++) {
            std::string attributeValue = _attributeGenerators[j]->attributeValue(*touchSymbol, &_stream);
            touchSymbol->addAttributeValue(attributeValue);
        }
        if(_verbose)
            std::cout << touchSymbol->symbolString() << std::endl;
    }

    // 3. process new touch symbols
    if(_stream.size() == 1) {
        _processFirstFrame(touchSymbols);
    } else
        _processFrame(touchSymbols);
    
    // 4. clears finished touch id on touch up
    for(unsigned int i = 0; i < touchSymbols->size(); i++) {
        PtTouchSymbol *touchSymbol = (*touchSymbols)[i];
        if(touchSymbol->touchAction() == PT_UP && touchSymbol->touchId() >= 0 && touchSymbol->touchId() < PT_MAX_TOUCHES) {
            _touchIds[touchSymbol->touchId()] = -1;
        }
    }
        
    // 5. clear stream if no active gestures left
    if(_activeGestures.size() == 0) {
        //_clearIds(); problem with clearing Ids here is the gesture matcher should still
        // consume the touches even if no valid gestures left
        for(unsigned int i = 0; i < _stream.size(); i++) {
            clearRegexp(_stream[i]);
            delete _stream[i];
        }
        _stream.clear();
    }
}

void PtGestureMatcher::setExecuteBestGesture(bool executeBest)
{
    _executeBestGesture = executeBest;
}

int PtGestureMatcher::touchId(int hardwareTouchId) const
{
    int unused = PT_MAX_TOUCHES;
    for(int i = 0; i < PT_MAX_TOUCHES; i++) {
        if(_touchIds[i] == -1 && i < unused)
            unused = i;
        else if(_touchIds[i] == hardwareTouchId)
            return i;
    }
    //if(unused < 10) {
    //    _touchIds[unused] == hardwareTouchId;
    //    return unused;
    //}
    return -1;
}
