/*
 *  OscInputGenerator.cpp
 *  ofApp
 *
 *  Created by Brian Eschrich on 15/12/14
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "OscInputGenerator.h"


OscInputGenerator::OscInputGenerator(string name_, int nodeID_):ParamInputGenerator(name_, true){
    oscMap  = new std::map<string,DTOscMap* >();
    type    = OSC;
    nodeID  = nodeID_;
    port    = 6666;
    receiver.setup(port);
}

//------------------------------------------------------------------
OscInputGenerator::~OscInputGenerator(){
    
//    receiver.~ofxOscReceiver();
    
    oscMap->clear();
    delete oscMap;
    oscMap = NULL;
}

//------------------------------------------------------------------
/*bool OscInputGenerator::setupFromXML() {
    bool result = true;
    
    if( XML.loadFile("paramGen_" + generatorName + ".xml") ){
        
        //get osc input device
        
        string deviceName = XML.getAttribute("OSC_SETTINGS","port","12345");
        receiver.setup(ofToInt(deviceName));
        
        //get osc mapping data
        
        XML.pushTag("OSC_SETTINGS");
        int getNumAddressTag = XML.getNumTags("ADDRESS");
        for (int j=0; j<getNumAddressTag; ++j) {
            
            string address = XML.getAttribute("ADDRESS","path","/",j);
            XML.pushTag("ADDRESS");
            int getNumOSCMapTag = XML.getNumTags("OSC_MAP");
            DTOscMap* dtM = new DTOscMap();
            for(int i = 0;i<getNumOSCMapTag ; i++){
                
                string nodeId = XML.getAttribute("OSC_MAP","nodeName","",i);
                string param = XML.getAttribute("OSC_MAP","param","",i);
                float inputMinValue = ofToFloat(XML.getAttribute("OSC_MAP","inputMinValue","0",i));
                float inputMaxValue = ofToFloat(XML.getAttribute("OSC_MAP","inputMaxValue","127",i));
                float paramMinValue = ofToFloat(XML.getAttribute("OSC_MAP","paramMinValue","0",i));
                float paramMaxValue = ofToFloat(XML.getAttribute("OSC_MAP","paramMaxValue","127",i));
                
                dtM->path = address;
                dtM->paramId.push_back(param);
                dtM->nodeId.push_back(nodeId);
                dtM->inputMinValue.push_back(inputMinValue);
                dtM->inputMaxValue.push_back(inputMaxValue);
                dtM->paramMinValue.push_back(paramMinValue);
                dtM->paramMaxValue.push_back(paramMaxValue);
                
                oscMap->insert(std::pair<string,DTOscMap* >(address,dtM));
            }
            XML.popTag();
        }
        XML.popTag();
    }
    
    return result;

	
}*/

//------------------------------------------------------------------
void OscInputGenerator::processInput() {
    
    if(lock()){
        while(receiver.hasWaitingMessages()){
            // get the next message
            ofxOscMessage m;
            receiver.getNextMessage(&m);
            
            std::map<string,DTOscMap* >::iterator it;
            
            it = oscMap->find(m.getAddress());
            
            if(it!=oscMap->end()){
                for (int j=0; j<m.getNumArgs(); ++j) {
                    
                    Param* p        = new Param();
                    p->inputMax     = it->second->inputMaxValue[j];
                    p->inputMin     = it->second->inputMinValue[j];
                    p->imageInputId = it->second->nodeId[j];
                    p->name         = it->second->paramId[j];
                    p->value        = m.getArgAsFloat(j);
                    p->floatVal     = ofMap(m.getArgAsFloat(j), it->second->inputMinValue[j], it->second->inputMaxValue[j], it->second->paramMinValue[j], it->second->paramMaxValue[j]);
                    p->intVal       = p->floatVal;
                    storeMessage(p);
                }
            }
        }
        unlock();
    }
}

