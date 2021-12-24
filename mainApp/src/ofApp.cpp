#include "ofApp.h"

#define WIDTH   1920
#define HEIGHT  1080
//
//--------------------------------------------------------------
void ofApp::setup(){
    state = Intro;
    ofSetFrameRate(30);
    
    /*
    jonisk.load("joniskLayer.png");
    joniskBig.load("joniskBig.png");
    gradient.load("gradient2.png");
    welcomTxt.load("welkomTxt.png");
    benzine.load("benzine.png");
    captainPicto.load("captainPicto.png");
//    scale.load("scale.png");
    
    routeStartEnd.load("route/TXT Aarde _ Planet Bi_1920x1080.png");
    lineGray.load("route/route dashed GRIJS_1920x1080.png");
    planetsGray.load("route/tussenplaneten GRIJS_1920x1080.png");
    legenda.load("route/legenda_1920x1080.png");
    joniskRoute.load("route/jonisk_2.png");
    joniskRouteGlow.load("route/jonisk_s Outer Glow_2.png");
    lineWhite.load("route/route dashed WIT_1920x1080.png");
    planetNames.load("route/TXT planeetnamen GRIJS_1920x1080.png");
    
    bingoSwirl[0].load("bingo_swirl.png");
    bingoSwirl[1].load("bingo_swirlM.png");
    bingoBase.load("bingoBase.png");
    
    codeTxt.load("codeTxt.png");
    codeCircle.load("codeCircle.png");
    codeGlow.load("codeGlow2.png");
    
    wordtVervolgd.load("wordtVervolgd.png");
    planet.load("planet.png");
    
    codeFont.load("Helvetica-Bold.ttf", 60);
    
    autoPilot.load("loading.png");
    autoPilotFont.load("Geneva Normal.ttf", 36);
    
    ofSetWindowShape(WIDTH, HEIGHT);
//    ofSetFullscreen(true);
    
    martian[0].load("martian/0/" + ofToString(0) + ".png");
//    stars[0].load("stars/" + ofToString(0) + ".mov");
    
    for(int i=0; i<3; i++){
        commercial[i].load("commercial/" + ofToString(i) + ".mp4");
        commercial[i].setLoopState(ofLoopType::OF_LOOP_NONE);
    }
    for(int i=0; i<7; i++){
        instructions[i].load("instructions/" + ofToString(i) + ".png");
    }
    for(int i=0; i<4; i++){
        returnImages[i].load("return/" + ofToString(i) + ".png");
    }
    
    countFont.load("Helvetica-Bold.ttf", 200);
    helveticaBold.load("Helvetica-Bold.ttf", 22);
    helveticaRegular.load("Helvetica.ttf", 22);
    
    instructionsID = 1;
    
    id = 0;
    
//    stars[0].play();
    
    dest = glm::vec2(714, 437);
    
    initBingo();
    initStars();
     */
    ofHideCursor();
}
//
//--------------------------------------------------------------
void ofApp::update(){
//    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(m);
        processMsg(m);
    }
    
    switch(state){
        case Commercial0:
        case Commercial1:
        case Commercial2:{
            for(int i=0; i<3; i++)
                commercial[i].update();
        }
            break;
        case StarsVideo:
        case Route:{
//            for(int i=0; i<1; i++)
//                stars[i].update();
            for(int i=0; i<stars.size(); i++){
                stars[i]->update(speed);
            }
        }
            break;
        case Bingo:{
            box2d.update();
            
            glm::vec2 rolling = glm::vec2(cos(ofGetFrameNum()*0.05) * 30 * ofNoise(ofGetFrameNum()*0.1), sin(ofGetFrameNum() * 0.05) * 50 * ofNoise(ofGetFrameNum()*0.1 + 100));
            glm::vec2 standard = glm::vec2(0, 10);
        //    cout << pct << endl;
            glm::vec2 f = (standard * bingoForce) + (rolling * (1.0-bingoForce));
        //    cout << f << endl;
            box2d.setGravity(f);
        }
        default:
            break;
    }
