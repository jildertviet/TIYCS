//
//  screen.hpp
//  mainApp
//
//  Created by Jildert Viet on 02/06/2022.
//

#ifndef screen_hpp
#define screen_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxJVisuals.hpp"
#include "Stars.hpp"
#include "Bingo.hpp"
#include "FboSource.h"
#include "ofxPiMapper.h"

#define NUM_BUSSES  24
#define PORT    5000
#define NUM_INSTRUCTIONS    8
#define NUM_RETURNIMAGES    8

class MappableFbo : public ofx::piMapper::FboSource {
	public:
    void setup(int width, int height){
      allocate(width, height);
    }
		void update(){};
		void draw(){
      ofClear(0);
      f->draw(0, 0);
    }

		ofFbo* f;
};


namespace scenes{
    enum Scene{
        Intro, // 0, bus: 0 (hoverMul, 0<>2)
        JoniskBig, // 1

        Instructions, // 2, bus: 0 (imageID, 0<>5)
        Countdown, // 3, bus: 0 (count)
        Stars, // 4, bus 0: height 0<>2, bus 1: travelSpeed, bus 2: xOffset (for 3 windows).
        Route,// 5, bus: 0 (height, 0<>2), route pct: 9
        Commercial, // 6
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
        Test, // 17
        Einde,
        Pseudo,
        QAndA, // 20

        StarsFinal
    };
}

struct FlightInfo{
    float height = 11000;
    float speed = 17;
    int temp = 270;
};

class screen {
public:
    screen();
    void setup(int portNumAdd = 0, glm::vec2 size = glm::vec2(1280, 800), int* screenOrder = nullptr);
    int* screenOrder;
    void update();

    void render(scenes::Scene pseudo = scenes::Pseudo, bool bUseFbo = true);
    void draw();

    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    void windowResized(int w, int h);

    int width, height;
    ofxOscReceiver receiver;
    int portNumAdd = 0;
    float busses[NUM_BUSSES];

    void processMsg(ofxOscMessage &m);

    scenes::Scene scene;
    std::map<std::string, ofImage> images;

    ofImage welcomTxt;
    ofImage captainPicto;
    ofImage instructions[NUM_INSTRUCTIONS];
    ofImage returnImages[NUM_RETURNIMAGES];

    ofImage routeStartEnd, lineGray, planetsGray, legenda, planetNames;
    ofImage joniskRoute, joniskRouteGlow;

    ofTrueTypeFont helveticaBold;
    ofTrueTypeFont helveticaRegular;
    ofTrueTypeFont countFont;
    ofTrueTypeFont codeFont;
    ofTrueTypeFont autoPilotFont;
    ofTrueTypeFont callCountFont;

    float brightness = 255;

    int id = 0;

    #ifdef  TARGET_RASPBERRY_PI
    ofxOMXPlayer commercial;
    #else
    ofVideoPlayer* commercial = nullptr;
    ofVideoPlayer* commercials;
    #endif
    bool bMuteAudio = true;

    vector<float> waveForm;

    bool bVluchtInfo = false;
    FlightInfo flightInfo;
    glm::vec2 joniskRoutePos = glm::vec2(466, 555);
    glm::vec2 start = glm::vec2(466, 555);
    glm::vec2 dest;
    ofImage lineWhite;
    float routePct;

    int code[4] = {4, 5, 9, 3};

    void joniskHover(bool bGradient = true);
    float seed = 0;

    Bingo* bingo;
    Stars* stars;

    ofxJVisuals* v;
    ofMesh lineMesh;

    #ifdef  TARGET_RASPBERRY_PI
    bool bRotate = false;
    #else
    bool bRotate = false;
    #endif

    bool bEditMode = false;
    char fboDisplayMode = 0; // Mapped
    // vector<glm::vec3> meshVertices;
    // vector<glm::vec2> texCoords;
    // ofMesh mesh;
    // void initMesh();
    ofFbo renderFbo;
    float windowScale = 1.0;

    int whiteFrame = 0;
    bool bBlack = false;

    scenes::Scene sceneToUse;
    MappableFbo mappableFbo;
    ofxPiMapper* piMapper;
};
#endif /* screen_hpp */
