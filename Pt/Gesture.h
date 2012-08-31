/*
 *  Gesture.h
 *  Proton
 *
 *  Created by Kenrick Kin on 2/24/12.
 *
 */

#ifndef PT_GESTURE_H
#define PT_GESTURE_H

#include "TouchSymbol.h"
#include "TransitionTable.h"

#include <string>
#include <vector>

typedef void (*fptrCallback)(const std::vector<PtTouchSymbols *> &touchStream);
typedef double (*fptrConfidence)(const std::vector<PtTouchSymbols *> &touchStream);
typedef void (*fptrReset)();

/// PtGesture represents a gesture as regular expression touch symbols specified by PtTouchSymbols.
/// PtGesture also manages the generated DFA used for gesture matching.
/// It also maintains a list of callbacks and associated confidence calculators indexed from 0.
/// PtGesture objects are added to a PtTouchProcessor which manages gesture matching via PtGestureMatchers
class PtGesture {

private:
    std::vector<fptrCallback>        _callbacks;
    std::vector<fptrConfidence>      _confidenceCalculators;
    PtTransitionTable                _dfa; // the DFA corresponding to the gesture regular expression
    PtTouchSymbols                   _gestureExpression; // the gesture regular expression
    std::string                      _name;
    fptrReset                        _reset;
    int                              _state; // state within the DFA the gesture is currently in, initial state is 1
    
public:
    /// creates a gesture using the provided regular expression.
    /// PtGesture stores a copy of the expression.
    PtGesture(const PtTouchSymbols &gestureExpression);
    PtGesture(std::string symbolString, std::vector<std::pair<int, int> > triggers);
    ~PtGesture();

    /// add a callback to the vector of callbacks (a default confidence calculator will be used)
    /// returns the index of the callback
    int addCallback(fptrCallback callback);
    
    /// add a callback to the vector of callbacks and confidence calculator to vector of confidence calculators
    /// returns the index of the callback
    int addCallback(fptrCallback callback, fptrConfidence confidence);

    /// returns confidence score for callback at index
    double confidenceScore(int index, const std::vector<PtTouchSymbols *> &touchStream) const;

    /// returns pointer to dfa
    PtTransitionTable* dfa();
    
    /// execute callback at index
    void executeCallback(int index, const std::vector<PtTouchSymbols *> &touchStream) const;

    /// returns pointer to gesture expression    
    const PtTouchSymbols* gestureExpression() const;

    /// returns whether gesture has reached the accept state in the dfa    
    bool isCompleted() const;

    /// returns gesture name
    std::string name() const;

    /// traverses the dfa given the symbols touchSymbols and stores triggers in matchedTriggers
    /// returns FALSE if dfa traversal fails
    bool processTouchSymbols(const PtTouchSymbols &touchSymbols, std::vector<int> *matchedTriggers);
    
    /// set state back to 1, executes reset callback
    void reset();
        
    /// setter for gesture name
    void setName(std::string name);

    /// sets reset callback for the gesture
    void setReset(fptrReset resetCallback);
};

#endif
