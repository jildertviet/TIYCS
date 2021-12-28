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

class Star{
public:
    Star();
    void update(float speedMul=1);
    void display();
    float r = 10;
    glm::vec3 speed;
    glm::vec3 originalLoc;
    void setLoc(glm::vec3 loc, bool bSetOrigin = false);
private:
    glm::vec3 loc;
};

class Stars{
public:
    Stars(glm::vec2 size);
    
    void update();
    void display(float brightness=255);

    float* height;
    float* travelSpeed;
    
//    ofxPostProcessing post;
//    BloomPass::Ptr bloom;
    
    vector<Star*> stars;
    void initStars();
    void drawStars();
    ofFbo starsFbo;
    ofImage planet;
    
};
#endif /* Star_hpp */
