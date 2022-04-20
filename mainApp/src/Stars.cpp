//
//  Star.cpp
//  spaceTravel
//
//  Created by Jildert Viet on 09/09/2021.
//

#include "Stars.hpp"

Star::Star(){
    r = pow(ofRandom(1.0), 4.0) * 5;
    r = 5;
    speed = glm::vec3(0,0,1);
    speed.z = ofRandom(1.5, 4);
    speed.x = ofRandom(-1, 1) * 0.1;
    color = ofColor(255, ofRandom(200, 255));
}

void Star::update(float speedMul){
    loc += speed * speedMul;
    if(loc.z > 1000){
//        loc = originalLoc;
        loc.z -= 3000;
    }
    if(loc.x < ofGetWidth() * -1){
        loc.x += ofGetWidth() * 3;
    }
    if(loc.x > ofGetWidth() * 2){
        loc.x += ofGetWidth() * -3;
    }
    locToDraw = loc;
}

void Star::display(){
    ofSetColor(color);
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
//    return;
    
    locToDraw += t;
//    locToDraw.x = (int)locToDraw.x % (ofGetWidth() * 3); // numScreens ...
//    locToDraw.y = (int)locToDraw.y % ofGetHeight();
//    locToDraw.z = ((int)locToDraw.z % 2000) - 1000;
    
//    if(locToDraw.z >= 1000){
//        locToDraw.z = locToDraw.z - 2000;
//    }
    
    /*
     Rotate the two outter screens, so the back plane is ajecting?
     */
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------||
Stars::Stars(glm::vec2 size, string prefix){
    planet.load(prefix + "planet.png");

    for(int i=0; i<100; i++){ // Happens in main.cpp
        stars.push_back(new Star());
        stars.back()->setLoc(
                            glm::vec3(
                                      ofGetWidth() * ofRandom(1),
//                                      ofGetWidth()*i,
                                      i*10,
//                                      ofRandom(ofGetHeight()),
//                                      ofRandom(-500, 500)
                                      0
//                                      0, 0,
//                                      100
                                      ),
                             true
                             );
    }
    
//    for(int x=0; x<10; x++){
//        for(int y=0; y<10; y++){
//            stars.push_back(new Star());
//            stars.back()->setLoc(glm::vec3(x * (1280/10), y * (800/10), -500));
//        }
//    }
    
//    ofSetSphereResolution(180);
    
    // Setup post-processing chain
    post.init(ofGetWidth(), ofGetHeight());
//    post.createPass<FxaaPass>()->setEnabled(false);
    bloom = post.createPass<BloomPass>();
    bloom->setEnabled(true);
//    kaleidoscope = post.createPass<KaleidoscopePass>();
//    kaleidoscope->setEnabled(true);
    
    this->size = size;
    starsFbo.allocate(size.x, size.y, GL_RGB); // Memory issue @ RBP?
    
}

void Stars::update(glm::vec3 t){
//    t.x += ofGetWidth() * (*hOffset);
    for(int i=0; i<stars.size(); i++){
//        stars[i]->translate(t);
        stars[i]->update(*travelSpeed);
    }
    
    starsFbo.begin();
//    ofClear(0, 127);
    post.begin();
    
    ofSetColor(255);
    ofFill();
    for(int i=0; i<stars.size(); i++){
        stars[i]->display();
    }

    post.end();
    
    ofSetColor(0, pow(1-((*height) / 2), 0.5) * 200); // Black fade when lowering
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
    starsFbo.end();
}

void Stars::display(float brightness){
    ofPushMatrix();

    ofTranslate(0, ofGetHeight() * (*height));
    
    ofSetColor(255, brightness);
    starsFbo.draw(0, ofGetHeight() * -2);

    planet.draw(ofGetWidth() * (*hOffset), ofGetHeight() - planet.getHeight());
    ofPopMatrix;
}
