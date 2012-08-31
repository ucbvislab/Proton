/*
 *  TransitionTable.cpp
 *  Proton
 *
 *  Created by Kenrick Kin on 2/3/12.
 *
 */

#include "TransitionTable.h"

#include <algorithm>
#include <iostream>
#include <sstream>


PtTransitionTable::PtTransitionTable()
{
    _startState = 1;
}

PtTransitionTable::~PtTransitionTable()
{
    PtStateMapMap::iterator it;
    PtStateTransitionsMap::iterator jt;
    
    for(it = _transitionMap.begin(); it != _transitionMap.end(); it++) {
        PtStateTransitionsMap *stateTransitionMap = it->second;
        for(jt = stateTransitionMap->begin(); jt != stateTransitionMap->end(); jt++) {
            std::vector<PtTouchSymbol *> *touchVector = jt->second;
            for(unsigned int i = 0; i < touchVector->size(); i++) {
                PtTouchSymbol *touchSymbol = (*touchVector)[i];
                if(touchSymbol) {
                    delete touchSymbol;
                }
            }
            delete jt->second;
        }
        delete stateTransitionMap;
    }
}

void PtTransitionTable::addTransition(PtTouchSymbol *touchSymbol, int stateOne, int stateTwo)
{
    PtStateTransitionsMap *mapOne = _transitionMap[stateOne];
    if(!mapOne) {
        mapOne = new PtStateTransitionsMap();
        _transitionMap[stateOne] = mapOne;
    }
    PtStateTransitionsMap *mapTwo = _transitionMap[stateTwo];
    if(!mapTwo) {
        mapTwo = new PtStateTransitionsMap();
        _transitionMap[stateTwo] = mapTwo;
    }

    std::vector<PtTouchSymbol *> *transitions = NULL;
    if(mapOne->count(stateTwo) == 0) {
        transitions = new std::vector<PtTouchSymbol *>();
        (*mapOne)[stateTwo] = transitions;
    } else
        transitions = (*mapOne)[stateTwo];
    
    //std::vector<PtTouchSymbol *> *transitions = (*mapOne)[stateTwo];
    if(std::find(transitions->begin(), transitions->end(), touchSymbol) == transitions->end()) {
        transitions->push_back(touchSymbol);
    }
}

std::string stateSetToString(PtStateSet stateSet)
{
    if(stateSet.size() == 0) return std::string("");
    
    //std::string keyString("%d", *(stateSet.begin()));
    std::ostringstream str;
    str << *(stateSet.begin());
    PtStateSet::iterator it = stateSet.begin();
    it++;
    for(; it != stateSet.end(); it++) {
        str << "_" << (*it);
        //keyString.append(std::string("_%d", (*it)));
    }
    std::string keyString = str.str();
    
    return keyString;
}

