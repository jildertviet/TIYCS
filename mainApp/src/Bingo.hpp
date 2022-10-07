//
//  Bingo.hpp
//  mainApp
//
//  Created by Jildert Viet on 27/12/2021.
//

#ifndef Bingo_hpp
#define Bingo_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxBox2d.h"

class Bingo{
public:
    enum DrawMode{
        BACKGROUND,
        WHEEL,
        NUMBER,
        BACKGROUND_AND_WHEEL
    };
    DrawMode state;
    
    Bingo(string prefix = "1280/", glm::vec2 size = glm::vec2(1280, 800));
    void update();
    void display(DrawMode mode = DrawMode::BACKGROUND);
    void display(DrawMode a, DrawMode b);
    void display(DrawMode a, DrawMode b, DrawMode c);
    
    float scaling = 1;
    void removeBall(string name);
    glm::vec2 size;
    
    bool bRotateBingo = false;
    int rotStart = 0;
    ofImage bingoSwirl[2];
    ofImage bingoBase;
    
    
    float bingoForce = 1;
    float                                   px, py;
    bool                                    bDrawLines;
    bool bMouseForce = false;
    
    ofxBox2d                                box2d;           // the box2d world
    ofPolyline                              drawing;         // we draw with this first
    ofxBox2dEdge                            edgeLine;        // the box2d edge/line shape (min 2 points)
    vector    <shared_ptr<ofxBox2dCircle> > circles;         // default box2d circles
    
    ofTrueTypeFont bingoFont;
    ofTrueTypeFont countFont;
    float count = 10;
    float* numberScale;

    void reInit();
    
};
#endif /* Bingo_hpp */
