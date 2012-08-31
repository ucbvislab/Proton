/*
 *  TransitionTable.h
 *  Proton
 *
 *  Created by Kenrick Kin on 2/3/12.
 *
 */

#ifndef PT_TRANSITIONTABLE_H
#define PT_TRANSITIONTABLE_H

#include <map>
#include <string>
#include <vector>
#include <set>

#include "TouchSymbol.h"

typedef std::set<int> PtStateSet;
typedef std::vector<PtStateSet> PtStateSetsVector;
typedef std::map<std::string, int> PtStateSetIdMap;

typedef std::map<int, std::vector<PtTouchSymbol *> *> PtStateTransitionsMap;
typedef std::map<int, PtStateTransitionsMap *> PtStateMapMap;

/// PtTransitionTable is used to represent the DFA (and NFA) of the gesture regular expressions
class PtTransitionTable {

private:
    PtStateMapMap   _transitionMap;
    int             _startState; // typically start state is numbered 1

public:
    PtTransitionTable();
    ~PtTransitionTable();    
    void addTransition(PtTouchSymbol *touchSymbol, int stateOne, int stateTwo);
    int convertToDFA(PtTransitionTable &transitionTable);
    bool isAcceptState(int state) const;
    void printDotFormatToFile(std::string filename); // use www.graphviz.org to display .dot file format
                                                     // http://en.wikipedia.org/wiki/DOT_language
    void removeEmptyTransitions();
    void removeUnreachableStatesFromState(int state);
    int stateFromState(int state, std::string transition, std::vector<int> *matchedTriggers); // adds triggers to matchedTriggers array;
    std::vector<PtTouchSymbol *>* transitions(int stateOne, int stateTwo);
};

#endif
