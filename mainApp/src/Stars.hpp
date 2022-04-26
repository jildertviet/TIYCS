//
//  Stars.hpp
//  spaceTravel
//
//  Created by Jildert Viet on 09/09/2021.
//

#ifndef Stars_hpp
#define Stars_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxPostProcessing.h"

class Star{
public:
    Star();
    void update(float speedMul=1);
    void display();
    float r = 10;
    glm::vec3 speed;
    glm::vec3 originalLoc;
    void setLoc(glm::vec3 loc, bool bSetOrigin = false);
    void translate(glm::vec3 t);
    ofColor color;
private:
    glm::vec3 loc;
    glm::vec3 locToDraw;
};

class Stars{
public:
    Stars(glm::vec2 size, string prefix);
    glm::vec2 size;
    
    void update(glm::vec3 t);
    void display(float brightness=255);

    float* height;
    float* hOffset;
    float* travelSpeed;
    
    ofxPostProcessing post;
    BloomPass::Ptr bloom;
    KaleidoscopePass::Ptr kaleidoscope;
    
    vector<Star*> stars;
    void initStars();
    void drawStars();
    ofFbo starsFbo;
    ofImage planet[2];
    float* planetID;
    
};
#endif /* Star_hpp */
