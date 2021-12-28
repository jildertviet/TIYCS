//
//  Star.cpp
//  spaceTravel
//
//  Created by Jildert Viet on 09/09/2021.
//

#include "Stars.hpp"

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


Stars::Stars(glm::vec2 size){
    for(int i=0; i<500; i++){
        stars.push_back(new Star());
        stars.back()->setLoc(
                            glm::vec3(
                                      ofGetWidth() * ofRandom(-1, 2),
                                      ofRandom(ofGetHeight()), -ofRandom(500, 1500)
                                      ),
                             true
                             );
    }
//    ofSetSphereResolution(180);
    
    // Setup post-processing chain
//    post.init(ofGetWidth(), ofGetHeight());
//    post.createPass<FxaaPass>()->setEnabled(false);
//    bloom = post.createPass<BloomPass>();
//    bloom->setEnabled(true);
    
    starsFbo.allocate(size.x, size.y, GL_RGB);
}

void Stars::update(){
    for(int i=0; i<stars.size(); i++)
        stars[i]->update(*travelSpeed);
}

void Stars::display(float brightness){
    ofPushMatrix();
    ofTranslate(0, ofGetHeight() * (*height));

    
    starsFbo.begin();
    ofClear(0);
//    post.begin();
    
    ofSetColor(255);
    ofFill();
    for(int i=0; i<stars.size(); i++){
        stars[i]->display();
    }
    ofSetColor(0, pow(1-((*height) / 2), 0.5) * 200);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
//    post.end();
    starsFbo.end();
    
    
    ofSetColor(255, brightness);
    starsFbo.draw(0, ofGetHeight()*-2);

    planet.draw(0,0);
    ofPopMatrix;
}
