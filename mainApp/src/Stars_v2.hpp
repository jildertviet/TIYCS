int halfw;
int halfh;
int step = 2;
int warpZ = 12;
float speed = 0.075;
ofVec3f v;

ofColor getColor() {
    return ofColor::fromHsb(200, 255, ofRandom(50, 100));
}

class Star {
public:
    float x;
    float y;
    float z;
    ofColor color;

    Star() {
        reset();
    }

    void reset() {
        v.set(ofRandom(-halfw, halfw), ofRandom(-halfh, halfh), ofRandom(1, warpZ));
        x = v.x;
        y = v.y;
        color = getColor();
    }

    ofVec3f calcVel() {
        return ofVec3f(0, 0, -speed);
    }

    void draw() {
        v += calcVel();
        float x = v.x / v.z;
        float y = v.y / v.z;
        float x2 = v.x / (v.z + speed * 0.50);
        float y2 = v.y / (v.z + speed * 0.50);

        ofSetColor(color);
        ofDrawLine(x, y, x2, y2);

        if (x < -halfw || x > halfw || y < -halfh || y > halfh) {
            reset();
        }
    }
};

class Starfield {
public:
    int numOfStars = 250;
    vector<Star> stars;

    Starfield() {
        for (int i = 0; i < numOfStars; i++) {
            stars.push_back(Star());
        }
    }

    void draw() {
        ofTranslate(halfw, halfh);

        for (int i = 0; i < stars.size(); i++) {
            stars[i].draw();
        }
    }
};

Starfield mStarField;

void ofApp::setup() {
    halfw = ofGetWidth() / 2;
    halfh = ofGetHeight() / 2;

    ofSetBackgroundColor(ofColor::black);
}

void ofApp::update() {
}

void ofApp::draw() {
    speed = 0.025;

    ofSetColor(ofColor(0, 0, 0, 51));
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

    mStarField.draw();
}

void ofApp::windowResized(int w, int h) {
    halfw = w / 2;
    halfh = h / 2;
}
