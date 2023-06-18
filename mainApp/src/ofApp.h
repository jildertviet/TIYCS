#pragma once

#include "ofMain.h"

#ifdef  TARGET_RASPBERRY_PI
    #include "ofxOMXPlayer.h"
#endif

#include "screen.hpp"
#include "ofxPostProcessing.h"
#include "ofxPiMapper.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
    void keyReleased(int key);

		void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseDragged(int x, int y, int button);

		void windowResized(int w, int h);

    int screenOrder[3] = {0,1,2};
    screen screens[3];
    ofVideoPlayer* commercials;
    ofShader blob;
    ofShader screenOrderShader;
    ofFbo toReArrange;
    ofxPostProcessing post;
    ofLight light;

    ofxPiMapper piMapper;
};