int PtTransitionTable::convertToDFA(PtTransitionTable &dfa)
{
    PtStateSetIdMap dfaSetToStateMap;
    int dfaStateCount = 1;
    
    PtStateSetsVector stateQueue;
    PtStateSetsVector stateChecked;
    std::set<int> startStateSet;
    startStateSet.insert(_startState);
    stateQueue.push_back(startStateSet);
        
    dfaSetToStateMap[stateSetToString(startStateSet)] = dfaStateCount;
    dfaStateCount++;
    
    while(stateQueue.size() > 0) {
        PtStateSet stateSet = stateQueue[0];
        int dfaStateInt = dfaSetToStateMap[stateSetToString(stateSet)];
        
        std::map<std::string, std::set<int>* > transitionMap;
        std::map<std::string, PtTouchSymbol* > transitionMapForTouchSymbol;
        
        PtStateSet::iterator it;
        for(it = stateSet.begin(); it != stateSet.end(); it++) {
            int stateInt = (*it);
            PtStateTransitionsMap *stateMap = _transitionMap[stateInt];
            PtStateTransitionsMap::iterator jt;
            for(jt = stateMap->begin(); jt != stateMap->end(); jt++) {
                int nextStateInt = (*jt).first;
                std::vector<PtTouchSymbol *> *nextStateTransitions = (*jt).second;
                
                for(unsigned int i = 0; i < nextStateTransitions->size(); i++) {
                    PtTouchSymbol *transitionTouchSymbol = (*nextStateTransitions)[i];
                    PtTouchSymbol *touchSymbol = transitionMapForTouchSymbol[transitionTouchSymbol->symbolString()];
                    
                    if(transitionMap.count(transitionTouchSymbol->symbolString()) == 0) {
                        // deleted (supposedly) at end of while loop, clearing transitionMap.
                        std::set<int> *setset = new std::set<int>();
                        setset->insert(nextStateInt);
                        transitionMap[transitionTouchSymbol->symbolString()] = setset;
                        // deleted (supposedly) at end of while loop, clearing transitionMapForTouchSymbol.
                        PtTouchSymbol *newTouchSymbol = new PtTouchSymbol(transitionTouchSymbol->symbolString());
                        newTouchSymbol->addTriggers(transitionTouchSymbol->triggers());
                        transitionMapForTouchSymbol[transitionTouchSymbol->symbolString()] = newTouchSymbol;
                    } else {
                        std::set<int> *setset = transitionMap[touchSymbol->symbolString()];
                        setset->insert(nextStateInt);
                        if(transitionTouchSymbol->symbolString().compare(touchSymbol->symbolString()) == 0) {
                            touchSymbol->addTriggers(transitionTouchSymbol->triggers());
                        }
                    }
                }
            }
        }
        
        std::map<std::string, std::set<int>* >::iterator kt;
        for(kt = transitionMap.begin(); kt != transitionMap.end(); kt++) {
            std::set<int> *transitionMapSet = kt->second;
            PtTouchSymbol *transitionTouchSymbol = transitionMapForTouchSymbol[kt->first];
            
            bool newSet = true;
            for(unsigned int ii = 0; newSet && ii < stateQueue.size(); ii++) {
                if(*transitionMapSet == stateQueue[ii])
                    newSet = false;
            }
            for(unsigned int ii = 0; newSet && ii < stateChecked.size(); ii++) {
                if(*transitionMapSet == stateChecked[ii])
                    newSet = false;
            }
            if(newSet)
                stateQueue.push_back(*transitionMapSet);
                
            std::string transitionMapSetString = stateSetToString(*transitionMapSet);
            int transitionMapSetInt = -1;
            if(dfaSetToStateMap.count(transitionMapSetString) == 0) {
                transitionMapSetInt = dfaStateCount;
                dfaSetToStateMap[transitionMapSetString] = transitionMapSetInt;
                dfaStateCount++;
            } else
                transitionMapSetInt = dfaSetToStateMap[transitionMapSetString];
            
            // not deleted, stored in dfa so dfa's responbility
            PtTouchSymbol *storedTouchSymbol = new PtTouchSymbol(std::string(""));
            *storedTouchSymbol = *transitionTouchSymbol;
            //dfa->addTransition(transitionTouchSymbol, dfaStateInt, transitionMapSetInt);
            dfa.addTransition(storedTouchSymbol, dfaStateInt, transitionMapSetInt);
        }
        stateChecked.push_back(stateSet);
        stateQueue.erase(stateQueue.begin());
        
        // delete transitionMap contents
        std::map<std::string, std::set<int>* >::iterator tmIt;
        for(tmIt = transitionMap.begin(); tmIt != transitionMap.end(); tmIt++) {
            delete (*tmIt).second;
        }
        // delete transitionMapForTouchSymbol contents        
        std::map<std::string, PtTouchSymbol* >::iterator tmftsIt;
        for(tmftsIt = transitionMapForTouchSymbol.begin(); tmftsIt != transitionMapForTouchSymbol.end(); tmftsIt++) {
            delete (*tmftsIt).second;
        }
        
    }
    if(dfa._transitionMap.size() > 0)
        dfa._startState = 1;
        
    return 1;
    
    /*
    NSMutableDictionary *dfaSetToStateDictionary = [[NSMutableDictionary alloc] init];
    int dfaStateCount = 1;

    NSMutableArray *stateQueue = [[NSMutableArray alloc] init];
    NSMutableArray *stateChecked = [[NSMutableArray alloc] init];
    NSSet *startStateSet = [NSSet setWithObject: _startState];
    [stateQueue addObject: startStateSet];
    [dfaSetToStateDictionary setObject: [NSNumber numberWithInt: dfaStateCount] forKey: [MtReTransitionTable stateSetToString: startStateSet]];
    dfaStateCount++;
    
    // visit all states in queue (each state is a set of states from original nfa)
    while([stateQueue count] > 0) {
        NSSet *stateSet = [stateQueue objectAtIndex: 0];
        // get remapped dfa state number for set of nfa states
        NSNumber *dfaStateNumber = [dfaSetToStateDictionary objectForKey: [MtReTransitionTable stateSetToString: stateSet]];
        
        NSMutableDictionary *transitionDictionary = [[NSMutableDictionary alloc] init];
        NSMutableDictionary *transitionDictionaryForBoobyString = [[NSMutableDictionary alloc] init];
        
        // consider all transitions for all nfa states in the set
        NSEnumerator *enumerator = [stateSet objectEnumerator];
        NSNumber *stateNumber;
        
        while ((stateNumber = [enumerator nextObject])) {
            NSMutableDictionary *state = [_transitionDictionary objectForKey: stateNumber];
            NSArray *nextStates = [state allKeys];
            
            for(int i = 0; i < [nextStates count]; i++) {
                NSNumber *nextStateNumber = [nextStates objectAtIndex: i];
                NSArray *nextStateTransitions = [state objectForKey: nextStateNumber];
                for(int j = 0; j < [nextStateTransitions count]; j++) {
                    MtReBoobyString *transitionBoobyString = [nextStateTransitions objectAtIndex: j];
                    NSString *transitionString = [transitionBoobyString string];
                    //printf("%d %s\n", [nextStateNumber intValue], [transitionString UTF8String]);
                    NSMutableSet *set = [transitionDictionary objectForKey: transitionString];
                    MtReBoobyString *boobyString = [transitionDictionaryForBoobyString objectForKey: transitionString];
                    if(set) {
                        [set addObject: nextStateNumber];
                        if([boobyString isEqualToString: [transitionBoobyString string]])
                            [boobyString addGlobalTrigger: [transitionBoobyString triggerAtIndex: [transitionBoobyString length]-1]];
                    } else {
                        set = [NSMutableSet setWithObject: nextStateNumber];
                        [transitionDictionary setObject: set forKey: transitionString];
                        MtReBoobyString *newBoobyString = [MtReBoobyString boobyStringWithString: [transitionBoobyString string]];
                        [newBoobyString addGlobalTrigger: [transitionBoobyString triggerAtIndex: [transitionBoobyString length]-1]];
                        [transitionDictionaryForBoobyString setObject: newBoobyString forKey: transitionString];
                    }
                }
            }
        }
        
        // for current state, add transitions to subset states
        // if subset is new, add it to the dfaSetToStateDictionary
        NSEnumerator *transitionDictionaryEnumerator = [transitionDictionary keyEnumerator];
        NSString *transitionDictionaryKey;
        //NSString *transitionDictionaryKey;
        
        while ((transitionDictionaryKey = [transitionDictionaryEnumerator nextObject])) {
            NSSet *transitionDictionarySet = [transitionDictionary objectForKey: transitionDictionaryKey];
            MtReBoobyString *transitionBoobyString = [transitionDictionaryForBoobyString objectForKey: transitionDictionaryKey];
            
            BOOL newSet = true;
            for(int ii = 0; newSet && ii < [stateQueue count]; ii++) {
                if([transitionDictionarySet isEqualToSet: [stateQueue objectAtIndex: ii]])
                    newSet = false;
            }
            for(int ii = 0; newSet && ii < [stateChecked count]; ii++) {
                if([transitionDictionarySet isEqualToSet: [stateChecked objectAtIndex: ii]])
                    newSet = false;
            }
            if(newSet)
                [stateQueue addObject: transitionDictionarySet];
            
            NSString *transitionDictionarySetString = [MtReTransitionTable stateSetToString: transitionDictionarySet];
            NSNumber *transitionDictionarySetNumber = [dfaSetToStateDictionary objectForKey: transitionDictionarySetString];
            if(!transitionDictionarySetNumber) {
                transitionDictionarySetNumber = [NSNumber numberWithInt: dfaStateCount];
                [dfaSetToStateDictionary setObject: transitionDictionarySetNumber forKey:transitionDictionarySetString];
                dfaStateCount++;
            }
            //[dfa addTransition: transitionDictionaryKey fromState: [dfaStateNumber intValue] toState: [transitionDictionarySetNumber intValue]];
            [dfa addTransition: transitionBoobyString fromState: [dfaStateNumber intValue] toState: [transitionDictionarySetNumber intValue]];
        }
        [stateChecked addObject: stateSet];
        [stateQueue removeObjectAtIndex: 0];
        
        [transitionDictionary release];
    }
    if([dfa->_transitionDictionary count] > 0)
        dfa->_startState = [NSNumber numberWithInt: 1];
    */
}