//------------------------------------------------------------------
void OscInputGenerator::setPort(int port_) {
    
    port = port_;
    if (port > 999)
        receiver.setup(port);
}

//------------------------------------------------------------------
void OscInputGenerator::setAddress(string oldAddress_, string address_) {
 
    std::map<string,DTOscMap* >::iterator it;
    
    it = oscMap->find(oldAddress_);
    if (it != oscMap->end()) {
//        it->second->path = address_;
        oscMap->insert(std::pair<string,DTOscMap* >(address_,it->second));
        oscMap->erase(it);
    }
}

//------------------------------------------------------------------
bool OscInputGenerator::addNewOSCMap(string address_, ImageOutput* node_, vector<string> params_) {
    
    std::map<string,DTOscMap* >::iterator it = oscMap->find(address_);
    
    if (it != oscMap->end()) {
        for (int i = 0; i < params_.size(); i++) {
            it->second->paramId.push_back(params_[i]);
            it->second->nodeId.push_back(node_->getId());
            it->second->inputMinValue.push_back(0);
            it->second->inputMaxValue.push_back(127);
            it->second->paramMinValue.push_back(node_->getMidiMin(params_[i]));
            it->second->paramMaxValue.push_back(node_->getMidiMax(params_[i]));
        }
    }
    else {
        DTOscMap* dtM = new DTOscMap();
//        dtM->path = address_;
        
        for (int i = 0; i < params_.size(); i++) {
            dtM->paramId.push_back(params_[i]);
            dtM->nodeId.push_back(node_->getId());
            dtM->inputMinValue.push_back(0);
            dtM->inputMaxValue.push_back(127);
            dtM->paramMinValue.push_back(node_->getMidiMin(params_[i]));
            dtM->paramMaxValue.push_back(node_->getMidiMax(params_[i]));
        }
        oscMap->insert(std::pair<string,DTOscMap* >(address_,dtM));
    }
}

//------------------------------------------------------------------
bool OscInputGenerator::addNewOSCMap(string address_, ImageOutput* node_, string param_) {
    
    std::map<string,DTOscMap* >::iterator it = oscMap->find(address_);
    
    if (it != oscMap->end()) {
        it->second->paramId.push_back(param_);
        it->second->nodeId.push_back(node_->getId());
        it->second->inputMinValue.push_back(0);
        it->second->inputMaxValue.push_back(127);
        it->second->paramMinValue.push_back(node_->getMidiMin(param_));
        it->second->paramMaxValue.push_back(node_->getMidiMax(param_));
    }
    else {
        DTOscMap* dtM = new DTOscMap();
//        dtM->path = address_;
        
        dtM->paramId.push_back(param_);
        dtM->nodeId.push_back(node_->getId());
        dtM->inputMinValue.push_back(0);
        dtM->inputMaxValue.push_back(127);
        dtM->paramMinValue.push_back(node_->getMidiMin(param_));
        dtM->paramMaxValue.push_back(node_->getMidiMax(param_));
        
        oscMap->insert(std::pair<string,DTOscMap* >(address_,dtM));
    }
}

//------------------------------------------------------------------
void OscInputGenerator::removeOSCMap(string address_, ImageOutput* node_, string param_) {
    
    std::map<string,DTOscMap* >::iterator it = oscMap->find(address_);
    
    if (it != oscMap->end()) {
        
        int j = 0;
        while (j < it->second->paramId.size()) {
            if (it->second->nodeId[j] == node_->getId() && (it->second->paramId[j] == param_ ||
               (it->second->paramId[j].compare(0, 5, "Blend")==0 && param_.compare(0, 5, "Blend")==0))) {
                
                it->second->paramId.erase(it->second->paramId.begin() + j);
                it->second->nodeId.erase(it->second->nodeId.begin() + j);
                it->second->inputMinValue.erase(it->second->inputMinValue.begin() + j);
                it->second->inputMaxValue.erase(it->second->inputMaxValue.begin() + j);
                it->second->paramMinValue.erase(it->second->paramMinValue.begin() + j);
                it->second->paramMaxValue.erase(it->second->paramMaxValue.begin() + j);
                
                j = it->second->paramId.size();
            }
            
            j++;
        }
    }
}

