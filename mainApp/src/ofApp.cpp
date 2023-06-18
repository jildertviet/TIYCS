#include "ofApp.h"

float windowScaler = 0.5;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);

    ofSetWindowShape(1280*3 * windowScaler, 800 * windowScaler);

#ifdef  TARGET_RASPBERRY_PI
    ofHideCursor();
#endif
//    ofHideCursor();

    // Shared resources
    commercials = new ofVideoPlayer[3];
    for(int i=0; i<3; i++){
        commercials[i].load("1280/commercial/" + ofToString(i) + ".mp4"); // Hard coded prefix :(
        commercials[i].setLoopState(ofLoopType::OF_LOOP_NONE);
        commercials[i].setVolume(0); // Audio off
    }

    for(int i=0; i<3; i++){
        screens[i].setup(i, glm::vec2(1280 * windowScaler, 800 * windowScaler), &screenOrder[0]);
        screens[i].commercials = commercials;
        screens[i].mappableFbo.name = "FBO_" + ofToString(i);
        screens[i].piMapper = &piMapper;
        piMapper.registerFboSource(screens[i].mappableFbo);
    }

    ofSetVerticalSync(false);
    blob.load("shadersGL2/shader");
    // screenOrderShader.load("shadersGL2/offsetShader");
    post.init(1024*4 * windowScaler, 1024 * windowScaler, false);
    post.createPass<BloomPass>()->setEnabled(true);
    post.createPass<GodRaysPass>()->setEnabled(true);
    light.setPosition(ofGetWidth()*0.5, ofGetHeight()*0.5, 4000);
    light.setPointLight();
    toReArrange.allocate(ofGetWidth(), ofGetHeight());
//    light.setAmbientColor(ofFloatColor::white);
//    light.setDiffuseColor(ofFloatColor(1.0, 0.5, 0.5));


    piMapper.setup();

}

//--------------------------------------------------------------
void ofApp::update(){
    for(int i=0; i<3; i++){
        screens[i].update();
    }
    piMapper.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofBackground(0);

    bool bRotate = false;
    if(screens[0].scene == scenes::StarsFinal){
        if(screens[0].brightness == 0){
            ofBackground(0);
            return;
        }
//        screenOrderShader.begin();
        toReArrange.begin();
        if(bRotate){
            ofPushMatrix();
                ofTranslate(ofGetWidth(), ofGetHeight());
                ofRotateDeg(180, 0, 0, 1);
        }
        float v = (float)(ofGetElapsedTimeMillis()) / 4000.;
        float v2 = (float)(1000 + ofGetElapsedTimeMillis()) / 4000.;

        post.setFlip(false);
        post.begin();
        ofEnableDepthTest();
        for(int i=0; i<3; i++){
            ofPushMatrix();
            ofTranslate(1280 * i * windowScaler, 0);
    //        screens[screenOrder[i]].draw();
            for(int j=0; j<screens[i].stars->stars.size(); j++){
//                screens[i].stars->update(glm::vec3(1.));
//                screens[i].stars->display();
                screens[i].stars->stars[j]->update(screens[0].busses[14]);
                screens[i].stars->stars[j]->display();
            }
            ofPopMatrix();
        }
        light.enable();
        blob.begin();
            blob.setUniform3f("iResolution", ofGetWidth(), ofGetHeight(), 0);
            blob.setUniform1f("iTime", ofGetElapsedTimeMillis() / 1000.);
            blob.setUniform4f("iMouse", ofGetMouseX(), ofGetMouseY(), 0, 0);
//            blob.setUniform2f("offset", (ofNoise(v)-0.5)*2.0, (ofNoise(v2)-0.5)*2.0);
            blob.setUniform2f("offset", screens[0].busses[10],screens[0].busses[11]);
            blob.setUniform1f("Radius", screens[0].busses[12]);
            blob.setUniform1f("NoiseAmplitude", screens[0].busses[13]);
    //        blob.setUniform2f("offset", 0,0);
            ofDrawRectangle(0,0,ofGetWidth(), ofGetHeight());
        blob.end();
        light.disable();
        post.end();
        if(bRotate)
            ofPopMatrix();
//        screenOrderShader.end();

//        ofDrawBitmapString("0", 10, 10);
//        ofDrawBitmapString("1", 10 + 1280, 10);
//        ofDrawBitmapString("2", 10 + (2*1280), 10);
        toReArrange.end();

        for(int i=0; i<3; i++){
            ofPushMatrix();
// //            ofRotateYDeg(180);
// //            ofTranslate(1280, 0);
            ofTranslate(1280 * screenOrder[2-i] * windowScaler, 0); // Reverse screenorder ...
            toReArrange.getTexture().drawSubsection(0, 0, 1280 * windowScaler, 800 * windowScaler, 1280 * i * windowScaler, 0);
            ofPopMatrix();
// //            draw(1280 * screenOrder[i], 0);
        }
    } else{
        for(int i=0; i<3; i++){
            ofPushMatrix();
            ofTranslate(1280*i * windowScaler, 0);
            screens[screenOrder[i]].draw(); // Write to FBO
            ofPopMatrix();
        }
    }
  ofSetColor(255);
  ofFill();
  piMapper.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case 'f':
            ofSetFullscreen(true);
            break;
    }
    // for(int i=0; i<3; i++){
        // screens[i].keyPressed(key);
    // }
    if(key == 't'){

      return;
    }
    piMapper.keyPressed(key);
}

void ofApp::keyReleased(int key){
	piMapper.keyReleased(key);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    // int i = floor(x / (1280. * windowScaler));
    // screens[i].mousePressed(x - (i * 1280 * windowScaler), y, button);
    piMapper.mousePressed(x, y, button);
}


void ofApp::mouseReleased(int x, int y, int button){
	piMapper.mouseReleased(x, y, button);
}

void ofApp::mouseDragged(int x, int y, int button){
	piMapper.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}
