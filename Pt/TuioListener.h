/*
 *  TuioListenerPt.h
 *  Proton
 *
 *  Created by Kenrick Kin on 6/6/12.
 *
 */

#ifndef PT_TUIOLISTENER_H
#define PT_TUIOLISTENER_H

#include "TUIO/TuioListener.h"
#include "TUIO/TuioClient.h"
#include "TUIO/TuioObject.h"
#include "TUIO/TuioCursor.h"
#include "TUIO/TuioPoint.h"
#include <list>
#include <math.h>
#include "TouchProcessor.h"

/// PtTuioListener listens for TUIO events, converts them to PtTuioTouchSymbols, and sends them to the
/// specified PtTouchProcessor for processing.
class PtTuioListener : public TUIO::TuioListener { 
	TUIO::TuioClient *_tuioClient; // client for listening for tuio touch events
    PtTouchProcessor *_touchProcessor; // processes Proton touch events

public:
	PtTuioListener(int port, PtTouchProcessor *touchProcessor);
    
    /// these are unused
	void addTuioObject(TUIO::TuioObject *tobj);
	void updateTuioObject(TUIO::TuioObject *tobj);
	void removeTuioObject(TUIO::TuioObject *tobj);
	
    /// generates D (down) symbol
	void addTuioCursor(TUIO::TuioCursor *tcur);
    
    /// generates M (move) symbol
	void updateTuioCursor(TUIO::TuioCursor *tcur);
    
    /// generates U (move) symbol
	void removeTuioCursor(TUIO::TuioCursor *tcur);
	
	void refresh(TUIO::TuioTime frameTime);
};

#endif
