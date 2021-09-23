#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;

    settings.setSize(1920, 1080);
    settings.setPosition(glm::vec2(0,0));
    settings.resizable = true;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

    settings.setSize(300, 300);
    settings.setPosition(glm::vec2(0,0));
    settings.resizable = true;
    shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);

    shared_ptr<ofApp> mainApp(new ofApp);
    shared_ptr<ofApp> outterWindow(new ofApp);
    
    ofRunApp(guiWindow, outterWindow);
    ofRunApp(mainWindow, mainApp);
    
    mainApp->setOsc(5000);
    outterWindow->setOsc(5001);
    outterWindow->setCommercialAmp(0);
    
    ofRunMainLoop();

}
