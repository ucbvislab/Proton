/*
 *  Scene.h
 *  Proton
 *
 *  Created by Kenrick Kin on 8/13/12.
 *
 */
 
#ifndef SCENE_H
#define SCENE_H

#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

enum SceneObjectType {
    SOT_CIRCLE,
    SOT_SQUARE,
    SOT_OTHER,
};

class Scene {
    double  _circle[2];
    double  _square[2];
    bool    _connect;

public:
    Scene() {
        _connect = false;
    
        _circle[0] = -.25;
        _circle[1] = 0;
        
        _square[0] = 0.25;
        _square[1] = 0;
    }

    void connect() {
        _connect = !_connect;
    }

    void draw() {
        glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0f,0.0f,0.0f);
        glVertex2f(_circle[0], _circle[1]);
        for(int angle = 0; angle < 360; angle++) {
            glVertex2f(_circle[0] + sin(angle) * 0.2, _circle[1] + cos(angle) * 0.2);
        }
        glEnd();
        
        glBegin(GL_QUADS);
        glColor3f(0.0f,0.0f,1.0f);
        glVertex2f(_square[0]-0.2, _square[1]-0.2);
        glVertex2f(_square[0]+0.2, _square[1]-0.2);
        glVertex2f(_square[0]+0.2, _square[1]+0.2);
        glVertex2f(_square[0]-0.2, _square[1]+0.2);
        glEnd();
        
        if(_connect){
            glBegin(GL_LINES);
            glEnable(GL_LINE_SMOOTH_HINT);
            glLineWidth(5.0);
            glColor3f(1.0f, 1.0f, 1.0f);
            glVertex2f(_circle[0], _circle[1]);
            glVertex2f(_square[0], _square[1]);
            glEnd();
        }
    }

    /// returns the object type underneath the touch position
    /// note that this is a cheat and anything in the left half is considered a circle
    /// while anything on the right half is considered a square
    SceneObjectType hitTest(double x, double y) {
        if(x < 0.5)
            return SOT_CIRCLE;
        else
            return SOT_SQUARE;
    }
};

#endif
