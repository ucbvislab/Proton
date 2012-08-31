/*
 *  Utils.cpp
 *  Proton
 *
 *  Created by Kenrick Kin on 2/4/12.
 *
 */

#include "Utils.h"

#include <sstream>

PtTouchSymbol *_MatchSymbol = new PtTouchSymbol(std::string("MATCH"));
PtTouchSymbol *_SplitSymbol = new PtTouchSymbol(std::string("SPLIT"));

/// The following code was modified from a regular expression implementation under the MIT License
/// BEGIN: modified regular expression implementation
/*
 * Regular expression implementation.
 * Supports only ( | ) * + ?.  No escapes.
 * Compiles to NFA and then simulates NFA
 * using Thompson's algorithm.
 *
 * See also http://swtch.com/~rsc/regexp/ and
 * Thompson, Ken.  Regular Expression Search Algorithm,
 * Communications of the ACM 11(6) (June 1968), pp. 419-422.
 * 
 * Copyright (c) 2007 Russ Cox.
 * Can be distributed under the MIT license, see bottom of file.
 */
int convertRegularExpressionInfixToPostfix(const PtTouchSymbols &infix, PtTouchSymbols &postfix)
{
    int nalt = 0, natom = 0;
    struct {
		int nalt;
		int natom;
	} paren[100], *p;
    p = paren;
    
    for(int i = 0; i < (int)infix.size(); i++) {
        PtTouchSymbol *touchSymbol = (infix)[i];
        if(touchSymbol->isEqualToString(std::string("("))) {
            if(natom > 1) {
                --natom;
                postfix.push_back(new PtTouchSymbol(std::string(".")));
            }
            if(p >= paren+100)
				return 0;
			p->nalt = nalt;
			p->natom = natom;
			p++;
			nalt = 0;
			natom = 0;
        } else if(touchSymbol->isEqualToString(std::string("|"))) {
            if(natom == 0)
				return 0;
			while(--natom > 0)
                postfix.push_back(new PtTouchSymbol(std::string(".")));
			nalt++;
        } else if(touchSymbol->isEqualToString(std::string(")"))) {
            if(p == paren)
				return 0;
			if(natom == 0)
				return 0;
			while(--natom > 0)
                postfix.push_back(new PtTouchSymbol(std::string(".")));
			for(; nalt > 0; nalt--)
                postfix.push_back(new PtTouchSymbol(std::string("|")));
			--p;
			nalt = p->nalt;
			natom = p->natom;
			natom++;
        } else if(touchSymbol->isEqualToString(std::string("*"))) {
            if(natom == 0)
				return 0;
            postfix.push_back(new PtTouchSymbol(touchSymbol->symbolString(), touchSymbol->triggers()));
        } else {
            if(natom > 1){
				--natom;
                postfix.push_back(new PtTouchSymbol(std::string(".")));
			}
            postfix.push_back(new PtTouchSymbol(touchSymbol->symbolString(), touchSymbol->triggers()));
			natom++;            
        }
    }
    if(p != paren)
		return 0;
	while(--natom > 0)
        postfix.push_back(new PtTouchSymbol(std::string(".")));
	for(; nalt > 0; nalt--)
        postfix.push_back(new PtTouchSymbol(std::string("|")));
        
    return 1;
}

typedef struct State State;
struct State
{
	int c;
    PtTouchSymbol *symbol;
	State *out;
	State *out1;
	int lastlist;
    int visited;
};

/* Allocate and initialize State */
State* state(PtTouchSymbol *symbol, State *out, State *out1)
{
	State *s;
	
	//nstate++;
	s = (State*)malloc(sizeof *s);
	s->lastlist = 0;
    s->visited = 0;
	s->symbol = symbol;
	s->out = out;
	s->out1 = out1;
	return s;
}

/*
 * A partially built NFA without the matching state filled in.
 * Frag.start points at the start state.
 * Frag.out is a list of places that need to be set to the
 * next state for this fragment.
 */
typedef struct Frag Frag;
typedef union Ptrlist Ptrlist;
struct Frag
{
	State *start;
	Ptrlist *out;
};

/* Initialize Frag struct. */
Frag frag(State *start, Ptrlist *out)
{
	Frag n = { start, out };
	return n;
}

/*
 * Since the out pointers in the list are always 
 * uninitialized, we use the pointers themselves
 * as storage for the Ptrlists.
 */
union Ptrlist
{
	Ptrlist *next;
	State *s;
};

/* Create singleton list containing just outp. */
Ptrlist*
list1(State **outp)
{
	Ptrlist *l;
	
	l = (Ptrlist*)outp;
	l->next = NULL;
	return l;
}

/* Patch the list of states at out to point to start. */
void
patch(Ptrlist *l, State *s)
{
	Ptrlist *next;
	
	for(; l; l=next){
		next = l->next;
		l->s = s;
	}
}

/* Join the two lists l1 and l2, returning the combination. */
Ptrlist*
append(Ptrlist *l1, Ptrlist *l2)
{
	Ptrlist *oldl1;
	
	oldl1 = l1;
	while(l1->next)
		l1 = l1->next;
	l1->next = l2;
	return oldl1;
}


/*
 * Convert postfix regular expression to NFA.
 * Return start state.
 */