bool PtTransitionTable::isAcceptState(int state) const
{
    //TODO: MAKE THIS MORE GENERAL, RIGHT NOW DEAD ENDS ARE ACCEPT STATES
    PtStateMapMap::const_iterator it = _transitionMap.find(state);
    if(it == _transitionMap.end())
        return false;
    else {
        if(it->second->size() == 0) return true;
        else return false;
    }

    //PtStateTransitionsMap *aMap = _transitionMap[state];
    //if(aMap) {
    //    if(aMap->size() == 0) return true;
    //}
    //return false;
}

void PtTransitionTable::printDotFormatToFile(std::string filename)
{
    FILE *file;
    file = fopen(filename.c_str(), "w");
    //NSArray *allKeys = [_transitionDictionary allKeys];
    fprintf(file, "digraph G {\n");
    
    PtStateMapMap::iterator it;
    for(it=_transitionMap.begin(); it != _transitionMap.end(); it++) {
        int state = (*it).first;
        PtStateTransitionsMap *stateMap = (*it).second;
        PtStateTransitionsMap::iterator iit;
        for(iit=stateMap->begin(); iit != stateMap->end(); iit++) {
            int nextState = (*iit).first;
            std::vector<PtTouchSymbol *> *transitions = (*iit).second;
            for(unsigned int i = 0; i < transitions->size(); i++) {
                PtTouchSymbol *touchSymbol = (*transitions)[i];
                std::string symbolString = touchSymbol->symbolString();
                fprintf(file, "\t%d -> %d [label=\"%s\"]\n", state, nextState, symbolString.c_str());
            }
        }
    }
    fprintf(file, "}\n");
    fclose(file);
}

