//
//  Bingo.cpp
//  mainApp
//
//  Created by Jildert Viet on 27/12/2021.
//

#include "Bingo.hpp"


Bingo::Bingo(){
    size = ofGetWindowSize();
    
    bingoSwirl[0].load("bingo_swirl.png");
    bingoSwirl[1].load("bingo_swirlMirror.png");
    bingoBase.load("bingoWheel.png");
    
    ofLog(OF_LOG_NOTICE, "Loading Helvatica-Bold, 100px");
    countFont.load("Helvetica-Bold.ttf", 100);
        
    box2d.init();
    box2d.setGravity(0, 10);
    box2d.createGround();
    box2d.setFPS(ofGetTargetFrameRate());
//    box2d.registerGrabbing();
    
    // make the shape
    edgeLine.setPhysics(0.0, 0.5, 0.5);
    edgeLine.create(box2d.getWorld());
    
    for(int i=0; i<360; i++){ // Draw a circle as a container
        glm::vec2 pos;
        pos = (size*0.5) + glm::vec2(sin(ofDegToRad(i)), cos(ofDegToRad(i))) * 262;
        drawing.addVertex(pos.x, pos.y);
    }
    drawing.setClosed(false);
    drawing.simplify();
    
    edgeLine.addVertexes(drawing);
    //polyLine.simplifyToMaxVerts(); // this is based on the max box2d verts
    edgeLine.setPhysics(0.0, 0.5, 0.5);
    edgeLine.create(box2d.getWorld());
        
    drawing.clear();
    
    bingoFont.load("Geneva bold.ttf", 40);
    ofSetCircleResolution(64);
    
    reInit();
}

void Bingo::update(){
    box2d.update();
    
    glm::vec2 rolling = glm::vec2(cos(ofGetFrameNum()*0.05) * 30 * ofNoise(ofGetFrameNum()*0.1), sin(ofGetFrameNum() * 0.05) * 50 * ofNoise(ofGetFrameNum()*0.1 + 100));
    glm::vec2 standard = glm::vec2(0, 10);
//    cout << pct << endl;
    glm::vec2 f = (standard * bingoForce) + (rolling * (1.0-bingoForce));
//    cout << f << endl;
    box2d.setGravity(f);
}

void Bingo::display(DrawMode mode){
    // Mode 3: display all layers, mode 2, display 1st 2 layers
    switch(mode){ // Background
        case 0:{
            int dir[2] = {1, -1};
            for(int i=0; i<2; i++){
                ofPushMatrix();
                ofTranslate(size * 0.5);
                if(bRotateBingo)
                    ofRotateDeg((ofGetFrameNum()-rotStart) * 0.05 * dir[i]);
                bingoSwirl[i].draw(bingoSwirl[i].getWidth()*-0.5,bingoSwirl[i].getHeight()*-0.5);
                ofPopMatrix();
            }
        }
            break;
        case WHEEL:{
            bingoBase.draw(0,0, size.x, size.y);
            int i=0;
            for(auto &circle : circles) {
                ofFill();
                circle->draw();
                ofSetColor(255);
                ofPushMatrix();
                ofTranslate(circle->getPosition().x, circle->getPosition().y);
                ofRotateDeg(circle->getRotation());
                ofDrawCircle(0,0,circle->getRadius());
                ofSetColor(0);
                string num = circle->name;
                bingoFont.drawString(num, bingoFont.getStringBoundingBox(num, 0, 0).getWidth()*-0.5, bingoFont.getStringBoundingBox(num, 0, 0).getHeight()*0.5);
                ofPopMatrix();
                i++;
            }
            edgeLine.draw();
        }
            break;
        case NUMBER:{
            if(count >= 0){
                ofFill();
                ofSetColor(255);
                ofDrawCircle(ofGetWindowSize() * 0.5, ofGetWindowSize()[1] * 0.25);
                ofSetColor(0);
                ofRectangle r = countFont.getStringBoundingBox(ofToString((int)floor(count)), 0, 0);
                countFont.drawString(ofToString((int)floor(count)), ofGetWidth()*0.5 - r.getWidth() * 0.5, ofGetHeight()*0.5 + r.getHeight() * 0.5);
            }
        }
            break;
        case BACKGROUND_AND_WHEEL:
            display(DrawMode::BACKGROUND);
            display(DrawMode::WHEEL);
            break;
    }
}

void Bingo::display(DrawMode a, DrawMode b){
    display(a);
    display(b);
}

void Bingo::display(DrawMode a, DrawMode b, DrawMode c){
    display(a, b);
    display(c);
}

void Bingo::removeBall(string name){
    for(int i=0; i<circles.size(); i++){
        if(circles[i]->name == name){
            circles.erase(circles.begin() + i);
            break;
        }
    }
}

void Bingo::reInit(){
    circles.clear();
    for(int i=0; i<9; i++){ // Create balls
        auto circle = make_shared<ofxBox2dCircle>();
        circle->setPhysics(3.0, 0.53, 0.1);
        circle->setup(box2d.getWorld(), (size.x*0.5) + ofRandom(-50, 50), (size.y*0.5) + ofRandom(-50, 50), 40);
        circle->name = ofToString(i+1);
        circles.push_back(circle);
    }
}
