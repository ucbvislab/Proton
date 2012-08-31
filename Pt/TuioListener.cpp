/*
 *  TuioListenerPt.cpp
 *  Proton
 *
 *  Created by Kenrick Kin on 6/6/12.
 *
 */

#include "TuioListener.h"

#include "Utils.h"

#include "TuioTouchSymbol.h"
//#include <tr1/memory>

static bool _verbose = true;

PtTuioListener::PtTuioListener(int port, PtTouchProcessor *touchProcessor)
{
	_tuioClient = new TUIO::TuioClient(port);
	_tuioClient->addTuioListener(this);
	_tuioClient->connect();

    _touchProcessor = touchProcessor;
    
	if (!_tuioClient->isConnected()) {
		exit(1);
	}
}

void PtTuioListener::addTuioObject(TUIO::TuioObject *tobj)
{
//    if (verbose)
//        std::cout << "add obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() << std::endl;
}

void PtTuioListener::updateTuioObject(TUIO::TuioObject *tobj)
{
//    if (verbose) 	
//        std::cout << "set obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() 
//        << " " << tobj->getMotionSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
}

void PtTuioListener::removeTuioObject(TUIO::TuioObject *tobj)
{
//	if (verbose)
//		std::cout << "del obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ")" << std::endl;
}

void PtTuioListener::addTuioCursor(TUIO::TuioCursor *tcur)
{
	if (_verbose) 
		std::cout << "add cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << std::endl;
    //create touch down ("D") and touch ID string
    char buff[20];
    sprintf(buff, "D%d", tcur->getCursorID());
    std::string buffAsStdStr = buff;
    
    PtTuioTouchSymbol *touchSymbol = new PtTuioTouchSymbol(buffAsStdStr);
    touchSymbol->setTuioCursor(tcur);
    
    PtTouchSymbols *expression = new PtTouchSymbols();
    expression->push_back(touchSymbol);

    //_touchProcessor has responsibility for deleting expression
    _touchProcessor->processTouchSymbols(expression);
    
    //delete touchSymbol;
}

void PtTuioListener::updateTuioCursor(TUIO::TuioCursor *tcur)
{
	if (_verbose) 	
		std::cout << "set cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() 
					<< " " << tcur->getMotionSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
    char buff[20];
    sprintf(buff, "M%d", tcur->getCursorID());
    std::string buffAsStdStr = buff;

    PtTuioTouchSymbol *touchSymbol = new PtTuioTouchSymbol(buffAsStdStr);
    touchSymbol->setTuioCursor(tcur);
    
    PtTouchSymbols *expression = new PtTouchSymbols();
    expression->push_back(touchSymbol);

    _touchProcessor->processTouchSymbols(expression);
    
    //delete touchSymbol;
}

void PtTuioListener::removeTuioCursor(TUIO::TuioCursor *tcur)
{
	if (_verbose)
		std::cout << "del cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ")" << std::endl;
    char buff[20];
    sprintf(buff, "U%d", tcur->getCursorID());
    std::string buffAsStdStr = buff;

    PtTuioTouchSymbol *touchSymbol = new PtTuioTouchSymbol(buffAsStdStr);
    touchSymbol->setTuioCursor(tcur);
    
    PtTouchSymbols *expression = new PtTouchSymbols();
    expression->push_back(touchSymbol);

    _touchProcessor->processTouchSymbols(expression);
    
    //delete touchSymbol;
}

void PtTuioListener::refresh(TUIO::TuioTime frameTime)
{
	//drawObjects();
}
