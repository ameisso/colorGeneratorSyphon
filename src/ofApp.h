#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "MSABPMTapper.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"

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
    void flap();
    void bar();
    void arc();
    void square();
    
    bool fileExists(string name);
    
    ofxSyphonServer flickerServer;
    ofxSyphonServer smoothServer;
    ofxSyphonServer beatServer;
    ofxSyphonServer flapServer;
    ofxSyphonServer barServer;
    ofxSyphonServer arcServer;
    ofxSyphonServer squareServer;
    ofxOscSender oscSender;
    ofFbo fbo;
    bool showResults = true;
    vector <ofColor > colors;
    
    float previousBeat;
    int currentBeatColor;
    float lastBeat;
    msa::BPMTapper tapTempo;
    string errorMessage;
};
