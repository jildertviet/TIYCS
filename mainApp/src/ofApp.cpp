#include "ofApp.h"

#define WIDTH   1280
#define HEIGHT  800
// Width an height are also set in main.cpp

#ifdef  TARGET_RASPBERRY_PI
    bool bRotate = true;
#else
    bool bRotate = false;
#endif

//--------------------------------------------------------------
void ofApp::setup(){
    scene = scenes::Stars;
    ofSetFrameRate(30);
    ofSetWindowShape(WIDTH, HEIGHT);
    
    memset(busses, 0, sizeof(float)*NUM_BUSSES); // Set busses to 0.
    
    bingo = new Bingo();
    
    stars = new Stars(glm::vec2(WIDTH, HEIGHT));
    stars->height = &busses[0];
    stars->travelSpeed = &busses[1];
    
    // IMAGES
    images["Intro Jonisk"] =    ofImage("joniskLayer.png");
    images["Intro gradient"] =  ofImage("gradient2.png");
    images["Jonisk big"] =      ofImage("joniskBig.png");
    images["Benzine"] =         ofImage("benzine.png");
    images["CodeTxt"] =         ofImage("codeTxt.png");
    images["Code Circle"] =     ofImage("codeCircle.png");
    images["Code Glow"] =       ofImage("codeGlow2.png");
    images["Autopilot"] =       ofImage("loading.png");
    images["Captain"] =         ofImage("captainPicto.png");

    for(int i=0; i<7; i++){
        instructions[i].load("instructions/" + ofToString(i) + ".png");
    }
    for(int i=0; i<4; i++){
        returnImages[i].load("return/" + ofToString(i) + ".png");
    }
    
    // FONTS
    countFont.load("Helvetica-Bold.ttf", 200);
    codeFont.load("Helvetica-Bold.ttf", 60);
    autoPilotFont.load("Geneva Normal.ttf", 36);
//    helveticaBold.load("Helvetica-Bold.ttf", 22); // Overlay
//    helveticaRegular.load("Helvetica.ttf", 22);
    
    // VIDEOS
//#ifdef TARGET_RASPBERRY_PI
//    for(int i=0; i<3; i++){
        
//    }
//#else
//        commercial[i].load("commercial/" + ofToString(i) + ".mp4");
//        commercial[i].setLoopState(ofLoopType::OF_LOOP_NONE);
//#endif
    
    /*
    welcomTxt.load("welkomTxt.png");
    
    routeStartEnd.load("route/TXT Aarde _ Planet Bi_1920x1080.png");
    lineGray.load("route/route dashed GRIJS_1920x1080.png");
    planetsGray.load("route/tussenplaneten GRIJS_1920x1080.png");
    legenda.load("route/legenda_1920x1080.png");
    joniskRoute.load("route/jonisk_2.png");
    joniskRouteGlow.load("route/jonisk_s Outer Glow_2.png");
    lineWhite.load("route/route dashed WIT_1920x1080.png");
    planetNames.load("route/TXT planeetnamen GRIJS_1920x1080.png");
        
    id = 0;

    dest = glm::vec2(714, 437);
     */
//    ofHideCursor();
    
    v = new ofxJVisuals(glm::vec2(WIDTH, HEIGHT));
    receiver.setup(5000);
}
//
//--------------------------------------------------------------
void ofApp::update(){
    v->update();

//    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(m);
        processMsg(m);
    }
    
    switch(scene){
        case scenes::Commercial:{
#ifndef TARGET_RASPBERRY_PI
            commercial.update();
#endif
        }
            break;
        case scenes::Stars:
        case scenes::Route:
            stars->update();
            break;
        case scenes::Bingo:{
            bingo->bingoForce = busses[1];
            bingo->update();
        }
        default:
            break;
    }
    
    joniskRoutePos = glm::normalize(dest - start) * glm::distance(dest, start) * routePct + start;
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(bRotate){
        ofPushMatrix();
            ofTranslate(ofGetWidth(), ofGetHeight());
            ofRotateDeg(180, 0, 0, 1);
    }
    
    ofBackground(0);
    if(busses[2] != 0){
        ofSetColor(255, 0, 0, busses[2] * 255);
        ofDrawRectangle(0, 0, WIDTH, HEIGHT);
    }
    
    ofSetColor(255);
    v->display();
    
    switch(scene){
        case scenes::Nothing:
            break;
        case scenes::Intro:
            joniskHover();
//            welcomTxt.draw(0,0);
            break;
        case scenes::JoniskBig:
            images["Jonisk big"].draw(0,0);
            break;
        case scenes::Instructions:{
            int imageID = busses[0];
            ofPushMatrix();
                ofTranslate(ofGetWindowSize() * 0.5);
                if(imageID <= 6)
                    instructions[imageID].draw(instructions[imageID].getWidth() * -0.5, instructions[imageID].getHeight() * -0.5);
            ofPopMatrix();
        }
            break;
        case scenes::Countdown:{
            int count = busses[0];
            if(count > 0){
                ofRectangle r = countFont.getStringBoundingBox(ofToString((int)floor(count)), 0, 0);
                countFont.drawString(ofToString((int)floor(count)), ofGetWidth()*0.5 - r.getWidth() * 0.5, ofGetHeight()*0.5 + r.getHeight() * 0.5);
            }
        }
            break;
        case scenes::Stars:
            ofPushMatrix();
            ofTranslate(busses[2], 0);
            stars->display(brightness);
            ofPopMatrix();
            break;
        case scenes::Route:
            stars->display(brightness);
            
            ofSetColor(0, 100);
            ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
            
            ofSetColor(255);
            lineGray.draw(0,0);
            lineWhite.drawSubsection(0, 0, joniskRoutePos.x, HEIGHT, 0, 0);
            routeStartEnd.draw(0,0);
            planetNames.draw(0,0);
            planetsGray.draw(0,0);
            
//            joniskRoutePos
            ofPushMatrix();
            ofTranslate(joniskRoutePos);
            ofScale((sin(ofGetFrameNum() * 0.02) * 0.2) + 1.2);
            joniskRouteGlow.draw(glm::vec2(joniskRouteGlow.getWidth() * -0.5, joniskRouteGlow.getHeight() * -0.5));
            ofPopMatrix();
            joniskRoute.draw(joniskRoutePos - glm::vec2(joniskRoute.getWidth()*0.5, joniskRoute.getHeight() * 0.5));
            break;
        case scenes::Commercial:{
            if(commercial.isPlaying())
                commercial.draw(0,0, ofGetWidth(), ofGetHeight());
        }
            break;
        case scenes::Benzine:
            images["Benzine"].draw(0,0);
            ofPushMatrix();
            ofPushStyle();
                ofSetLineWidth(215);
                ofTranslate(959, 705);
                ofRotateDeg(180 + 17.5 + busses[0] + ofNoise((float)(ofGetFrameNum()/50.) * 10));
                ofDrawLine(0, 0, 340, 0);
            ofPopStyle();
            ofPopMatrix();
            break;
        case scenes::ReturnToShip:{
            int imgIndex = busses[0];
            if(imgIndex < 4){
                returnImages[imgIndex].draw(0,0, ofGetWidth(), ofGetHeight());
            }
        }
            break;
        case scenes::CaptainPicto:
            images["Captain"].draw(0,0);
            break;
        case scenes::Waveform:{
            ofSetColor(255);
            ofSetLineWidth(3);
            ofNoFill();
            ofBeginShape();
            for (unsigned int i = 0; i < waveForm.size(); i++){
                ofVertex(
                         (i*((float)ofGetWidth()/waveForm.size())),
                         waveForm[i]
                         );
            }
            ofEndShape(false);
//            captainPicto.draw(0,0, ofGetWindowWidth() * 0.25, ofGetWindowHeight() * 0.25);
        }
            break;
        case scenes::Scene::Bingo:
            bingo->display((Bingo::DrawMode)busses[0]);
            break;
        case scenes::Code:{
            ofPushMatrix();
            ofTranslate(ofGetWindowSize()*0.5);
            ofScale(1.4);
            ofTranslate(ofGetWindowSize()*-0.5);
            images["CodeTxt"].draw(0,0, WIDTH, HEIGHT);
            int m = 60; // margin
            for(int i=0; i<4; i++){
                ofPushMatrix();
                ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5 + 50);
                ofTranslate(((images["Code Circle"].getWidth() + m)*0.5) + (i-2) * (images["Code Circle"].getWidth() + m), 0);
                
                if(busses[0]-1 >= i){
                    ofPushMatrix();
//                    ofScale(1.1);
                    ofSetColor(255);
                    images["Code Glow"].draw(images["Code Glow"].getWidth()*-0.5, images["Code Glow"].getHeight()*-0.5);
//                    ofScale(1/1.1);
                    ofPopMatrix();
                    
                    ofSetColor(38);
                    ofDrawCircle(0,0,70);
                    ofSetColor(255);
                    ofDrawCircle(0,0,50);
                    ofSetColor(0);
                    string txt = ofToString(code[i]);
                    codeFont.drawString(
                                         txt,
                                         codeFont.getStringBoundingBox(txt, 0, 0).getWidth()*-0.5,
                                         codeFont.getStringBoundingBox(txt, 0, 0).getHeight()*0.5
                                         );
                } else{
                    ofSetColor(255);
                    ofRotateDeg(ofGetFrameNum()*0.2);
                    images["Code Circle"].draw(images["Code Circle"].getWidth()*-0.5, images["Code Circle"].getHeight()*-0.5);
                }
                ofPopMatrix();
            }
            ofPopMatrix();
        }
            break;
        case scenes::Autopilot:{
            ofSetColor(255); ofFill();
            ofDrawRectangle(416, 536, (busses[1]/100.)*(866-416), ofGetHeight());
            images["Autopilot"].draw(0,0, ofGetWidth(), ofGetHeight());
            joniskHover();
            string txt = "Starten automatische piloot...";
            ofRectangle r = autoPilotFont.getStringBoundingBox(txt, 0, 0);
            autoPilotFont.drawString(txt, ofGetWidth() * 0.5 - (r.getWidth()*0.5), ofGetHeight() * 0.625);
            if(busses[0] <= 10){
                string str = " " + ofToString((int)busses[1]) + "%";
//                ofRectangle box = autoPilotFont.getStringBoundingBox(str, 0, 0);
                autoPilotFont.drawString(str, ofGetWidth() * 0.5 - 20, ofGetHeight() * 0.85);
            } else{
                string str = ofToString((int)busses[0]) + "%";
                autoPilotFont.drawString(ofToString((int)busses[1]) + "%", ofGetWidth() * 0.5 - 20, ofGetHeight() * 0.85);
            }
        }
            break;
    }
    
    if(bVluchtInfo){ // Overlay
        vector<string> names = {"Hoogte", "Snelheid", "Buitentemperatuur"};
        vector<string> units = {" km", " ziljard km/u", "°C"};
        vector<string> values = {ofToString(flightInfo.height), ofToString(flightInfo.speed), ofToString(flightInfo.temp)};
        for(int i=0; i<3; i++){
            helveticaBold.drawString(names[i], 120 + (i*300), ofGetWindowHeight() - 70);
            helveticaRegular.drawString(values[i] + units[i], 120 + (i*300), ofGetWindowHeight() - 30);
        }
        legenda.draw(0,0);
    }
    
    ofFill();
    ofSetColor(0, 255-brightness); // This line makes all black, even in the next line is commented :/
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
    if(bRotate){
        ofPopMatrix();
    }
}

