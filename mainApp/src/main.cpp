#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main(int argc, char *argv[]){
    string appName = "mainApp";
    vector<string> arguments = vector<string>(argv, argv + argc);
    
    if(arguments[1] == "-h"){
        cout << "Run like: " << endl;
        cout << "\t" << appName << " numWindows" << endl;
        exit(0);
    }
    
    int width = arguments.size() > 1 ? ofToInt(arguments[1]) : 1280;
    int height = arguments.size() > 2 ? ofToInt(arguments[2]) : 800;
    int numWindows = arguments.size() > 3 ? ofToInt(arguments[3]) : 3;
    float windowScale = arguments.size() > 4 ? ofToInt(arguments[4]) : 0.5;
    bool bRotate = arguments.size() > 5 ? ofToInt(arguments[5]) : false;
    
    width *= windowScale;
    height *= windowScale;
    
#ifdef  TARGET_RASPBERRY_PI
    ofSetupOpenGL(width,height,OF_FULLSCREEN);
    ofGLFWWindowSettings settings;

    settings.setSize(width,height);
    settings.setPosition(glm::vec2(0,0));
    settings.decorated = false;
    settings.resizable = false;
//    settings.orientation = OF_ORIENTATION_180; // Doesn't work
    
    shared_ptr<ofAppBaseWindow> window = ofCreateWindow(settings);
    shared_ptr<ofApp> app(new ofApp);
    ofRunApp(window, app);

    ofRunMainLoop();
#else
    
    vector<Star*> stars;
    for(int i=0; i<300; i++){
        stars.push_back(new Star());
        stars.back()->setLoc(
                            glm::vec3(
                                      width * ofRandom(3), // Left screen, mid screen, right screen. This will be set from SC eventually
                                      ofRandom(height),
                                      ofRandom(-1000, 1000)
                                      ),
                             true
                             );
    }
    
    for(int i=0; i<numWindows; i++){
        ofGLFWWindowSettings settings;

        settings.setSize(width, height);
        settings.setPosition(glm::vec2(width * i,0));
        settings.resizable = true;
        shared_ptr<ofAppBaseWindow> window = ofCreateWindow(settings);

        shared_ptr<ofApp> mainApp(new ofApp);
        mainApp->portNumAdd = i;
        mainApp->width = width;
        mainApp->height = height;
        mainApp->bRotate = bRotate;
        
        ofRunApp(window, mainApp); // This calls setup() ?
        
        mainApp->stars->stars.clear();
        mainApp->stars->stars = stars;
    
//        outterWindow->setOsc(5001);
    }
    
    
    ofRunMainLoop();
    
#endif
}