State* convertRegularExpressionPostfixToNFA(const PtTouchSymbols &postfix, int *nState)
{
	Frag stack[1000], *stackp, e1, e2, e;
	State *s;

	// fprintf(stderr, "postfix: %s\n", postfix);

	if(postfix.size() == 0)
		return NULL;

	#define push(s) *stackp++ = s
	#define pop() *--stackp

	stackp = stack;
	for(unsigned int i = 0; i < postfix.size(); i++){
        PtTouchSymbol *touchSymbol = postfix[i];
        if(touchSymbol->isEqualToString(std::string("."))) {
            e2 = pop();
			e1 = pop();
			patch(e1.out, e2.start);
			push(frag(e1.start, e2.out));
        } else if(touchSymbol->isEqualToString(std::string("|"))) {
            e2 = pop();
			e1 = pop();
			s = state(_SplitSymbol, e1.start, e2.start);
            (*nState)++;
			push(frag(s, append(e1.out, e2.out)));
        } else if(touchSymbol->isEqualToString(std::string("*"))) {
            e = pop();
			s = state(_SplitSymbol, e.start, NULL);
            (*nState)++;
			patch(e.out, s);
			push(frag(s, list1(&s->out1)));
        } else {
            s = state(touchSymbol, NULL, NULL);
            (*nState)++;
			push(frag(s, list1(&s->out)));
        }
	}

	e = pop();
	if(stackp != stack)
		return NULL;

	//patch(e.out, &matchstate);
    patch(e.out, state(_MatchSymbol, NULL, NULL));
	return e.start;
#undef pop
#undef push
}

/*
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall
 * be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/// END: modified regular expression implementation

void traverseState(State *state, int *counter, std::vector<State *> *stateVector)
{
    if(state->visited != 0) return;
    (*counter)++;
    state->visited = *counter;
    stateVector->push_back(state);
    if(state->symbol->isEqualToString(std::string("MATCH"))) {
        //printf("Match -- State Id: %d\n", state->visited);
    } else if (state->symbol->isEqualToString(std::string("SPLIT"))) {
        //printf("Split -- State Id: %d\n", state->visited);
        traverseState(state->out, counter, stateVector);
        traverseState(state->out1, counter, stateVector);
        //printf("End Split -- State Id: %d\n", state->visited);
    } else {
        //printf("Character %s -- State Id: %d\n", [[state->symbol string] UTF8String], state->visited);
        traverseState(state->out, counter, stateVector);
    }
}

int createNFATransitionTable(std::vector<State *> *stateVector, PtTransitionTable &transitionTable)
{
    for(unsigned int i = 0; i < stateVector->size(); i++) {
        State *state = (*stateVector)[i];
        if(state->symbol->isEqualToString(std::string("MATCH"))) {
            //printf("Match -- State Id: %d\n", state->visited);
        } else if(state->symbol->isEqualToString(std::string("SPLIT"))) {
            //printf("Split -- State Id: %d to State Id: %d\n", state->visited, state->out->visited);
            //printf("Split -- State Id: %d to State Id: %d\n", state->visited, state->out1->visited);
            transitionTable.addTransition(new PtTouchSymbol(std::string("~")), state->visited, state->out->visited);
            transitionTable.addTransition(new PtTouchSymbol(std::string("~")), state->visited, state->out1->visited);
        } else {
            //printf("Symbol %s -- State Id: %d to State Id: %d\n", [[state->symbol string] UTF8String], state->visited, state->out->visited);
            PtTouchSymbol *touchSymbol = new PtTouchSymbol(std::string(""));
            *touchSymbol = *(state->symbol);
            transitionTable.addTransition(touchSymbol, state->visited, state->out->visited);
        }
    }
    return 1;
}

int convertRegularExpressionToNFA(const PtTouchSymbols &regexp, PtTransitionTable &transitionTable)
{
    int nState = 0, count = 0;
    PtTouchSymbols postfix;
    int success = convertRegularExpressionInfixToPostfix(regexp, postfix); // must delete contents
    if(success) {
        State *start = convertRegularExpressionPostfixToNFA(postfix, &nState);
        
        std::vector<State*> *stateVector = new std::vector<State*>();
        
        traverseState(start, &count, stateVector);
        createNFATransitionTable(stateVector, transitionTable);
        
        for(unsigned int i = 0; i < stateVector->size(); i++) {
            State *state = (*stateVector)[i];
            free(state);
        }
        delete stateVector;
        transitionTable.removeEmptyTransitions();
        
        for(unsigned int i = 0; i < postfix.size(); i++) {
            delete postfix[i];
        }
        
        return 1;
    } else {
        for(unsigned int i = 0; i < postfix.size(); i++) {
            delete postfix[i];
        }
        return 0;
    }
}

int convertRegularExpressionToDFA(const PtTouchSymbols &regexp, PtTransitionTable &transitionTable)
{
    PtTransitionTable tableNFA;
    int success = convertRegularExpressionToNFA(regexp, tableNFA);
    if(success) {
        return tableNFA.convertToDFA(transitionTable);
    } else {
        return 0;
    }
}

int convertStringToTouchSymbolRegularExpression(std::string stringRegexp, std::string delimiter, PtTouchSymbols &regularExpression)
{
    if(regularExpression.size() != 0) return 0;

    int i = 0;
    
    while(stringRegexp.size() - i > 0) {
        if(stringRegexp[i] == '(' || stringRegexp[i] == ')' || stringRegexp[i] == '|' || stringRegexp[i] == '*') {
            regularExpression.push_back(new PtTouchSymbol(stringRegexp.substr(i,1)));
            i++;
        } else {
            size_t found = stringRegexp.find(delimiter, i);
            regularExpression.push_back(new PtTouchSymbol(stringRegexp.substr(i, found-i)));
            i = found + delimiter.size();
        }
    }
    
    return 1;
}
