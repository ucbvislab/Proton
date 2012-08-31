/*
 *  hitTester.cpp
 *  Proton
 *
 *  Created by Kenrick Kin on 8/12/12.
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
#include "HitTestAttributeGenerator.h"
#include "LeftRightSplitStreamAttributeGenerator.h"
#include "Scene.h"

double pXLeft = -0.5;
double pYLeft = 0;
double pXRight = 0.5;
double pYRight = 0;

static bool _verbose = true;
static Scene _scene;

void connectCallback(const std::vector<PtTouchSymbols *> &touchStream)
{
    if(_verbose)
        printf("CONNECT CALLBACK!\n");
    _scene.connect();
    glutPostRedisplay();    
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    _scene.draw();

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
    glutCreateWindow ("HitTester");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
  
    // creating a two-touch regular expression
    // the first touch must hit a circle object and the second touch must hit a square object
    // the second touch must lift up before the first touch lifts up
    PtTouchSymbols regexp;
    convertStringToTouchSymbolRegularExpression(std::string("D0:C_(M0:C_)*D1:S_(M0:C_|M1:S_)*U1:S_(M0:C_)*U0:C_"), std::string("_"), regexp);
    addTriggerToIthTouchSymbol(&regexp, 0, 7);

    // creating the two-touch gesture with two callbacks that do nothing
    PtGesture *gesture1 = new PtGesture(regexp);
    gesture1->setName(std::string("TWO_TOUCH"));
    gesture1->addCallback(connectCallback);

    clearRegexp(&regexp);

    // create a PtTouchProcesser with two gesture matchers
    PtTouchProcessor touchProcessor;
    // first matcher gets gesture1 and gesture2
    touchProcessor.addGesture(gesture1);
        
    // assigns a HitTestAttributeGenerator to gesture matchers at indices 0 and 1
    touchProcessor.addAttributeGenerator(new HitTestAttributeGenerator(&_scene));
    
    // create TUIO listener on port 3333
    PtTuioListener listener(3333, &touchProcessor);

    glutMainLoop();

    return 0;
}
