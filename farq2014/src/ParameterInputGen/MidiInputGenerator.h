//
//  MidiInputGenerator.h
//  ofApp
//
//  Created by Christian Clark on 12/14/14.
//
//

#ifndef _MidiInputGenerator
#define _MidiInputGenerator

#include "ParamInputGenerator.h"
#include "Param.h"
#include "DTMidiMap.h"
#include "ofxMidi.h"
#include "ImageOutput.h"

class MidiInputGenerator: public ParamInputGenerator, public ofxMidiListener  {
    
public:
    
    MidiInputGenerator(string name_, string midiDeviceName);
    void processInput();
//    bool setupFromXML();
    void newMidiMessage(ofxMidiMessage& eventArgs);
//    void keyPressed (int key);
//    void nextMidiMap();
//    void prevMidiMap();
    bool addNewMidiMap(int control_, ImageOutput* node_, vector<string> params_);
    void setMidiLearnActive(bool active_) { midiLearnActive = active_; };
    
    //for storing midi control ids and map them to Node id and parameter.
//    vector<std::map<int,vector<DTMidiMap*>* >*>* midiMaps;
//    int activeMidiMap;
    map<int,vector<DTMidiMap*> > midiControlMaps;
    
    bool loadSettings(ofxXmlSettings &XML, map<int, ImageOutput*> nodes_);
    bool saveSettings(ofxXmlSettings &XML);
    
private:
    
    string midiDeviceName;
    ofxMidiIn midiIn;
    ofxMidiMessage midiMessage;
    bool midiLearnActive;
    
};


#endif /* defined(_MidiInputGenerator) */
