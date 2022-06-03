#pragma once

#include "ofMain.h"

#ifdef  TARGET_RASPBERRY_PI
    #include "ofxOMXPlayer.h"
#endif

#include "screen.hpp"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void mousePressed(int x, int y, int button);
		void windowResized(int w, int h);
    
    screen screens[3];
    ofVideoPlayer* commercials;
};
