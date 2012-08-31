/*
 *  TouchSymbol.cpp
 *  Proton
 *
 *  Created by Kenrick Kin on 2/3/12.
 *
 */

#include "TouchSymbol.h"

#include "stdio.h"
#include "assert.h"
#include <algorithm>
#include <iostream>
#include <sstream>

PtTouchSymbol::PtTouchSymbol(std::string symbolString)
{
    //_symbolString = symbolString;
    this->_setSymbolString(symbolString);
}

PtTouchSymbol::PtTouchSymbol(std::string symbolString, std::vector<int> triggers)
{
    //_symbolString = symbolString;
    this->_setSymbolString(symbolString);
    _triggers = triggers;
}

void PtTouchSymbol::addAttributeValue(std::string attributeValue)
{
    _attributeValues.push_back(attributeValue);
}

void PtTouchSymbol::addTrigger(int trigger)
{
    if(std::find(_triggers.begin(), _triggers.end(), trigger) == _triggers.end())
        _triggers.push_back(trigger);
    std::sort(_triggers.begin(), _triggers.end());
}

void PtTouchSymbol::addTriggers(std::vector<int> triggers)
{
    for(unsigned int i = 0; i < triggers.size(); i++) {
        int trigger = triggers[i];
        if(std::find(_triggers.begin(), _triggers.end(), trigger) == _triggers.end())
            _triggers.push_back(trigger);
    }
    std::sort(_triggers.begin(), _triggers.end());
}

int PtTouchSymbol::attributeValueCount() const
{
    return _attributeValues.size();
}

std::string PtTouchSymbol::attributeValueAtIndex(int i) const
{
    assert(i >= 0 && (unsigned int)i < _attributeValues.size());
    return _attributeValues[i];
}

bool PtTouchSymbol::isEqualToString(std::string str) const
{
    return !this->symbolString().compare(str);
//    return !_symbolString.compare(str);
}

void PtTouchSymbol::_setSymbolString(std::string symbolString)
{
    bool actionId = false;
    int i = 0;
    while(symbolString.size() - i > 0) {
        std::string subSymbolString;
        size_t found = symbolString.find(std::string(":"), i);
        if(found == std::string::npos) {
            subSymbolString = symbolString.substr(i, symbolString.size()-i);
            i = symbolString.size();
        } else {
            subSymbolString = symbolString.substr(i, found-i);
            i = found + 1; // + delimiter length, which is 1 for ":"
        }
                
        if(!actionId) {
            _symbolString = subSymbolString;
        
            /*char action = subSymbolString[0];
            if(action == 'D')
                _action = PT_DOWN;
            else if(action == 'M')
                _action = PT_MOVE;
            else if(action == 'U')
                _action = PT_UP;
            else
                _action = PT_OTHER;
                
            _id = atoi(subSymbolString.substr(1,subSymbolString.size()-1).c_str());*/
            
            actionId = true;
        } else {
            this->addAttributeValue(subSymbolString);
        }
    }
}

void PtTouchSymbol::setTouchId(int touchId)
{
    char buff[20];
    sprintf(buff, "%s%d", _symbolString.substr(0,1).c_str(), touchId);
    _symbolString = buff;
}

std::string PtTouchSymbol::symbolString() const
{
    std::string symbolString;
    /*switch(_action) {
        case PT_DOWN:
            symbolString = std::string("D");
            break;
        case PT_MOVE:
            symbolString = std::string("M");
            break;
        case PT_UP:
            symbolString = std::string("U");
            break;
        default:
            break;
    }

    std::stringstream ss;
    ss << _id;
    symbolString.append(ss.str());*/
    
    symbolString = _symbolString;

    for(unsigned int i = 0; i < _attributeValues.size(); i++) {
        symbolString.append(":");
        symbolString.append(_attributeValues[i]);
    }
    return symbolString;
    //return _symbolString;
}

PtTouchAction PtTouchSymbol::touchAction() const
{
    if(_symbolString.size() < 2) return PT_OTHER;
    char action = _symbolString[0];
    if(action == 'D')
        return PT_DOWN;
    else if(action == 'M')
        return PT_MOVE;
    else if(action == 'U')
        return PT_UP;
    else
        return PT_OTHER;
}

int PtTouchSymbol::touchId() const
{
    if(_symbolString.size() < 2) return -1;
    else return atoi(_symbolString.substr(1, _symbolString.size()-1).c_str());
}

std::vector<int> PtTouchSymbol::triggers() const
{
    return _triggers;
}


void addTriggerToIthTouchSymbol(PtTouchSymbols *regularExpression, int trigger, int i)
{
    int count = 0;
    for(unsigned int j = 0; j < regularExpression->size(); j++) {
        PtTouchSymbol *touchSymbol = (*regularExpression)[j];
        if(touchSymbol->symbolString().compare(std::string("|")) == 0 || touchSymbol->symbolString().compare(std::string("(")) == 0 ||
           touchSymbol->symbolString().compare(std::string(")")) == 0 || touchSymbol->symbolString().compare(std::string("*")) == 0) {
            continue;
        }
        if(count == i) {
            touchSymbol->addTrigger(trigger);
            return;
        } else {
            count++;
        }

    }
}

void clearRegexp(PtTouchSymbols *regularExpression)
{
    for(unsigned int i = 0; i < regularExpression->size(); i++) {
        PtTouchSymbol *ts = (*regularExpression)[i];
        delete ts;
    }
    regularExpression->clear();
}