void PtTransitionTable::removeEmptyTransitions()
{
    PtStateMapMap::iterator it;
    for(it=_transitionMap.begin(); it != _transitionMap.end(); it++) {
        int stateInt = (*it).first;
        PtStateTransitionsMap *stateMap = (*it).second;
        bool hasEmptyTransition = true;
        
        while(hasEmptyTransition) {
            PtStateTransitionsMap::iterator iit;
            for(iit=stateMap->begin(); iit != stateMap->end();) {
                int nextStateInt = (*iit).first;
                std::vector<PtTouchSymbol *> *transitions = (*iit).second;
                PtStateTransitionsMap *nextStateMap = _transitionMap[nextStateInt];
                PtStateTransitionsMap::iterator jjt;

                for(int i = 0; i < (int)transitions->size(); i++) {
                    PtTouchSymbol *touchSymbol = (*transitions)[i];

                    if(touchSymbol->isEqualToString(std::string("~"))) {
                        for(jjt=nextStateMap->begin(); jjt != nextStateMap->end(); jjt++) {
                            int nextNextStateInt = (*jjt).first;
                            std::vector<PtTouchSymbol *> *nextStateTransitions = (*jjt).second;
                            for(unsigned int k = 0; k < nextStateTransitions->size(); k++) {
                                PtTouchSymbol *pTSCopy = new PtTouchSymbol(std::string(""));
                                PtTouchSymbol *pTS = (*nextStateTransitions)[k];
                                *pTSCopy = *pTS;
                                addTransition(pTSCopy, stateInt, nextNextStateInt);
                            }
                        }
                        delete touchSymbol;
                        transitions->erase(transitions->begin()+i);
                        i--;
                    }

                }
                if(transitions->size() == 0) {
                    delete transitions;
                    stateMap->erase(iit++);
                } else
                    iit++;
            }
            
            hasEmptyTransition = false;
            for(iit=stateMap->begin(); iit != stateMap->end(); iit++) {
                std::vector<PtTouchSymbol *> *transitions = (*iit).second;
                for(unsigned int i = 0; i < transitions->size(); i++) {
                    PtTouchSymbol *touchSymbol = (*transitions)[i];
                    if(touchSymbol->isEqualToString(std::string("~"))) {
                        hasEmptyTransition = true;
                    }
                }
            }
        }
    }
    removeUnreachableStatesFromState(1);;
}

