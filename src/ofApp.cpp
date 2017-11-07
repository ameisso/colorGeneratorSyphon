#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetFrameRate(60);
    flickerServer.setName("flicker");
    smoothServer.setName("smooth");
    beatServer.setName("beat");
    flapServer.setName("flap");
    barServer.setName("bar");
    arcServer.setName("arc");
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
            settings.pushTag("osc");
            oscSender.setup(settings.getAttribute("target", "IP", "localhost"), settings.getAttribute("target", "port", 5000));
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
    ofSetWindowTitle("bpm : "+ofToString(floor(tapTempo.bpm())));
    ofxOscMessage m;
    m.setAddress("/beatStream");
    m.addFloatArg(tapTempo.beatPerc());
    m.addFloatArg(tapTempo.sin1);
    oscSender.sendMessage(m, false);
    
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(0);
    ofDrawBitmapString("'b' for beat, 's' for results, 'r' to reload", 30, ofGetHeight()/2);
    if(colors.size()>1)
    {
        flicker();
        smooth();
        beat();
        flap();
        bar();
        arc();
    }
}

void ofApp::flicker()
{
    fbo.begin();
    int v = ofRandom(colors.size());
    ofSetColor(colors.at(v));
    ofFill();
    ofDrawRectangle(0, 0, fbo.getWidth()  , fbo.getHeight());
    flickerServer.publishTexture(&fbo.getTexture());
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
        ofxOscMessage m;
        m.setAddress("/beat");
        m.addIntArg(int(tapTempo.beatTime())%2);
        oscSender.sendMessage(m, false);
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

void ofApp::flap()
{
    fbo.begin();
    ofSetColor(colors[0]);
    ofFill();
    ofDrawRectangle(0, 0, fbo.getWidth()  , fbo.getHeight());
    
    ofSetColor(colors[1]);
    ofDrawRectangle(0, 0, fbo.getWidth(), abs(tapTempo.sin4)*fbo.getHeight());
    flapServer.publishTexture(&fbo.getTexture());
    fbo.end();
    if( showResults )
    {
        fbo.draw(300, 0,100,100);
    }
}

void ofApp::bar()
{
    int barWidth = fbo.getWidth()/10;
    fbo.begin();
    ofSetColor(colors[0]);
    ofFill();
    ofDrawRectangle(0, 0, fbo.getWidth()  , fbo.getHeight());
    
    ofSetColor(colors[1]);
    ofDrawRectangle(abs(tapTempo.sin4)*fbo.getWidth(), 0, barWidth  , fbo.getHeight());
    barServer.publishTexture(&fbo.getTexture());
    fbo.end();
    if( showResults )
    {
        fbo.draw(400, 0,100,100);
    }
}

void ofApp::arc()
{
    int barWidth = fbo.getWidth()/10;
    fbo.begin();
    ofSetColor(colors[0]);
    ofFill();
    ofDrawRectangle(0, 0, fbo.getWidth()  , fbo.getHeight());
    ofEnableSmoothing();
    ofPath curve;
    curve.arc(ofPoint( fbo.getWidth()/2  , fbo.getHeight()/2),  fbo.getHeight()/2*0.8 , fbo.getHeight()/2*0.8, 270, 360*tapTempo.beatPerc());
    curve.setStrokeColor(colors[1]);
    curve.setStrokeWidth(fbo.getWidth()/5.0);
    curve.setFilled(false);
    curve.setCircleResolution(100);
    curve.draw();
    ofSetColor(colors[0]);
    ofPath insideCurve;
    insideCurve.arc(ofPoint( fbo.getWidth()/2  , fbo.getHeight()/2),  fbo.getHeight()/2*0.8 , fbo.getHeight()/2*0.8, 0, 360);
    insideCurve.setCircleResolution(100);
    insideCurve.setFillColor(colors[0]);
    insideCurve.setFilled(true);
    insideCurve.draw();
    ofDisableSmoothing();
    arcServer.publishTexture(&fbo.getTexture());
    fbo.end();
    if( showResults )
    {
        fbo.draw(500, 0,100,100);
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
            ofSetWindowShape(800, 40);
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