//------------------------------------------------------------------
void OscInputGenerator::removeNodeFromParams(int nodeID_) {
    
    for (std::map<string,DTOscMap* >::iterator it = oscMap->begin(); it != oscMap->end(); it++) {
        for (int j = 0; j < it->second->nodeId.size(); j++) {
            if (it->second->nodeId[j] == nodeID_) {
                it->second->paramId.erase(it->second->paramId.begin() + j);
                it->second->nodeId.erase(it->second->nodeId.begin() + j);
                it->second->inputMinValue.erase(it->second->inputMinValue.begin() + j);
                it->second->inputMaxValue.erase(it->second->inputMaxValue.begin() + j);
                it->second->paramMinValue.erase(it->second->paramMinValue.begin() + j);
                it->second->paramMaxValue.erase(it->second->paramMaxValue.begin() + j);
                j--;
            }
        }
    }
}

//------------------------------------------------------------------
void OscInputGenerator::clearOSCMap() {
    oscMap->clear();
}


//------------------------------------------------------------------
bool OscInputGenerator::loadSettings(ofxXmlSettings &XML) {
    
    bool result = true;
    
    //get osc input device
    //
    port = ofToInt(XML.getAttribute("OSC_SETTINGS","port","6666"));
    receiver.setup(port);
    
    //get osc mapping data
    //
    XML.pushTag("OSC_SETTINGS");
    int getNumAddressTag = XML.getNumTags("ADDRESS");
    
    for (int j = 0; j < getNumAddressTag; ++j) {
        
        string address = XML.getAttribute("ADDRESS","path","/",j);
        XML.pushTag("ADDRESS");
        int getNumOSCMapTag = XML.getNumTags("OSC_MAP");
        DTOscMap* dtM = new DTOscMap();
        
        for(int i = 0; i < getNumOSCMapTag; i++){
            
            int     nodeId = XML.getAttribute("OSC_MAP","nodeId",0,i);
            string  param = XML.getAttribute("OSC_MAP","param","",i);
            float   inputMinValue = ofToFloat(XML.getAttribute("OSC_MAP","inputMinValue","0",i));
            float   inputMaxValue = ofToFloat(XML.getAttribute("OSC_MAP","inputMaxValue","127",i));
            float   paramMinValue = ofToFloat(XML.getAttribute("OSC_MAP","paramMinValue","0",i));
            float   paramMaxValue = ofToFloat(XML.getAttribute("OSC_MAP","paramMaxValue","127",i));
            
//            dtM->path = address;
            dtM->paramId.push_back(param);
            dtM->nodeId.push_back(nodeId);
            dtM->inputMinValue.push_back(inputMinValue);
            dtM->inputMaxValue.push_back(inputMaxValue);
            dtM->paramMinValue.push_back(paramMinValue);
            dtM->paramMaxValue.push_back(paramMaxValue);
            
            oscMap->insert(std::pair<string,DTOscMap* >(address,dtM));
        }
        XML.popTag();
    }
    XML.popTag();
    
    return result;
}


