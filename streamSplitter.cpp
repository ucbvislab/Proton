/*
 *  streamSplitter.cpp
 *  Proton
 *
 *  Created by Kenrick Kin on 2/3/12.
 *
 */

#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Pt/Gesture.h"
#include "Pt/TouchProcessor.h"
#include "Pt/TouchSymbol.h"
#include "Pt/TuioListener.h"
#include "Pt/TransitionTable.h"
#include "Pt/Utils.h"
#include "KKAttributeGenerator.h"
#include "LeftRightSplitStreamAttributeGenerator.h"

double pXLeft = -0.5;
double pYLeft = 0;
double pXRight = 0.5;
double pYRight = 0;

static bool _verbose = true;

void twoTouchCallback0(const std::vector<PtTouchSymbols *> &touchStream)
{
    if(_verbose)
        printf("TWO TOUCH CALLBACK 0!\n");
}

void twoTouchCallback1(const std::vector<PtTouchSymbols *> &touchStream)
{
    if(_verbose)
        printf("TWO TOUCH CALLBACK 1!\n");
}

void oneTouchLeftCallback(const std::vector<PtTouchSymbols *> &touchStream)
{
    if(_verbose)
        printf("ONE TOUCH LEFT CALLBACK!\n");
    PtTouchSymbols *lastFrame = touchStream[touchStream.size()-1];
    PtTuioTouchSymbol *tS = static_cast<PtTuioTouchSymbol*>((*lastFrame)[lastFrame->size()-1]);
    pXLeft = tS->tuioCursor()->getX()*2-1;
    pYLeft = (1-tS->tuioCursor()->getY())*2-1;
    glutPostRedisplay();    
}

void oneTouchRightCallback(const std::vector<PtTouchSymbols *> &touchStream)
{
    if(_verbose)
        printf("ONE TOUCH RIGHT CALLBACK!\n");
    PtTouchSymbols *lastFrame = touchStream[touchStream.size()-1];
    PtTuioTouchSymbol *tS = static_cast<PtTuioTouchSymbol*>((*lastFrame)[lastFrame->size()-1]);
    pXRight = tS->tuioCursor()->getX()*2-1;
    pYRight = (1-tS->tuioCursor()->getY())*2-1;
    glutPostRedisplay();
}

double halfConfidenceCalculator(const std::vector<PtTouchSymbols *> &touchStream)
{
    return 0.5;
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBegin(GL_QUADS);
    glColor3f(1.0f,0.0f,0.0f);
    glVertex2f(pXLeft-0.2, pYLeft-0.2);
    glVertex2f(pXLeft+0.2, pYLeft-0.2);
    glVertex2f(pXLeft+0.2, pYLeft+0.2);
    glVertex2f(pXLeft-0.2, pYLeft+0.2);
    
    glColor3f(0.0f,0.0f,1.0f);
    glVertex2f(pXRight-0.2, pYRight-0.2);
    glVertex2f(pXRight+0.2, pYRight-0.2);
    glVertex2f(pXRight+0.2, pYRight+0.2);
    glVertex2f(pXRight-0.2, pYRight+0.2);
    glEnd();
    
    glutSwapBuffers();
}

void idle(void)
{
    //glutPostRedisplay();
}

void reshape(GLint width, GLint height)
{
    glViewport(0, 0, width, height);
}

int main (int argc, char** argv)
{
    glutInit (&argc, argv);
    glutInitWindowSize (512, 512);
    glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow ("Stream Splitter");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
  
    // creating a two-touch regular expression
    PtTouchSymbols regexp;
    convertStringToTouchSymbolRegularExpression(std::string("D0:KK_(M0:KK_)*D1:KK_(M0:KK_|M1:KK_)*(U0:KK_(M1:KK_)*U1:KK_|U1:KK_(M0:KK_)*U0:KK_)"), std::string("_"), regexp);
    addTriggerToIthTouchSymbol(&regexp, 0, 1);
    addTriggerToIthTouchSymbol(&regexp, 1, 3);

    // creating the two-touch gesture with two callbacks that do nothing
    PtGesture *gesture1 = new PtGesture(regexp);
    gesture1->setName(std::string("TWO_TOUCH"));
    gesture1->addCallback(twoTouchCallback0);
    gesture1->addCallback(twoTouchCallback1);    

    clearRegexp(&regexp);
    
    // creating a one-touch regular expression
    convertStringToTouchSymbolRegularExpression(std::string("D0:KK_(M0:KK_)*U0:KK_"), std::string("_"), regexp);
    addTriggerToIthTouchSymbol(&regexp, 0, 1);
    
    // creating the one-touch gesture
    PtGesture *gesture2 = new PtGesture(regexp);
    gesture2->setName(std::string("ONE_TOUCH_LEFT"));
    gesture2->addCallback(oneTouchLeftCallback);
    
    PtGesture *gesture3 = new PtGesture(regexp);
    gesture3->setName(std::string("ONE_TOUCH_RIGHT"));
    //gesture3->addCallback(oneTouchRightCallback, halfConfidenceCalculator);
    gesture3->addCallback(oneTouchRightCallback);
    
    clearRegexp(&regexp);   
    
    // create a PtTouchProcesser with two gesture matchers
    PtTouchProcessor touchProcessor(2);
    // first matcher gets gesture1 and gesture2
    touchProcessor.addGesture(gesture1, 0);
    touchProcessor.addGesture(gesture2, 0);
    // second matcher gets gesture3
    touchProcessor.addGesture(gesture3, 1);    
    
    // each matcher will only execute the callback with the highest confidence score
    //touchProcessor.setExecuteBestGesture(true, 0);
    //touchProcessor.setExecuteBestGesture(true, 1);    
    
    // assigns a KKAttributeGenerator to gesture matchers at indices 0 and 1
    touchProcessor.addAttributeGenerator(new KKAttributeGenerator(), 0);
    touchProcessor.addAttributeGenerator(new KKAttributeGenerator(), 1);    
    
    // adds a split stream attribute generator to split the stream by left and right halves of the screen
    touchProcessor.setSplitStreamAttributeGenerator(new LeftRightSplitStreamAttributeGenerator());
    
    // create TUIO listener on port 3333
    PtTuioListener listener(3333, &touchProcessor);

    glutMainLoop();

    return 0;
}
