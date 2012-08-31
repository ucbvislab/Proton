/*
 *  GestureMatcher.h
 *  Proton
 *
 *  Created by Kenrick Kin on 2/24/12.
 *
 */

#ifndef PT_GESTUREMATCHER_H
#define PT_GESTUREMATCHER_H

#include <string>
#include <vector>

#include "AttributeGenerator.h"
#include "TouchSymbol.h"

#define PT_MAX_TOUCHES 10

class PtGesture;

/// PtGestureMatcher maintains a gesture set comprised of PtGestures as well as a set of
/// PtAttributeGenerators that generate attribute values for each PtTouchSymbol being matched
/// for the set. The developer need never directly interact with a PtGestureMatcher.
class PtGestureMatcher {

private:
    std::vector<PtGesture *>                    _activeGestures; // valid gestures based on user input
    std::vector<PtAttributeGenerator *>         _attributeGenerators; // various attribute generators
    bool                                        _executeBestGesture; // execute only the best gesture
    std::vector<PtGesture *>                    _gestures; // all gestures
    std::vector<PtTouchSymbols *>               _stream; // stream of current touch events
    int                                         _touchIds[PT_MAX_TOUCHES]; // maps hardware touch id to gesture matcher touch id
    
public:
    PtGestureMatcher();
    ~PtGestureMatcher();

    /// Add attributeGenerator to generator list
    /// PtGestureMatcher is responsible for deleting attributeGenerator    
    void addAttributeGenerator(PtAttributeGenerator *attributeGenerator);

    /// Add gesture to gesture set
    /// PtGestureMatcher is responsible for deleting gesture
    void addGesture(PtGesture *gesture);
    
    /// assigns a mapped touch id to the hardware touch id and returns the new id
    int assignTouchId(int hardwareTouchId);

    /// Traverse the DFAs for each gesture that is still valid given touchSymbols.
    /// Removes invalid gestures from consideration    
    void processTouchSymbols(PtTouchSymbols *touchSymbols);
    
    /// If "executeBest" is true, only the best matched gesture's callback will be executed
    /// If "executeBest" is false, all matched gestures' callbacks will be executed
    void setExecuteBestGesture(bool executeBest);
    
    /// returns the gesture matcher touch id for the given hardware touch id
    /// returns -1 if not found
    int touchId(int hardwareTouchId) const;

private:
    void _clearIds();
    void _processFirstFrame(PtTouchSymbols *touchSymbols);
    void _processFrame(PtTouchSymbols *touchSymbols);    
};

#endif
