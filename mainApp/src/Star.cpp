//
//  Star.cpp
//  spaceTravel
//
//  Created by Jildert Viet on 09/09/2021.
//

#include "Star.hpp"

Star::Star(){
    r = pow(ofRandom(1.0), 4.0) * 5;
    speed = glm::vec3(0,0,1);
    speed.z = ofRandom(1.5, 4);
}

void Star::update(float speedMul){
    loc += speed * speedMul;
    if(loc.z > 1000){
        loc = originalLoc;
    }
}

void Star::display(){
    ofSetColor(255);
    ofDrawSphere(loc, r);
}

void Star::setLoc(glm::vec3 loc, bool bSetOrigin){
    if(bSetOrigin)
        originalLoc = loc;
    this->loc = loc;
}
