//
//  Star.cpp
//  spaceTravel
//
//  Created by Jildert Viet on 09/09/2021.
//

#include "Stars.hpp"

Star::Star(){
    r = pow(ofRandom(1.0), 4.0) * 15;
//    r = 5;
    speed = glm::vec3(0,0,1);
    speed.z = ofRandom(1.5, 4);
    speed.x = ofRandom(-1, 1) * 0.1;
    color = ofColor(255, ofRandom(200, 255));
}

void Star::update(float speedMul, glm::vec2 size){
    loc += speed * speedMul;
    if(loc.z > 1000){
//        loc = originalLoc;
        loc.z -= 3000;
        reset();
    }
    if(loc.x < size.x * -1){
        loc.x += size.x * 3;
    }
    if(loc.x > size.x * 2){
        loc.x += size.x * -3;
    }
    locToDraw = loc;
}

void Star::display(){
    ofSetColor(color);
    ofDrawSphere(locToDraw, r);
}

Planet::Planet(){
    r = pow(ofRandom(1.0), 4.0) * 15 * 2; // 2x star
//    r = 5;
    speed = glm::vec3(0,0,1);
    speed.z = ofRandom(1.5, 4) * 0.5; // 2x slower
    speed.x = ofRandom(-1, 1) * 0.1;
    color = ofColor(255, ofRandom(200, 255));
    
    numRings = pow(ofRandom(1.0), 3.0) * 3.0 + 1.0;
//    numRings = 5;
    for(int j=0; j<numRings; j++){
        rings.push_back(ofMesh());
        rings[j].setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        for(int i=0; i<91; i++){
            float x = cos((i/90.)*TWO_PI) * (r * (2.0 + (j*2.0)));
            float y = sin((i/90.)*TWO_PI) * (r * (2.0 + (j*2.0)));
            float z = 0;
            float xInner = cos((i/90.)*TWO_PI) * (r * (1.3 + (j*2)));
            float yInner = sin((i/90.)*TWO_PI) * (r * (1.3 + (j*2)));
            rings[j].addVertex(glm::vec3(x, y, z));
            rings[j].addVertex(glm::vec3(xInner, yInner, z));
        }
    }
    ringRotation = glm::vec3(1, 0, ofRandom(-0.2, 0.2));
    
    if(ofRandom(10) > 7){
        bVisible = true;
    } else{
        bVisible = false;
    }
}

void Planet::display(){
    if(!bVisible){
        return;
    }
//    ofPopMatrix();
    ofSetColor(color);

    ofNoFill();
    ofPushMatrix();
    ofTranslate(locToDraw);
    ofRotateDeg(90, ringRotation[0], ringRotation[1], ringRotation[2]);
    ofEnableDepthTest();
    ofFill();
    ofDrawSphere(0, 0, r);
    ofSetColor(255, 200);
    for(int i=0; i<rings.size(); i++)
        rings[i].draw();
    ofDisableDepthTest();
    ofPopMatrix();
//    ofDrawCircle(locToDraw, r * 1.5);
}

void Planet::reset(){
    if(ofRandom(10) > 7){
        bVisible = true;
    } else{
        bVisible = false;
    }
    ringRotation = glm::vec3(1, 0, ofRandom(-0.2, 0.2));
//    loc.x *= ofRandom(10.);
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
    this->size = size;
    planet[0].load(prefix + "planetA.png");
    planet[1].load(prefix + "planetB.png");

    for(int i=0; i<100; i++){
        stars.push_back(new Star());
        float x = ofRandom(size.x-2)+1;
        while(abs(x - size.x*0.5) < 10){
            x = ofRandom(size.x-2)+1;
        }
        stars.back()->setLoc(
                            glm::vec3(
                                      x,
                                      i*10,
                                      0
                                      ),
                             true
                             );
    }
    for(int i=0; i<5; i++){
        stars.push_back(new Planet());
        float x = ofRandom(size.x-2)+1;
        while(abs(x - size.x*0.5) < 10){
            x = ofRandom(size.x-2)+1;
        }
        stars.back()->setLoc(
                            glm::vec3(
                                      x,
                                      ofRandom(size.y-2)+1,
                                      0
                                      ),
                             true
                             );
    }
    
//    ofSetSphereResolution(180);
    
    // Setup post-processing chain
    post.init(size.x, size.y);
//    post.createPass<FxaaPass>()->setEnabled(false);
    bloom = post.createPass<BloomPass>();
    bloom->setEnabled(true);
//    kaleidoscope = post.createPass<KaleidoscopePass>();
//    kaleidoscope->setEnabled(true);
    
    starsFbo.allocate(size.x, size.y, GL_RGB); // Memory issue @ RBP?
}

void Stars::update(glm::vec3 t){
    for(int i=0; i<stars.size(); i++){
        stars[i]->update(*travelSpeed);
    }
    ofDisableLighting();
    
    starsFbo.begin();
//    post.setFlip(false);
    post.begin();

    ofSetColor(255);
    ofFill();
    for(int i=0; i<stars.size(); i++){
        stars[i]->display();
    }

    post.end();
    
    ofSetColor(0, pow(1-((*height) / 2), 0.5) * 255); // Black fade when lowering
    ofDrawRectangle(0, 0, size.x, size.y);
    
    starsFbo.end();
}

void Stars::display(float brightness){
    ofPushMatrix();

    ofTranslate(0, size.y * (*height));
    
    ofSetColor(255, brightness);
    starsFbo.draw(0, size.y * -2);
    
    planet[(int)*planetID].draw(size.x * (*hOffset), size.y - planet[(int)*planetID].getHeight());
    ofPopMatrix;
}
