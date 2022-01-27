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
    ofDrawSphere(locToDraw, r);
}

void Star::setLoc(glm::vec3 loc, bool bSetOrigin){
    if(bSetOrigin)
        originalLoc = loc;
    this->loc = loc;
    this->locToDraw = loc;
}

void Star::translate(glm::vec3 t){
    locToDraw = loc;
    locToDraw += t;
    locToDraw.x = (int)locToDraw.x % 1280;
    locToDraw.y = (int)locToDraw.y % 800;
    locToDraw.z = (int)locToDraw.z % 100;
}


Stars::Stars(glm::vec2 size){
    planet.load("planet.png");

    for(int i=0; i<1; i++){
        stars.push_back(new Star());
        stars.back()->setLoc(
                            glm::vec3(
//                                      ofGetWidth() * ofRandom(-1, 2),
//                                      ofRandom(ofGetHeight()),
//                                      -ofRandom(500, 1500)
                                      0, 0,
                                      100
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
    this->size = size;
    starsFbo.allocate(size.x, size.y, GL_RGB);
}

void Stars::update(){
    for(int i=0; i<stars.size(); i++){
        stars[i]->translate(glm::vec3(0,0,ofGetFrameNum()));
//        stars[i]->update(*travelSpeed);
    }
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

    planet.draw(0,0, size.x, size.y);
    ofPopMatrix;
}
