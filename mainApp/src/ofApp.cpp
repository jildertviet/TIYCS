#include "ofApp.h"

string prefix = "1280/";

//--------------------------------------------------------------
void ofApp::setup(){
    scene = scenes::Test;
    ofSetFrameRate(60);
    ofSetWindowShape(width, height);
    renderFbo.allocate(width, height);
    initMesh();
    seed = ofRandom(TWO_PI);
    
    memset(busses, 0, sizeof(float)*NUM_BUSSES); // Set busses to 0.
    
    bingo = new Bingo(prefix);
    bingo->numberScale = &busses[3];
    
    stars = new Stars(glm::vec2(width, height), prefix);
    stars->height = &busses[0];
    stars->travelSpeed = &busses[1];
    stars->hOffset = &busses[6];
    stars->planetID = &busses[7];
    
    // IMAGES
    images["Intro Jonisk"] =    ofImage(prefix + "joniskLayer.png");
    images["Intro gradient"] =  ofImage(prefix + "gradient3.jpg");
    images["Jonisk big"] =      ofImage(prefix + "joniskBig.png");
    images["Benzine"] =         ofImage(prefix + "benzine.png");
    images["CodeTxt"] =         ofImage(prefix + "codeTxt.png");
    images["Code Circle"] =     ofImage(prefix + "codeCircle.png");
    images["Code Glow"] =       ofImage(prefix +"codeGlow2.png");
    images["Autopilot"] =       ofImage(prefix + "loading.png");
    images["Welcome"] =         ofImage(prefix + "welkomTxt.png");
    images["Einde"] =           ofImage(prefix + "einde.png");
    images["Shopping"] =        ofImage(prefix + "inflightshopping.png");
    
    images["incomingCall"] =    ofImage(prefix + "incomingCall.png");
    images["telephone"] =       ofImage(prefix + "telephone.png");
    images["duringCall"] =    ofImage(prefix + "duringCall.png");
    
    images["Motor"] =    ofImage(prefix + "motor.png");
    images["450-629"] =    ofImage(prefix + "450-629.png");
    images["847-212"] =    ofImage(prefix + "847-212.png");
    images["351-364"] =    ofImage(prefix + "351-364.png");
    images["983-376"] =    ofImage(prefix + "983-376.png");
    
    
    images["lineGray"] =    ofImage(prefix + "/overlay/lijn_grijs.png");
    images["lineWhite"] =    ofImage(prefix + "/overlay/lijn_wit.png");
    images["joniskOL"] =    ofImage(prefix + "/overlay/jonisk.png");
    images["glowOL"] =    ofImage(prefix + "/overlay/glow.png");

    for(int i=0; i<NUM_INSTRUCTIONS; i++){
        instructions[i].load(prefix + "instructions/" + ofToString(i) + ".png");
    }
    for(int i=0; i<8; i++){
        returnImages[i].load(prefix + "return/" + ofToString(i) + ".png");
    }
    
    if(windowScale != 1){
        auto iter = images.begin();
        while (iter != images.end()) {
            cout << iter->second.getWidth() << endl;;
            iter->second.resize(iter->second.getWidth() * windowScale, iter->second.getHeight() * windowScale);
            ++iter;
        }
        
        for(int i=0; i<NUM_INSTRUCTIONS; i++)
            instructions[i].resize(instructions[i].getWidth() * windowScale, instructions[i].getHeight() * windowScale);
        for(int i=0; i<NUM_RETURNIMAGES; i++)
            returnImages[i].resize(returnImages[i].getWidth() * windowScale, returnImages[i].getHeight() * windowScale);
    }
    
    // FONTS
    ofLog(OF_LOG_NOTICE, "Loading Helvetica-Bold, 100px");
    countFont.load("fonts/Helvetica-Bold.ttf", 100);
    ofLog(OF_LOG_NOTICE, "Loading Helvetica-Bold, 60px");
    codeFont.load("fonts/Helvetica-Bold.ttf", 60);
    
    // CRASH @ RBP
    ofLog(OF_LOG_NOTICE, "Loading Geneva Normal, 36px");
    autoPilotFont.load("fonts/Geneva Normal.ttf", 36);
    
    callCountFont.load("fonts/Geneva Normal.ttf", 25);
    
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
    
    lineGray.load(prefix + "/route/route dashed GRIJS.png");
    lineWhite.load(prefix + "/route/route dashed WIT.png");
    routeStartEnd.load(prefix + "/route/TXT Aarde _ Planet Bi.png");
    planetNames.load(prefix + "/route/TXT planeetnamen GRIJS.png");
    planetsGray.load(prefix + "/route/tussenplaneten GRIJS.png");

    //    legenda.load("route/legenda_1920x1080.png");
    joniskRoute.load(prefix + "/route/jonisk_2.png"); // Is scaled to 1280 res
    joniskRouteGlow.load(prefix + "/route/jonisk_s Outer Glow_2.png");
    
    for(int i=0; i<3; i++){
        commercials[i].load(prefix + "commercial/" + ofToString(i) + ".mp4");
        commercials[i].setLoopState(ofLoopType::OF_LOOP_NONE);
        commercials[i].setVolume(1-bMuteAudio); // True gives 1-1: 0, so it's muted
    }

    start = glm::vec2(307, 373) * windowScale;
    dest = glm::vec2(626, 438) * windowScale;
    
#ifdef  TARGET_RASPBERRY_PI
    ofHideCursor();
#endif
//    ofHideCursor();
    
    v = new ofxJVisuals(glm::vec2(width, height));
    receiver.setup(PORT + portNumAdd);
    
    ofSetFrameRate(60);
    ofSetVerticalSync(false);
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
    
    switch(scene){
        case scenes::Commercial:{
#ifndef TARGET_RASPBERRY_PI
            if(commercial)
                commercial->update();
#endif
        }
            break;
        case scenes::Stars:
        case scenes::Benzine:
        case scenes::Route:
            stars->update(glm::vec3(busses[3], busses[4], busses[5]));
            break;
        case scenes::Bingo:{
            bingo->bingoForce = busses[1];
            bingo->update();
        }
        default:
            break;
    }
    
    render(); // Writes to FBO, can be called recursively
}

