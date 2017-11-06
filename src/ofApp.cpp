#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetFrameRate(60);
    server.setName("flicker");
    smoothServer.setName("smooth");
    cutServer.setName("cut");
    beatServer.setName("beat");
    loadSettings();
}

void ofApp::loadSettings()
{
    errorMessage = "";
    if( ! fileExists(XML_NAME) )
    {
        errorMessage = " 'settings.xml' not found in /data... \n";
        for(int i = 0; i < 3; i++)
        {
            colors.push_back(ofColor(255,0,0));
            colors.push_back(ofColor(0,0,0));
            colors.push_back(ofColor(255));
            fbo.allocate(100,100);
        }
    }
    else
    {
        ofxXmlSettings settings;
        if(settings.loadFile(XML_NAME))
        {
            settings.pushTag("params");
            fbo.allocate(settings.getAttribute("size", "X", 1920),settings.getAttribute("size", "X", 1080));
            settings.popTag();
            
            settings.pushTag("colors");
            int colorCount = settings.getNumTags("color");
            for(int i = 0; i < colorCount; i++)
            {
                settings.pushTag("color", i);
                colors.push_back(ofColor(settings.getAttribute("params", "R", 255),settings.getAttribute("params", "G", 0),settings.getAttribute("params", "B", 0)));
                settings.popTag();
            }
            settings.popTag();
        }
        else
        {
            errorMessage = " 'settings.xml' can't be read. Please check that the tags \n";
            for(int i = 0; i < 3; i++)
            {
                colors.push_back(ofColor(255,0,0));
                colors.push_back(ofColor(0,0,0));
                colors.push_back(ofColor(255));
                fbo.allocate(100,100);
            }
        }
    }
    if(errorMessage.length()>0)
    {
        ofLogError()<<errorMessage;
    }
    tapTempo.setBpm(60);
    currentBeatColor = 0;
}

//--------------------------------------------------------------
void ofApp::update()
{
    tapTempo.update();
    ofSetWindowTitle("bpm : "+ofToString(tapTempo.bpm()));
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(0);
    ofDrawBitmapString("RGB", ofGetWidth()/2-10, ofGetHeight()/2);
    if(colors.size()>1)
    {
        flicker();
        smooth();
        beat();
    }
}

void ofApp::flicker()
{
    fbo.begin();
    int v = ofRandom(colors.size());
    ofSetColor(colors.at(v));
    ofFill();
    ofDrawRectangle(0, 0, fbo.getWidth()  , fbo.getHeight());
    server.publishTexture(&fbo.getTexture());
    fbo.end();
    if( showResults )
    {
        fbo.draw(0, 0,100,100);
    }
}

void ofApp::smooth()
{
    if( colors.size() > 1 )
    {
        fbo.begin();
        float ratio = tapTempo.beatTime();
        
        ofColor curColor = ofColor() ;
        curColor.r = ofLerp( colors[0].r , colors[1].r , ratio ) ;
        curColor.g = ofLerp( colors[0].g , colors[1].g , ratio ) ;
        curColor.b = ofLerp( colors[0].b , colors[1].b , ratio ) ;
        
        ofSetColor(curColor);
        ofFill();
        
        ofDrawRectangle(0, 0, fbo.getWidth()  , fbo.getHeight());
        
        smoothServer.publishTexture(&fbo.getTexture());
        fbo.end();
        if( showResults )
        {
            fbo.draw(100, 0,100,100);
        }
    }
}

void ofApp::beat()
{
    fbo.begin();
    if ( previousBeat > 0 && tapTempo.sin1 < 0 )
    {
        currentBeatColor++;
        if(currentBeatColor == colors.size())
        {
            currentBeatColor = 0;
        }
    }
    previousBeat = tapTempo.sin1;
    ofSetColor(colors.at(currentBeatColor));
    ofFill();
    ofDrawRectangle(0, 0, fbo.getWidth()  , fbo.getHeight());
    beatServer.publishTexture(&fbo.getTexture());
    fbo.end();
    if( showResults )
    {
        fbo.draw(200, 0,100,100);
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ( key == 'r' )
    {
        loadSettings();
    }
    else if( key == 's')
    {
        showResults = ! showResults ;
        if(showResults)
        {
            ofSetWindowShape(400, 100);
        }
        else
        {
            ofSetWindowShape(100, 30);
        }
    }
    else if( key == 'b')
    {
        if(tapTempo.beatTime() > 3)
        {
            tapTempo.startFresh();
        }
        else
        {
            tapTempo.tap();
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

bool ofApp::fileExists(string name)
{
    bool fileExists = false;
    
    ofDirectory dir("");
    dir.listDir();
    for(int i = 0; i < dir.size(); i++)
    {
        if(dir.getName(i) == name)
        {
            fileExists = true;
            break;
        }
    }
    return fileExists;
}

