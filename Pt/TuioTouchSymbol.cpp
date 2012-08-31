/*
 *  TuioTouchSymbol.cpp
 *  Proton
 *
 *  Created by Kenrick Kin on 6/21/12.
 *
 */

#include "TuioTouchSymbol.h"

PtTuioTouchSymbol::~PtTuioTouchSymbol()
{
    if(_tcur != NULL)
        delete _tcur;
}

void PtTuioTouchSymbol::setTuioCursor(TUIO::TuioCursor *tcur)
{
    _tcur = new TUIO::TuioCursor(tcur);
}

TUIO::TuioCursor* PtTuioTouchSymbol::tuioCursor() const
{
    return _tcur;
}