void ofApp::render(scenes::Scene pseudo, bool bUseFbo){
    scenes::Scene sceneToUse = pseudo != scenes::Pseudo ? pseudo : scene; // Is update() called with a scene specified?

    if(bUseFbo){
        renderFbo.begin();
        
        if(bRotate){ // Don't rotate again when recursive render()
            ofPushMatrix();
                ofTranslate(ofGetWidth(), ofGetHeight());
                ofRotateDeg(180, 0, 0, 1);
        }
    }
        ofBackground(0);
        
        ofSetColor(255);
//        v->display();
        
        switch(sceneToUse){
            case scenes::Nothing:
                break;
            case scenes::Intro:
            {
                ofPushMatrix();
                ofTranslate(ofGetWidth()*0.5, 0);
                ofRotateYDeg(busses[1]);
                ofTranslate(ofGetWidth()*-0.5, 0);
                if(busses[1] > 90){
                    images["Welcome"].draw(0,0);
                } else{
                    joniskHover();
                }
                ofPopMatrix();
            }
                break;
            case scenes::JoniskBig:
                images["Jonisk big"].draw(0,0);
                break;
            case scenes::Instructions:{
//                int imageID = busses[0];
                for(int i=0; i<NUM_INSTRUCTIONS; i++){
                    ofPushMatrix();
                    ofTranslate(0, (instructions[0].getHeight()) * (i - busses[0]));
                        ofPushMatrix();
                            ofTranslate(ofGetWindowSize() * 0.5);
                                if(i <= (NUM_INSTRUCTIONS-1)){
                                    if(i > 0){
                                        ofTranslate((ofGetWidth() - instructions[i].getWidth()) * 0.5, (ofGetHeight() - instructions[i].getHeight()) * 0.5);
                                        ofScale(0.8);
                                        ofTranslate((ofGetWidth() - instructions[i].getWidth()) * -0.5, (ofGetHeight() - instructions[i].getHeight()) * -0.5);
                                    }
                                    instructions[i].draw(instructions[i].getWidth() * -0.5, instructions[i].getHeight() * -0.5);
                                }
                        ofPopMatrix();
                    ofPopMatrix();
                }
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
            case scenes::Stars:{
                ofPushMatrix();
                stars->display(brightness);
                ofPopMatrix();
                            
                if(busses[8]){
                    joniskRoutePos = glm::normalize(dest - start) * glm::distance(dest, start) * busses[9] + start;
        
                    ofSetColor(0, 100 * busses[8]);
                    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

                    ofSetColor(255, 255 * busses[8]);
                    
                    float widthRatio = ofGetWidth() / lineGray.getWidth();
                    float h = lineGray.getHeight() * widthRatio;
                    lineGray.draw(0,0, lineGray.getWidth() * (widthRatio), h);
                    lineWhite.drawSubsection(0, 0, joniskRoutePos.x * widthRatio, h, 0, 0);
                    
                    routeStartEnd.draw(0,0, lineGray.getWidth() * (widthRatio), h);
                    planetNames.draw(0,0, lineGray.getWidth() * (widthRatio), h);
                    planetsGray.draw(0,0, lineGray.getWidth() * (widthRatio), h);
        
                    ofPushMatrix();
                    ofTranslate(joniskRoutePos);
                    ofScale((sin(ofGetFrameNum() * 0.02) * 0.2) + 1.2);
                    joniskRouteGlow.draw(glm::vec2(joniskRouteGlow.getWidth() * -0.5, joniskRouteGlow.getHeight() * -0.5));
                    ofPopMatrix();
                    joniskRoute.draw(joniskRoutePos - glm::vec2(joniskRoute.getWidth()*0.5, joniskRoute.getHeight() * 0.5));
                }
                if(busses[9]){ // Dashboard overview
                    float hOffset = busses[6];
                    ofPushMatrix();
                    ofSetColor(255, busses[9]);
                    ofTranslate(ofGetWidth() * (hOffset), 0);
                    images["Benzine"].draw(0,0);
                    ofPushMatrix(); // DOUBLE CODE :(
                    ofPushStyle();
                        ofSetLineWidth(10 * windowScale);
                        ofTranslate(ofGetWidth() * 0.5, ofGetHeight() * 0.655);
                        float noiseRange = ofMap(busses[10], 0, 180, 1, 0.05);
                        noiseRange *= 30;
                        ofRotateDeg(180 + busses[10] + ((ofNoise((float)(ofGetFrameNum()/50.)) * noiseRange)));
                        ofDrawLine(0, 0, ofGetWidth() * 0.25, 0);
                        ofDrawCircle(0, 0, 20);
                    ofPopStyle();
                    ofPopMatrix();
                    
                    
                    ofPushMatrix();
                    ofTranslate(ofGetWidth(), 0);
                        images["lineGray"].draw(0,0);
                        images["lineWhite"].drawSubsection(0, 0, images["lineWhite"].getWidth() * 0.2 + busses[14], images["lineWhite"].getHeight(), 0, 0);
                        ofTranslate(busses[14], 0); // joniskAdd
                            images["glowOL"].draw(0,0);
                            images["joniskOL"].draw(0,0);
                    ofPopMatrix();
                    
                    images["Motor"].draw(ofGetWidth() * 2, 0);
                    
                    for(auto i: vector<glm::vec4>{
                        glm::vec4(450, 629, 5,0),
                        glm::vec4(847,212, -5,0),
                        glm::vec4(351, 364, 6,0),
                        glm::vec4(983,376, -2,1),
                    }){
                        string name = ofToString(i.x) + "-" + ofToString(i.y);
                        ofPushMatrix();
                        ofTranslate(ofGetWidth() * 2, 0);
                        ofTranslate(i.x, i.y);
                        if(i[3] == 1){
                            float r = ofGetFrameNum() / i.z;
                            r = fmod(r, 180.);
                            r /= 180.;
                            r *= TWO_PI;
                            r = sin(r);
                            r = r * 0.5 + 0.5;
                            ofRotateXDeg(r * 180);
                        } else{
                            ofRotateDeg(ofGetFrameNum() / i.z);
                        }
                        ofTranslate(images[name].getWidth()*-0.5, images[name].getHeight()*-0.5);
                        images[name].draw(0,0);
                        ofPopMatrix();
                    }
                     
                    ofPopMatrix();
                }
            }
                break;
            case scenes::Commercial:{
                if(busses[0]){
                    images["Shopping"].draw(0,0);
                }
                if(commercial){
                    if(commercial->isPlaying()){
                        float scaledHeight = commercial->getHeight() * (ofGetWidth() / commercial->getWidth());
                        commercial->draw(0,(ofGetHeight() - scaledHeight) * 0.5, ofGetWidth(), scaledHeight);
                    }
                }
            }
                break;
            case scenes::Benzine:{
                if(busses[13]){
                    render(scenes::Stars, false); // Don't write to main FBO
                }
                
                float h = ofGetHeight() * (busses[4]/100.);
                h *= 0.5;
                ofSetColor(255, 0, 0, busses[5]);
                ofDrawRectangle(0, ofGetHeight() * 0.5, ofGetWidth(), h);
                ofDrawRectangle(0, ofGetHeight() * 0.5 - (h), ofGetWidth(), h);
                
                if(busses[3] == 255){
                    ofSetColor(255);
                    images["Benzine"].draw(0,0);
                    
                    ofPushMatrix();
                    ofPushStyle();
                        ofSetLineWidth(10 * windowScale);
                        ofTranslate(ofGetWidth() * 0.5, ofGetHeight() * 0.655);
                        float noiseRange = ofMap(busses[10], 0, 180, 1, 0.05);
                        noiseRange *= 30;
                        ofRotateDeg(180 + busses[10] + ((ofNoise((float)(ofGetFrameNum()/50.)) * noiseRange)));
                        ofDrawLine(0, 0, ofGetWidth() * 0.25, 0);
                        ofDrawCircle(0, 0, 20);
                    ofPopStyle();
                    ofPopMatrix();
                }
            }
                break;
            case scenes::ReturnToShip:{
                int imgIndex = busses[0];
                if(imgIndex < NUM_RETURNIMAGES && imgIndex >= 0){
                    returnImages[imgIndex].draw(0,0, ofGetWidth(), ofGetHeight());
                }
            }
                break;
            case scenes::CaptainPicto:
                images["Captain"].draw(0,0);
                break;
            case scenes::Waveform:{
                if(busses[0]){
                    ofSetColor(255, busses[0]);
                    ofSetLineWidth(3);
                    ofNoFill();
                    ofPushMatrix();
                    ofTranslate(0, -ofGetHeight()*0.2); // Waveform @ 25% of screen H
                    ofBeginShape();
                    for (unsigned int i = 0; i < waveForm.size(); i++){
                        ofVertex(
                                 (i*((float)ofGetWidth()/waveForm.size())),
                                 waveForm[i]
                                 );
                    }
                    ofEndShape(false);
                    ofPopMatrix();
                }
                if(busses[1]){
                    ofSetColor(255, busses[1]);
                    images["incomingCall"].draw(0,0);
                }
                if(busses[2]){
                    ofSetColor(255);
                    images["duringCall"].draw(0,0);
                    int x = 595;
                    if(busses[4] < 10){
                        callCountFont.drawString("00:0" + ofToString(busses[4]), x, 547);
                    } else{
                        callCountFont.drawString("00:" + ofToString(busses[4]), x, 547);
                    }
                }
                if(busses[3]){
                    ofSetColor(255);
                    images["telephone"].draw(images["telephone"].getHeight(),images["telephone"].getHeight());
                }
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
                images["CodeTxt"].draw(0,0, width, height);
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
                if(prefix == "1280/"){
                    ofDrawRectangle(389, 536 * 0.666, (busses[1]/100.)*(889-389), ofGetHeight()); // 66%
                } else{
                    ofDrawRectangle(416, 536, (busses[1]/100.)*(866-416), ofGetHeight());
                }
                images["Autopilot"].draw(0,0, ofGetWidth(), ofGetHeight());
                joniskHover(false);
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
            case scenes::Test:{
                ofFill(); ofSetColor(255);
                ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
                float lineW = 20;
                ofSetColor(0);
                ofDrawRectangle(lineW, lineW, ofGetWidth() - lineW*2, ofGetHeight() - lineW*2);
                ofSetColor(255);
//                ofDrawBitmapString(ofToString(portNumAdd), lineW*2, lineW*2);
                string idTxt = ofToString(portNumAdd);
                countFont.drawString(idTxt, ofGetWidth()*0.5 - countFont.getStringBoundingBox(idTxt, 0, 0).width*0.5,
                                     ofGetHeight()*0.5 + countFont.getStringBoundingBox(idTxt, 0, 0).height*0.5);
            }
                break;
            case scenes::Einde:
                images["Einde"].draw(0,0);
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
        if(whiteFrame > 0){
            whiteFrame--;
            ofSetColor(255);
            ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        }
        if(bBlack){
            ofSetColor(0);
            ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        }
    if(bUseFbo){
        renderFbo.end();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    switch(fboDisplayMode){
        case 0:
            ofBackground(0);
            renderFbo.getTexture().bind();
            mesh.draw();
            renderFbo.getTexture().unbind();
            if(bEditMode){
                for(char i=0; i<4; i++)
                    ofDrawCircle(meshVertices[i], 10);
            }
            break;
        case 1: // Stretch
            renderFbo.draw(0, 0);
            break;
        case 2: //
            cout << "Display mode not supported" << endl;
//            fbo.draw(0, ofGetHeight() * 0.5 - (0.5 * (ofGetHeight() / (f.getWidth() / ofGetWidth()))), ofGetWidth(), ofGetHeight() / (f.getWidth() / ofGetWidth()));
            break;
    }
}

void ofApp::processMsg(ofxOscMessage &m){
    if(m.getAddress() == "/setScene"){
        scene = (scenes::Scene)m.getArgAsInt(0);
    } else if(m.getAddress() == "/eventById"){
        switch(m.getArgAsInt(0)){
            case 1:{
                int movieID = m.getArgAsInt(1);
                if(movieID == -1 && commercial){
                    commercial->stop();
                    return;
                }
                commercial = &(commercials[movieID]);
                commercial->stop(); // Restart
#ifdef TARGET_RASPBERRY_PI
                string videoPath = ofToDataPath(prefix + "commercial/" + ofToString(movieID) + ".mp4", true);
                ofxOMXPlayerSettings settings;
                settings.videoPath = videoPath;
                settings.enableTexture = false;
                settings.enableLooping = false;
                settings.enableAudio = false;
                    
                commercial.setup(settings);
                commercial.setPaused(false);
#else
//                commercial.load(prefix + "commercial/" + ofToString(movieID) + ".mp4");
//                commercial.setLoopState(ofLoopType::OF_LOOP_NONE);
                commercial->play();
#endif

                if(movieID == -1){
#ifdef TARGET_RASPBERRY_PI
                    commercial.setPaused(true);
                    commercial.close();
#else
                    commercial->stop();
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
            case 4:{
                meshVertices[m.getArgAsInt(1)] = glm::vec3(m.getArgAsFloat(2), m.getArgAsFloat(3), 0);
                mesh.clear();
                mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

                for(char i=0; i<4; i++){
                    mesh.addTexCoord(texCoords[i]);
                    mesh.addVertex(meshVertices[i]);
                }
            }
            break;
            case 5:
                ofSetFullscreen(m.getArgAsBool(1));
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
            case 16:
                whiteFrame = m.getArgAsInt(1);
                break;
            case 17:
                bBlack = m.getArgAsBool(1);
                break;
        }
    } else if (m.getAddress() == "/setId"){
        id = m.getArgAsInt(0);
    } else if(m.getAddress() == "/setWave"){
        waveForm.clear();
        ofBuffer b = m.getArgAsBlob(0);
//        cout << b.size() << endl;
        double data[b.size() / 8]; // 3200 bytes = 400 doubles
        memcpy(data, b.getData(), b.size()); // Needs 3200 bytes
        for(int i=0; i<b.size() / 8; i++){
            waveForm.push_back((float)data[i] * ofGetHeight()*0.5 + (ofGetHeight()*0.5));
//            cout << data[i] << endl;
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
        case 'q':
            fboDisplayMode = 0;
            break;
        case 'w':
            fboDisplayMode = 1;
            break;
        case 'e':
            fboDisplayMode = 2;
            break;
        case 'a':
            bEditMode = !bEditMode;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    cout << x << " " << y << endl;
    if(bEditMode){
        unsigned char indexOfClosest = 0;
        float minDistance = 99999999;
        for(char i=0; i<4; i++){
            if(ofVec2f(x, y).distance(meshVertices[i]) < minDistance){
                minDistance = ofVec2f(x, y).distance(meshVertices[i]);
                indexOfClosest = i;
            }
        }
        cout << "Selected: " << (int)indexOfClosest << endl;
        meshVertices[indexOfClosest] = glm::vec3(x, y, 0);
        mesh.clear();
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

        for(char i=0; i<4; i++){
            mesh.addTexCoord(texCoords[i]);
            mesh.addVertex(meshVertices[i]);
        }
    }
}


//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}

void ofApp::joniskHover(bool bGradient){
    ofPushMatrix();
        ofTranslate(0, pow(sin(seed + ofGetFrameNum()/50.), 2.) * 30 * busses[0]);
        
        ofSetColor(255);
            ofPushMatrix(); // Note: the zoom is not fixed to jonisk center
                ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.25);
                ofScale(sin(ofGetFrameNum()/50.) * (0.1 * 1.) + 1.);
                if(bGradient)
                    images["Intro gradient"].draw(-ofGetWidth()*0.5, -ofGetHeight()*0.25, width, height);
            ofPopMatrix();
        ofSetColor(255);
        images["Intro Jonisk"].draw(0,0, width, height);
    ofPopMatrix();
}

void ofApp::initMesh(){
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    texCoords = {glm::vec2(0,0), glm::vec2(width, 0), glm::vec2(width, height), glm::vec2(0, height)};
    meshVertices = {glm::vec3(0, 0, 0), glm::vec3(width, 0, 0), glm::vec3(width, height, 0), glm::vec3(0, height, 0)};
    for(char i=0; i<4; i++){
//        mesh.addTexCoord(texCoords[i] + glm::vec2(size.x, 0)); // Center piece
        mesh.addTexCoord(texCoords[i]); // Center piece
        mesh.addVertex(meshVertices[i]);
    }
}
