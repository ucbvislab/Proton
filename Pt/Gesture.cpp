/*
 *  Gesture.cpp
 *  Proton
 *
 *  Created by Kenrick Kin on 2/24/12.
 *
 */

#include "Gesture.h"

#include "TouchSymbol.h"
#include "Utils.h"

double defaultConfidenceCalculator(const std::vector<PtTouchSymbols *> &touchStream)
{
    return 1.0;
}

void defaultReset()
{
    // do nothing
}

PtGesture::PtGesture(const PtTouchSymbols &gestureExpression)
{
    for(unsigned int i = 0; i < gestureExpression.size(); i++) {
        PtTouchSymbol *touchSymbol = new PtTouchSymbol(std::string(""));
        *touchSymbol = *(gestureExpression[i]);
        _gestureExpression.push_back(touchSymbol);
    }
    convertRegularExpressionToDFA(_gestureExpression, _dfa);
    _name = std::string("");
    _reset = defaultReset;
    _state = 1;
}


PtGesture::PtGesture(std::string symbolString, std::vector<std::pair<int, int> > triggers)
{
    convertStringToTouchSymbolRegularExpression(symbolString, std::string("_"), _gestureExpression);
    for(unsigned int i = 0; i < triggers.size(); i++) {
        std::pair<int,int> pair = triggers[i];
        addTriggerToIthTouchSymbol(&_gestureExpression, pair.first, pair.second);
    }
    convertRegularExpressionToDFA(_gestureExpression, _dfa);
    _name = std::string("");
    _reset = defaultReset;
    _state = 1;
}

PtGesture::~PtGesture()
{
    for(unsigned int i = 0; i < _gestureExpression.size(); i++) {
        PtTouchSymbol *touchSymbol = _gestureExpression[i];
        delete touchSymbol;
    }
}

int PtGesture::addCallback(fptrCallback callback)
{
    _callbacks.push_back(callback);
    _confidenceCalculators.push_back(defaultConfidenceCalculator);
    
    return _confidenceCalculators.size() - 1;
}

int PtGesture::addCallback(fptrCallback callback, fptrConfidence confidence)
{
    _callbacks.push_back(callback);
    _confidenceCalculators.push_back(confidence);
    
    return _confidenceCalculators.size() - 1;
}

double PtGesture::confidenceScore(int index, const std::vector<PtTouchSymbols *> &touchStream) const
{
    if(index >= 0 && (unsigned int)index < _confidenceCalculators.size())
        return _confidenceCalculators[index](touchStream);
        
    return 0;
}

    
const PtTouchSymbols* PtGesture::gestureExpression() const
{
    return &_gestureExpression;
}

PtTransitionTable* PtGesture::dfa()
{
    return &_dfa;
}

void PtGesture::executeCallback(int index, const std::vector<PtTouchSymbols *> &touchStream) const
{
    if(index >= 0 && (unsigned int)index < _callbacks.size())
        _callbacks[index](touchStream);
}

bool PtGesture::isCompleted() const
{
    return _dfa.isAcceptState(_state);
}


std::string PtGesture::name() const
{
    return _name;
}


bool PtGesture::processTouchSymbols(const PtTouchSymbols &touchSymbols, std::vector<int> *matchedTriggers)
{
    for(unsigned int j = 0; j < touchSymbols.size(); j++) {
        PtTouchSymbol *touchSymbol = (touchSymbols)[j];
        _state = _dfa.stateFromState(_state, touchSymbol->symbolString(), matchedTriggers);
        if(_state < 0)
            return false;
    }
    return true;
}

void PtGesture::reset()
{
    _state = 1;
    _reset();
}

void PtGesture::setName(std::string name)
{
    _name = name;
}

void PtGesture::setReset(fptrReset resetCallback)
{
    _reset = resetCallback;
}