//    pct += 0.005;
//    if(pct > 1)
//        pct = 0;
    joniskRoutePos = glm::normalize(dest - start) * glm::distance(dest, start) * pct + start;
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(bRedBg){
        ofBackground(ofColor(248, 118, 102));
    } else{
        ofBackground(bgColor);
    }
    ofPushMatrix();
        ofTranslate(-ofGetWidth(), -ofGetHeight());
        ofRotate(180, 0, 0, 1);
        ofSetColor(255);
        ofDrawBitmapString("TIYCS", 100, 100);
    ofPopMatrix();
    switch(state){
        case Intro:
//            joniskHover();
//            welcomTxt.draw(0,0);
            break;
        case Benzine:
            benzine.draw(0,0);
            ofPushMatrix();
            ofPushStyle();
                ofSetLineWidth(215);
                ofTranslate(959, 705);
                ofRotateDeg(180 + 17.5 + meterAngle + ofNoise((float)(ofGetFrameNum()/50.) * 10));
                ofDrawLine(0, 0, 340, 0);
            ofPopStyle();
            ofPopMatrix();
            break;
        case MartianWindow:
            martian[id].draw(0,0,WIDTH,martian[id].getHeight()*(WIDTH/martian[id].getWidth()));
            break;
        case Commercial0:{
            if(commercial[0].isPlaying())
                commercial[0].draw(0,0);
        }
            break;
        case Commercial1: {
            if(commercial[1].isPlaying())
                commercial[1].draw(0,0);
        }
            break;
        case Commercial2: {
            if(commercial[2].isPlaying())
                commercial[2].draw(0,0);
        }
            break;
        case Waveform:{
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
        case Countdown:{
            if(count >= 0){
                ofRectangle r = countFont.getStringBoundingBox(ofToString((int)floor(count)), 0, 0);
                countFont.drawString(ofToString((int)floor(count)), ofGetWidth()*0.5 - r.getWidth() * 0.5, ofGetHeight()*0.5 + r.getHeight() * 0.5);
            }
        }
            break;
        case StarsVideo:{
            ofPushMatrix();
            ofTranslate(0, ofGetHeight() * height);
//            stars[id].draw(0,ofGetHeight()*-2);
            drawStars();
            
            planet.draw(0,0);
            ofPopMatrix;
        }
            break;
        case CaptainPicto:
            captainPicto.draw(0,0);
            break;
        case JoniskBig:
            joniskBig.draw(0,0);
            break;
        case Instructions:
            ofPushMatrix();
                ofTranslate(ofGetWindowSize() * 0.5);
                if(instructionsID <= 6)
                    instructions[instructionsID].draw(instructions[instructionsID].getWidth() * -0.5, instructions[instructionsID].getHeight() * -0.5);
            ofPopMatrix();
            break;
        case Route:
            ofPushMatrix();
            ofTranslate(0, ofGetHeight() * height);
            drawStars();
            ofPopMatrix();
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
        case Bingo:{
            int dir[2] = {1, -1};
            for(int i=0; i<2; i++){
                ofPushMatrix();
                ofTranslate(960, 665);
                if(bRotateBingo)
                    ofRotateDeg((ofGetFrameNum()-rotStart) * 0.05 * dir[i]);
                bingoSwirl[i].draw(bingoSwirl[i].getWidth()*-0.5,bingoSwirl[i].getHeight()*-0.5);
                ofPopMatrix();
            }
            bingoBase.draw(0,0);
            int i=0;
            for(auto &circle : circles) {
                ofFill();
                circle->draw();
                ofSetColor(255);
                ofPushMatrix();
                ofTranslate(circle->getPosition().x, circle->getPosition().y);
                ofRotate(circle->getRotation());
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
        case Code:{
            ofPushMatrix();
            ofTranslate(ofGetWindowSize()*0.5);
            ofScale(1.4);
            ofTranslate(ofGetWindowSize()*-0.5);
            codeTxt.draw(0,0);
            int m = 60; // margin
            for(int i=0; i<4; i++){
                ofPushMatrix();
                ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5 + 50);
                ofTranslate(((codeCircle.getWidth() + m)*0.5) + (i-2) * (codeCircle.getWidth() + m), 0);
                
                if(codeLen-1 >= i){
                    ofPushMatrix();
//                    ofScale(1.1);
                    ofSetColor(255);
                    codeGlow.draw(codeGlow.getWidth()*-0.5, codeGlow.getHeight()*-0.5);
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
                    codeCircle.draw(codeCircle.getWidth()*-0.5, codeCircle.getHeight()*-0.5);
                }
                ofPopMatrix();
            }
            ofPopMatrix();
        }
            break;
        case Autopilot:{
            ofSetColor(255); ofFill();
            ofDrawRectangle(624, 730, (autoPilotPct/100.)*(1296-624), ofGetHeight());
            autoPilot.draw(0,0, ofGetWidth(), ofGetHeight());
            joniskHover();
            string txt = "Starten automatische piloot...";
            ofRectangle r = autoPilotFont.getStringBoundingBox(txt, 0, 0);
            autoPilotFont.drawString(txt, ofGetWidth() * 0.5 - (r.getWidth()*0.5), ofGetHeight() * 0.625);
            if(autoPilotPct <= 10){
                string str = " " + ofToString((int)autoPilotPct) + "%";
//                ofRectangle box = autoPilotFont.getStringBoundingBox(str, 0, 0);
                autoPilotFont.drawString(str, ofGetWidth() * 0.5 - 20, ofGetHeight() * 0.85);
            } else{
                string str = ofToString((int)autoPilotPct) + "%";
                autoPilotFont.drawString(ofToString((int)autoPilotPct) + "%", ofGetWidth() * 0.5 - 20, ofGetHeight() * 0.85);
            }
        }
            break;
        case Return:{
            if(returnImageIndex < 4)
                returnImages[returnImageIndex].draw(0,0, ofGetWidth(), ofGetHeight());
        }
            break;
        case BingoBall:{
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
        case WordtVervolgd:
            wordtVervolgd.draw(0,0);
            break;
        case BingoBg:
            int dir[2] = {1, -1};
            for(int i=0; i<2; i++){
                ofPushMatrix();
                ofTranslate(960, 665);
                if(bRotateBingo)
                    ofRotateDeg((ofGetFrameNum()-rotStart) * 0.05 * dir[i]);
                bingoSwirl[i].draw(bingoSwirl[i].getWidth()*-0.5,bingoSwirl[i].getHeight()*-0.5);
                ofPopMatrix();
            }
            break;
    }
    if(bVluchtInfo){
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
    ofSetColor(0, 255-brightness);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
}

void ofApp::processMsg(ofxOscMessage &m){
    if(m.getAddress() == "/setMode"){
        state = (State)m.getArgAsInt(0);
    } else if(m.getAddress() == "/eventById"){
        switch(m.getArgAsInt(0)){
            case 0:
                count = 10;
                bCountDown = true;
                break;
            case 1:{
                int movieID = m.getArgAsInt(1);
                for(int i=0; i<3; i++){
                    commercial[i].stop();
                    commercial[i].firstFrame();
                }
                commercial[movieID].play();
                }
                break;
            case 2:{
                for(int i=0; i<circles.size(); i++){
                    if(circles[i]->name == ofToString(m.getArgAsInt(1))){
                        circles.erase(circles.begin() + i);
                        break;
                    }
                }
            }
                break;
            }
    } else if(m.getAddress() == "/setValueById"){
        switch(m.getArgAsInt(0)){
            case 0:
                count = m.getArgAsFloat(1);
                break;
            case 1:
                meterAngle = m.getArgAsFloat(1);
                break;
            case 2:
                brightness = m.getArgAsFloat(1);
                break;
            case 3:
                instructionsID = m.getArgAsInt(1);
                break;
            case 4:
                bVluchtInfo = m.getArgAsBool(1);
                break;
            case 5:
                flightInfo.height = m.getArgAsFloat(1);
                flightInfo.speed = m.getArgAsFloat(2);
                flightInfo.temp = m.getArgAsFloat(3);
                break;
            case 6:
                codeLen = m.getArgAsInt(1);
                break;
            case 7:
                autoPilotPct = m.getArgAsInt(1);
                break;
            case 8:
                pct = m.getArgAsFloat(1);
                break;
            case 9:
                bingoForce = m.getArgAsFloat(1);
                break;
            case 10:
                bRedBg = m.getArgAsBool(1);
                break;
            case 11:
                returnImageIndex = m.getArgAsInt(1);
                break;
            case 12:
                welcomIntensity = m.getArgAsFloat(1);
                break;
            case 13:
                height = m.getArgAsFloat(1);
                break;
            case 14:
                speed = m.getArgAsFloat(1);
                break;
            case 15:
                bRotateBingo = m.getArgAsBool(1);
                rotStart = ofGetFrameNum();
                break;
        }
    } else if (m.getAddress() == "/setId"){
        id = m.getArgAsInt(0);
    } else if(m.getAddress() == "/setWave"){
        waveForm.clear();
        for(int i=0; i<m.getNumArgs(); i++){
            waveForm.push_back(m.getArgAsFloat(i));
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
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    cout << x << " " << y << endl;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::initBingo(){
    bMouseForce = false;
    
    box2d.init();
    box2d.setGravity(0, 10);
    box2d.createGround();
    box2d.setFPS(30.0);
//    box2d.registerGrabbing();
    
    // make the shape
    edgeLine.setPhysics(0.0, 0.5, 0.5);
    edgeLine.create(box2d.getWorld());
    
    for(int i=0; i<360; i++){
        glm::vec2 pos;
        pos = glm::vec2(960, 665) + glm::vec2(sin(ofDegToRad(i)), cos(ofDegToRad(i))) * 262;
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
    
    for(int i=0; i<9; i++){
        auto circle = make_shared<ofxBox2dCircle>();
        circle->setPhysics(3.0, 0.53, 0.1);
        circle->setup(box2d.getWorld(), 960 + ofRandom(-50, 50), 665 + ofRandom(-50, 50), 40);
        circle->name = ofToString(i+1);
        circles.push_back(circle);
    }
}

void ofApp::joniskHover(){
    ofPushMatrix();
        ofTranslate(0, pow(sin(ofGetFrameNum()/50.), 2.) * 30 * welcomIntensity);
        
        ofSetColor(255, 100);
            ofPushMatrix(); // Note: the zoom is not fixed to jonisk center
                ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.25);
                ofScale(sin(ofGetFrameNum()/50.) * (0.1 * 1.) + 1.);
                gradient.draw(-ofGetWidth()*0.5, -ofGetHeight()*0.25);
            ofPopMatrix();
        ofSetColor(255);
        jonisk.draw(0,0);
    ofPopMatrix();
}

void ofApp::setOsc(int port){
    receiver.setup(port);
    ofSetWindowTitle(ofToString(port));
}

void ofApp::setCommercialAmp(float amp){
    for(int i=0; i<3; i++){
        commercial[i].setVolume(amp);
    }
}

void ofApp::initStars(){
    for(int i=0; i<500; i++){
        stars.push_back(new Star());
        stars.back()->setLoc(
                             glm::vec3(ofGetWidth() * ofRandom(-1, 2),
                                    ofRandom(ofGetHeight()), -ofRandom(500, 1500)), true);
    }
//    ofSetSphereResolution(180);
    
    // Setup post-processing chain
//    post.init(ofGetWidth(), ofGetHeight());
//    post.createPass<FxaaPass>()->setEnabled(false);
//    bloom = post.createPass<BloomPass>();
//    bloom->setEnabled(true);
    
    starsFbo.allocate(WIDTH, HEIGHT, GL_RGB);
}

void ofApp::drawStars(){
    starsFbo.begin();
//    post.begin();
    
    ofSetColor(255);
    ofFill();
    for(int i=0; i<stars.size(); i++){
        stars[i]->display();
    }
    ofSetColor(0, pow(1-(height / 2), 0.5) * 200);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
//    post.end();
    starsFbo.end();
    
    
    ofSetColor(255, brightness);
    starsFbo.draw(0, ofGetHeight()*-2);
}