//------------------------------------------------------------------
bool OscInputGenerator::saveSettings(ofxXmlSettings &XML) {
    
    bool saved = true;
    
    // Search for the input generator name to update information
    // If it doesn't exists.. then I need to add it to the .xml
    //
    
    // Get the total number of input generators ...
    //
    int totalInputGen = XML.getNumTags("INPUT_GEN");
    
    // ... and search for the right name for loading
    //
    for (int i = 0; i <= totalInputGen; i++){
        
        // Once it found the right one ...
        //
        if ( XML.getAttribute("INPUT_GEN", "nodeId", -1, i) == nodeID){
            
            XML.pushTag("INPUT_GEN", i);
            
            XML.addAttribute("OSC_SETTINGS", "port", port, i);
            XML.pushTag("OSC_SETTINGS");
            
            int numAddressMapTag = XML.getNumTags("ADDRESS");
            for (int v = 0; v < numAddressMapTag; v++){
                XML.removeTag("ADDRESS");
            }
            
            int lastPlace = 0;
            for (std::map<string,DTOscMap* >::iterator it = oscMap->begin(); it != oscMap->end(); it++) {
                for (int j = 0; j < it->second->nodeId.size(); j++) {
                
                    lastPlace = XML.addTag("ADDRESS");
                    XML.setAttribute("ADDRESS","path", it->first, lastPlace);
                    XML.pushTag("ADDRESS", lastPlace);
                    
                    XML.addTag("OSC_MAP");
                    XML.setAttribute("OSC_MAP","nodeId", it->second->nodeId[j], j);
                    XML.setAttribute("OSC_MAP","param", it->second->paramId[j], j);
                    XML.setAttribute("OSC_MAP","inputMinValue", it->second->inputMinValue[j], j);
                    XML.setAttribute("OSC_MAP","inputMaxValue", it->second->inputMaxValue[j], j);
                    XML.setAttribute("OSC_MAP","paramMinValue", it->second->paramMinValue[j], j);
                    XML.setAttribute("OSC_MAP","paramMaxValue", it->second->paramMaxValue[j], j);
                    
                    XML.popTag(); // ADDRESS
                }
            }
            
            XML.popTag(); // OSC_SETTINGS
            XML.popTag(); // INPUT_GEN
            
            break;
        }
        
        // If it was the last input generator in the XML and it wasn't me..
        // I need to add myself in the .xml file
        //
        else if (i >= totalInputGen-1) {
            
            // Insert a new INPUT_GEN tag at the end
            // and fill it with the proper structure
            //
            int lastPlace = XML.addTag("INPUT_GEN");
            
            XML.addAttribute("INPUT_GEN", "nodeId", nodeID, lastPlace);
            XML.addAttribute("INPUT_GEN", "type", "OSC", lastPlace);
            
            saved = XML.pushTag("INPUT_GEN", lastPlace);
            if (saved) {
                lastPlace = XML.addTag("OSC_SETTINGS");
                XML.addAttribute("OSC_SETTINGS", "port", port, lastPlace);
                saved = XML.pushTag("OSC_SETTINGS", lastPlace);
                if (saved) {
                    for (std::map<string,DTOscMap* >::iterator it = oscMap->begin(); it != oscMap->end(); it++) {
                        lastPlace = XML.addTag("ADDRESS");
                        XML.setAttribute("ADDRESS","path", it->first, lastPlace);
                        XML.pushTag("ADDRESS", lastPlace);
                        for (int j = 0; j < it->second->nodeId.size(); j++) {
                            
                            XML.addTag("OSC_MAP");
                            XML.addAttribute("OSC_MAP","nodeId", it->second->nodeId[j], j);
                            XML.addAttribute("OSC_MAP","param", it->second->paramId[j], j);
                            XML.addAttribute("OSC_MAP","inputMinValue", it->second->inputMinValue[j], j);
                            XML.addAttribute("OSC_MAP","inputMaxValue", it->second->inputMaxValue[j], j);
                            XML.addAttribute("OSC_MAP","paramMinValue", it->second->paramMinValue[j], j);
                            XML.addAttribute("OSC_MAP","paramMaxValue", it->second->paramMaxValue[j], j);
                        }
                        XML.popTag(); // ADDRESS
                    }
                    
                    XML.popTag(); // FFT_SETTINGS
                }
                XML.popTag(); // INPUT_GEN
            }
        }
    }
    
    return saved;
    
}

