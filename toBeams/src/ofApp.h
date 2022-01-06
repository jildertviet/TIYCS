#pragma once

#include "ofMain.h"
#include "JRectangle.hpp"
#include "ofxJVisuals.hpp"
//#include "ofxOsc.h"

enum State{
    Mouse,
    Perlin,
    Gradient,
    Rectangles,
    RectHor,
    Webcam
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    ofSerial serial;
    ofSerial serial2;
    
    ofFbo f;
    ofPixels p;
    char data[228];
    
    State state;
    vector<JRectangle*> r;
    JRectangle* rH;
    
    ofxOscReceiver receiver;
    
    ofVideoGrabber cam;
};