void reachableStatesFromState(int state, PtStateMapMap transitionMap, std::vector<int> *checklist)
{
    PtStateTransitionsMap *stateMap = transitionMap[state];
    PtStateTransitionsMap::iterator it;
    for(it=stateMap->begin(); it != stateMap->end(); it++) {
        int nextStateInt = (*it).first;
        if(std::find(checklist->begin(), checklist->end(), nextStateInt) == checklist->end()) {
            checklist->push_back(nextStateInt);
            reachableStatesFromState(nextStateInt, transitionMap, checklist);
        }
    }
}

void PtTransitionTable::removeUnreachableStatesFromState(int state)
{
    std::vector<int> reachableStates;
    reachableStates.push_back(state);
    reachableStatesFromState(state, _transitionMap, &reachableStates);
    
    PtStateMapMap::iterator it;
    for(it=_transitionMap.begin(); it != _transitionMap.end();) {
        int stateInt = it->first;
        PtStateTransitionsMap *stateTransitionMap = it->second;
        PtStateTransitionsMap::iterator jt;

        if(std::find(reachableStates.begin(), reachableStates.end(), stateInt) == reachableStates.end()) {
            // delete stateTransitionMap allocated memory
            for(jt = stateTransitionMap->begin(); jt != stateTransitionMap->end(); jt++) {
                std::vector<PtTouchSymbol *> *touchVector = jt->second;
                for(unsigned int i = 0; i < touchVector->size(); i++) {
                    PtTouchSymbol *touchSymbol = (*touchVector)[i];
                    if(touchSymbol) {
                        delete touchSymbol;
                    }
                }
                delete jt->second;
            }
            delete stateTransitionMap;
            _transitionMap.erase(it++);
        } else
            it++;
    }
}

int PtTransitionTable::stateFromState(int state, std::string transition, std::vector<int> *matchedTriggers)
{
    if(_transitionMap.count(state) == 0) return -1;
    PtStateTransitionsMap *stateMap = _transitionMap[state];
    PtStateTransitionsMap::iterator it;
    
    for(it = stateMap->begin(); it != stateMap->end(); it++) {
        std::vector<PtTouchSymbol *> *transitions = it->second;
        for(unsigned int i = 0; i < transitions->size(); i++) {
            PtTouchSymbol *touchSymbol = (*transitions)[i];
            if(transition.compare(touchSymbol->symbolString()) == 0) {
                std::vector<int> triggers = touchSymbol->triggers();
                matchedTriggers->insert(matchedTriggers->end(), triggers.begin(), triggers.end());
                return it->first;
            }
        }
    }
    return -1;
}    

std::vector<PtTouchSymbol *>* PtTransitionTable::transitions(int stateOne, int stateTwo)
{
    if(_transitionMap.count(stateOne)) {
        PtStateTransitionsMap *mapOne = _transitionMap[stateOne];
        if(mapOne->count(stateTwo))
            return (*mapOne)[stateTwo];
        return NULL;
    }
    return NULL;
}
