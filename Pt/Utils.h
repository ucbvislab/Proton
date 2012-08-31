/*
 *  Utils.h
 *  Proton
 *
 *  Created by Kenrick Kin on 2/4/12.
 *
 */

#ifndef PT_UTILS_H
#define PT_UTILS_H

#include <string>
#include <vector>

#include "TouchSymbol.h"
#include "TransitionTable.h"
    
/// Returns 1 for success, 0 for failure.
/// Corresponding NFA is stored in "transitionTable."
int convertRegularExpressionToNFA(const PtTouchSymbols &regexp, PtTransitionTable &transitionTable);

/// Returns 1 for success, 0 for failure.
/// Corresponding DFA is stored in "transitionTable."
int convertRegularExpressionToDFA(const PtTouchSymbols &regexp, PtTransitionTable &transitionTable);

///  Returns 1 for succes, 0 for failure.
/// Converts "stringRegexp," a string of touch symbol strings, separated by "delimiter,"
/// and stores them in "regularExpression," which must initially be empty.
/// The caller is responsible for freeing the dynamically allocated TouchSymbol objects stored
/// in "regularExpression." Note: this is bad design, hopefully I'll think of a better solution.
/// I've been using underscore "_" as the delimiter.
/// For example "D1_M1_U1" will return the vector {D1, M1, U1}.
int convertStringToTouchSymbolRegularExpression(const std::string stringRegexp, const std::string delimiter, PtTouchSymbols &regularExpression);

#endif
