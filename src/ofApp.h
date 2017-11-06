#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "MSABPMTapper.h"
#include "ofxXmlSettings.h"

#define XML_NAME "settings.xml"

class ofApp : public ofBaseApp{
    
public:
    void setup();
     void loadSettings();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void gotMessage(ofMessage msg);
    
    void flicker();
    void smooth();
    void beat();
    
    bool fileExists(string name);
    
    ofxSyphonServer server;
    ofxSyphonServer smoothServer;
    ofxSyphonServer cutServer;
    ofxSyphonServer beatServer;
    ofFbo fbo;
    bool showResults = true;
    vector <ofColor > colors;
    
    float previousBeat;
    int currentBeatColor;
    float lastBeat;
    msa::BPMTapper tapTempo;
    string errorMessage;
};
