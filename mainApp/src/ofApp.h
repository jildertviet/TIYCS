#pragma once

#include "ofMain.h"
#include "ofxJVisuals.hpp"
//#include "ofxOsc.h"
//#include "ofxPostProcessing.h"
#include "Stars.hpp"
#include "Bingo.hpp"
#ifdef  TARGET_RASPBERRY_PI
    #include "ofxOMXPlayer.h"
#endif

#define NUM_BUSSES  12

namespace scenes{
    enum Scene{
        Intro, // 0, bus: 0 (hoverMul, 0<>2)
        JoniskBig, // 1

        Instructions, // 2, bus: 0 (imageID, 0<>5)
        Countdown, // 3, bus: 0 (count)
        Stars, // 4, bus: 0 (height, 0<>2), bus: 1 travelSpeed
        Route,// 5, bus: 0 (height, 0<>2)
        Commercial0, // 6
        Commercial1, // 7
        Commercial2, // 8
        
        Benzine, // 9, bus: 0 (meterAngle, )
        ReturnToShip, // 10, bus: 0 (imgIndex, 0<>3)

        CaptainPicto, // 11
        Waveform, // 12
        
        Bingo, // 13, bus: 0 (bingoState, 0<>3), bus: 1 (bingoForce)

        Code, // 14, bus: 0 (codeLen, 0<>3)
        Autopilot, // 15, bus: 0 (autoPilotPct 0<>100)
        
        Nothing, // 16
    };
}

struct FlightInfo{
    float height = 11000;
    float speed = 17;
    int temp = 270;
};

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void mousePressed(int x, int y, int button);
		void windowResized(int w, int h);
    
    ofxOscReceiver receiver;
    float busses[NUM_BUSSES];
    
    void processMsg(ofxOscMessage &m);
    
    scenes::Scene scene;
    std::map<std::string, ofImage> images;
    
    ofImage welcomTxt;
    ofImage captainPicto;
    ofImage instructions[7];
    ofImage returnImages[4];
    
    ofImage routeStartEnd, lineGray, planetsGray, legenda, planetNames;
    ofImage joniskRoute, joniskRouteGlow;
    
    ofTrueTypeFont helveticaBold;
    ofTrueTypeFont helveticaRegular;
    ofTrueTypeFont countFont;
    ofTrueTypeFont codeFont;
    ofTrueTypeFont autoPilotFont;
    
    float brightness = 255;

    int id = 0;

#ifdef  TARGET_RASPBERRY_PI
    ofxOMXPlayer commercial[3];
#else
    ofVideoPlayer commercial[3];
#endif
    
    vector<float> waveForm;
    
    bool bVluchtInfo = false;
    FlightInfo flightInfo;
    glm::vec2 joniskRoutePos = glm::vec2(466, 555);
    glm::vec2 start = glm::vec2(466, 555);
    glm::vec2 dest;
    ofImage lineWhite;
    float routePct;
    
    int code[4] = {4, 5, 9, 3};
    
    void joniskHover();
    
    Bingo* bingo;
    Stars* stars;

    ofxJVisuals* v;
};
