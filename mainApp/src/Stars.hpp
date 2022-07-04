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
    void update(float speedMul=1, glm::vec2 size = glm::vec2(1280, 800));
    virtual void display();
    virtual void reset(){};
    float r = 10;
    glm::vec3 speed;
    glm::vec3 originalLoc;
    void setLoc(glm::vec3 loc, bool bSetOrigin = false);
    void translate(glm::vec3 t);
    ofColor color;
    glm::vec3 locToDraw;
    glm::vec3 loc;
private:
};

class ShootingStar: public Star{
public:
    ShootingStar(){};
    
};

class Planet: public Star{
public:
    Planet();
    vector<ofMesh> rings;
    void display() override;
    void reset() override;
    glm::vec3 ringRotation;
    int numRings;
    bool bVisible = true;
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
    
    vector<Star*> stars;
    void initStars();
    void drawStars();
    ofFbo starsFbo;
    ofImage planet[2];
    float* planetID;
    
};
#endif /* Star_hpp */
