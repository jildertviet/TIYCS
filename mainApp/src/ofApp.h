#pragma once

#include "ofMain.h"
#include "ofxJVisuals.hpp"
//#include "ofxOsc.h"
#include "ofxBox2d.h"
//#include "ofxPostProcessing.h"
#include "Star.hpp"

enum State{
    Intro, // 0
    Instructions,
    CaptainPicto, // 2
    Waveform,
    StarsVideo, // 4
    Route,
    Commercial0, // 6
    Commercial1,
    Commercial2, // 8
    Benzine,
    MartianWindow, // 10
    Countdown,
    JoniskBig, // 12
    Bingo,
    Code, // 14
    Autopilot,
    Return, // 16
    BingoBall,
    None, // 18
    WordtVervolgd,
    BingoBg // 2
};

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
		
    void processMsg(ofxOscMessage &m);
    bool bCountDown = false;
    ofxOscReceiver receiver;
    State state;
    
    ofImage jonisk;
    ofImage joniskBig;
    ofImage welcomTxt;
    ofImage benzine;
    ofImage martian[3];
    ofImage gradient;
    ofImage captainPicto;
    float meterAngle = 0;
    ofColor bgColor = ofColor(13);
    int id = 0;
    ofVideoPlayer commercial[3];
//    ofVideoPlayer stars[3];
    
    
    vector<float> waveForm;
    
    ofTrueTypeFont countFont;
    ofTrueTypeFont helveticaBold;
    ofTrueTypeFont helveticaRegular;
    float count = 10;
    
    float brightness = 255;
    ofImage instructions[7];
    int instructionsID = 0;
    
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
    
    ofImage bingoSwirl[2];
    ofImage bingoBase;
    ofImage codeTxt;
    ofImage codeCircle;
    
    int codeLen = 0;
    int code[4] = {4, 5, 9, 3};
    ofTrueTypeFont codeFont;
    
    float bingoForce = 1;
    float                                   px, py;
    bool                                    bDrawLines;
    bool                                    bMouseForce;
    
    ofxBox2d                                box2d;           // the box2d world
    ofPolyline                              drawing;         // we draw with this first
    ofxBox2dEdge                            edgeLine;        // the box2d edge/line shape (min 2 points)
    vector    <shared_ptr<ofxBox2dCircle> > circles;         // default box2d circles
    
    ofTrueTypeFont bingoFont;
    void initBingo();
    
    
    void joniskHover();
    float autoPilotPct = 0;
    ofImage autoPilot;
    ofTrueTypeFont autoPilotFont;
    
    bool bRedBg = false;
    
    ofImage returnImages[4];
    int returnImageIndex = 0;
    
    void setOsc(int port);
    
    float welcomIntensity = 1;
    void setCommercialAmp(float amp);
    ofImage wordtVervolgd;
    
    float height = 2;
    float speed = 1;
    ofImage planet;
    
//    ofxPostProcessing post;
//    BloomPass::Ptr bloom;
    vector<Star*> stars;
    void initStars();
    void drawStars();
    
    ofFbo starsFbo;
    bool bRotateBingo = false;
    int rotStart = 0;
    
    ofImage codeGlow;
    
    ofxJVisuals* v;
};

