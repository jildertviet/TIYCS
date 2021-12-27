#pragma once

#include "ofMain.h"
#include "ofxJVisuals.hpp"
//#include "ofxOsc.h"
//#include "ofxPostProcessing.h"
#include "Star.hpp"
#include "Bingo.hpp"

#define NUM_BUSSES  12

namespace scenes{
    enum State{
        Intro, // 0, bus: 0 (hoverMul, 0<>2)
        JoniskBig, // 1

        Instructions, // 2, bus: 0 (imageID, 0<>5)
        Countdown, // 3, bus: 0 (count)
        Stars, // 4, bus: 0 (height, 0<>2)
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
        
        None,
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
		
    float busses[NUM_BUSSES];
    
    void processMsg(ofxOscMessage &m);
    bool bCountDown = false;
    ofxOscReceiver receiver;
    scenes::State state;
    
    std::map<std::string, ofImage> images;
    
    ofImage jonisk;
    ofImage joniskBig;
    ofImage welcomTxt;
    ofImage benzine;
    ofImage martian[3];
    ofImage gradient;
    ofImage captainPicto;
    ofColor bgColor = ofColor(13);
    int id = 0;
    ofVideoPlayer commercial[3];
//    ofVideoPlayer stars[3];
    
    Bingo* bingo;
    ofTrueTypeFont countFont;
    
    vector<float> waveForm;
    
    ofTrueTypeFont helveticaBold;
    ofTrueTypeFont helveticaRegular;
    
    float brightness = 255;
    ofImage instructions[7];
    
    bool bVluchtInfo = false;
    FlightInfo flightInfo;
    ofImage scale;
    
    ofImage routeStartEnd, lineGray, planetsGray, legenda, planetNames;
    ofImage joniskRoute, joniskRouteGlow;
    glm::vec2 joniskRoutePos = glm::vec2(466, 555);
    glm::vec2 start = glm::vec2(466, 555);
    glm::vec2 dest;
    ofImage lineWhite;
    float pct;
    
    ofImage codeTxt;
    ofImage codeCircle;
    
    int code[4] = {4, 5, 9, 3};
    ofTrueTypeFont codeFont;
    
    
    void joniskHover();
    ofImage autoPilot;
    ofTrueTypeFont autoPilotFont;
    
    bool bRedBg = false;
    
    ofImage returnImages[4];
    int returnImageIndex = 0;
    
    void setOsc(int port);
    
    void setCommercialAmp(float amp);
    ofImage wordtVervolgd;
    
    float speed = 1;
    ofImage planet;
    
//    ofxPostProcessing post;
//    BloomPass::Ptr bloom;
    vector<Star*> stars;
    void initStars();
    void drawStars();
    
    ofFbo starsFbo;

    
    ofImage codeGlow;
    
    ofxJVisuals* v;
};

