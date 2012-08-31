/*
 *  TuioTouchSymbol.h
 *  Proton
 *
 *  Created by Kenrick Kin on 6/21/12.
 *
 */

#include "TouchSymbol.h"

#include "TUIO/TuioCursor.h"

/// PtTuioTouchSymbol is a subclass of PtTouchSymbol. PtTuioTouchSymbol stores the touch data of the TUIO event 
class PtTuioTouchSymbol : public PtTouchSymbol {
    TUIO::TuioCursor *_tcur;

public:
    PtTuioTouchSymbol(std::string symbolString) : PtTouchSymbol(symbolString), _tcur(NULL) {}
    ~PtTuioTouchSymbol();

    /// makes and stores copy of tcur
    void setTuioCursor(TUIO::TuioCursor *tcur);
    
    /// returns tcur
    TUIO::TuioCursor* tuioCursor() const;
};