void ofApp::processMsg(ofxOscMessage &m){
    if(m.getAddress() == "/setScene"){
        scene = (scenes::Scene)m.getArgAsInt(0);
    } else if(m.getAddress() == "/eventById"){
        switch(m.getArgAsInt(0)){
            case 1:{
                int movieID = m.getArgAsInt(1);
#ifdef TARGET_RASPBERRY_PI
                string videoPath = ofToDataPath("commercial/" + ofToString(movieID) + ".mp4", true);
                ofxOMXPlayerSettings settings;
                settings.videoPath = videoPath;
                settings.enableTexture = false;
                settings.enableLooping = false;
                settings.enableAudio = false;
                    
                commercial.setup(settings);
                commercial.setPaused(false);
#else
                commercial.load("commercial/" + ofToString(movieID) + ".mp4");
                commercial.setLoopState(ofLoopType::OF_LOOP_NONE);
                commercial.play();
#endif

                if(movieID == -1){
#ifdef TARGET_RASPBERRY_PI
                    commercial.setPaused(true);
                    commercial.close();
#else
                    commercial.stop();

#endif
                }
            }
                break;
            case 2:
                bingo->removeBall(ofToString(m.getArgAsInt(1)));
                break;
            case 3:
                bingo->reInit();
                break;
            }
    } else if(m.getAddress() == "/setValueById"){
        switch(m.getArgAsInt(0)){
            case 2:
                brightness = m.getArgAsFloat(1);
                break;
            case 4:
                bVluchtInfo = m.getArgAsBool(1);
                break;
            case 5:
                flightInfo.height = m.getArgAsFloat(1);
                flightInfo.speed = m.getArgAsFloat(2);
                flightInfo.temp = m.getArgAsFloat(3);
                break;
            case 8:
                routePct = m.getArgAsFloat(1);
                break;
            case 15:
                bingo->bRotateBingo = m.getArgAsBool(1);
                bingo->rotStart = ofGetFrameNum();
                break;
        }
    } else if (m.getAddress() == "/setId"){
        id = m.getArgAsInt(0);
    } else if(m.getAddress() == "/setWave"){
        waveForm.clear();
        for(int i=0; i<m.getNumArgs(); i++){
            waveForm.push_back(m.getArgAsFloat(i));
        }
    } else if(m.getAddress() == "/setBus"){
        if(m.getArgAsInt(0) < NUM_BUSSES){
            busses[m.getArgAsInt(0)] = m.getArgAsFloat(1);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case 'f':
            ofSetFullscreen(true);
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    cout << x << " " << y << endl;
}


//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}

void ofApp::joniskHover(){
    ofPushMatrix();
        ofTranslate(0, pow(sin(ofGetFrameNum()/50.), 2.) * 30 * busses[0]);
        
        ofSetColor(255, 100);
            ofPushMatrix(); // Note: the zoom is not fixed to jonisk center
                ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.25);
                ofScale(sin(ofGetFrameNum()/50.) * (0.1 * 1.) + 1.);
                images["Intro gradient"].draw(-ofGetWidth()*0.5, -ofGetHeight()*0.25, WIDTH, HEIGHT);
            ofPopMatrix();
        ofSetColor(255);
        images["Intro Jonisk"].draw(0,0, WIDTH, HEIGHT);
    ofPopMatrix();
}
