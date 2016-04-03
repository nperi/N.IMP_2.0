//
//  OSCReceiver.c
//  nimp
//
//  Created by Mili Garicoits on 3/8/16.
//
//

#include "OSCReceiver.h"

OSCReceiver::OSCReceiver(string name_, int id_) : InputSource(name_, "OSC Receiver", id_){
    
    isOSC = true;
    disabledEdit = false;
    oscReceiverImg.loadImage("assets/osc_receiver.png");
    
    address = "/";
    oldAddress = "/";
    port = 6666;
    oscPortNumber = "6666";
    gui.add(oscPort.setup("Port", oscPortNumber, 100, 20));
    gui.add(oscAddress.setup("Address", address, 100, 20));
    gui.add(editOSC.set("Edit OSC Inputs", false));
    
    paramsGroup.setName("Params");
    gui.add(paramsGroup);
    
    oscPort.addListener(this, &OSCReceiver::editPort);
    oscAddress.addListener(this, &OSCReceiver::editAddress);
    editOSC.addListener(this, &OSCReceiver::editInputs);
    
    gui.setWidthElements(INSPECTOR_WIDTH);
    
    title->removeButton('r');
    title->removeButton('m');
}

//------------------------------------------------------------------
void OSCReceiver::setup() {

}

//------------------------------------------------------------------
void OSCReceiver::update() {
    
}

//------------------------------------------------------------------
void OSCReceiver::customDraw(){
    
    ofxPatch::customDraw();
}

//------------------------------------------------------------------
ofImage* OSCReceiver::getImage(){
    return &oscReceiverImg;
}

//------------------------------------------------------------------
ofTexture* OSCReceiver::getTexture(){
    return &oscReceiverImg.getTextureReference();
}

//------------------------------------------------------------------
void OSCReceiver::editPort(string& p){
    
    port = ofToInt(p);
    
    OSCEvent ev;
    ev.nodeId = nId;
    ev.port = port;
    ev.address = address;
    ofNotifyEvent(editOSCPort, ev);
}

//------------------------------------------------------------------
void OSCReceiver::editAddress(string& a){
    
    OSCEvent ev;
    ev.nodeId = nId;
    ev.port = port;
    ev.address = address;
    ev.oldAddress = oldAddress;
    ofNotifyEvent(editOSCPort, ev);
    
    oldAddress = a;
}

//------------------------------------------------------------------
void OSCReceiver::editInputs(bool& b){
    
    if (!disabledEdit) {
        OSCEvent ev;
        ev.nodeId = nId;
        ev.port = port;
        ev.address = address;
        ofNotifyEvent(editOSCInputsActive, ev);
    }
    else {
        editOSC = false;
    }
}

//------------------------------------------------------------------
void OSCReceiver::setPort(int port_){
    
    port = port_;
}

//------------------------------------------------------------------
void OSCReceiver::addParameter(int nodeId_, string paramName_){
    
    gui.remove("Params");
    
    ofParameter<string> param = paramName_;
    paramsGroup.add(param);
    
    OSCParam newParam;
    newParam.nodeId = nodeId_;
    newParam.paramName = paramName_;
    paramsLabels.push_back(newParam);
    
    gui.add(paramsGroup);
    gui.setWidthElements(INSPECTOR_WIDTH);
}

//------------------------------------------------------------------
void OSCReceiver::removeParameter(int nodeId_, string paramName_){
    
    gui.remove("Params");
    
    for (int i = 0; i < paramsLabels.size(); i++) {
        if (paramsLabels[i].nodeId == nodeId_ && (paramsLabels[i].paramName == paramName_ ||
            (paramsLabels[i].paramName.compare(0, 5, "Blend")==0 && paramName_.compare(0, 5, "Blend")==0))) {
            paramsLabels.erase(paramsLabels.begin() + i);
        }
    }
    
    paramsGroup.clear();
    
    for (int i = 0; i < paramsLabels.size(); i++) {
        ofParameter<string> p = paramsLabels[i].paramName;
        paramsGroup.add(p);
    }
    
    gui.add(paramsGroup);
    gui.setWidthElements(INSPECTOR_WIDTH);
}

//------------------------------------------------------------------
void OSCReceiver::clearParameters() {
    
    paramsGroup.clear();
    paramsLabels.clear();
}

//------------------------------------------------------------------
void OSCReceiver::removeNodeParams(int nodeId_){
    
    gui.remove("Params");
    
    for (int i = 0; i < paramsLabels.size(); i++) {
        if (paramsLabels[i].nodeId == nodeId_) {
            paramsLabels.erase(paramsLabels.begin() + i);
            i--;
        }
    }
    
    paramsGroup.clear();
    
    for (int i = 0; i < paramsLabels.size(); i++) {
        ofParameter<string> p = paramsLabels[i].paramName;
        paramsGroup.add(p);
    }
    
    gui.add(paramsGroup);
    gui.setWidthElements(INSPECTOR_WIDTH);
}

//------------------------------------------------------------------
bool OSCReceiver::loadSettings(ofxXmlSettings &XML, int nTag_, int nodesCount_) {
    
    bool loaded = false;
    
    nId     = XML.getAttribute("NODE", "id", -1, nTag_) + nodesCount_;
    port    = XML.getAttribute("NODE", "port", 6666, nTag_);
    address = XML.getAttribute("NODE", "address", "/", nTag_);
    oscAddress.setup("Address", address, 100, 20);
    oldAddress = address;
    
    
    XML.pushTag("NODE", nTag_);
    
    type            = XML.getValue("type","none");
    bVisible        = XML.getValue("visible", true);
    filePath        = XML.getValue("path", "none" );
    
    int numParamTag = XML.getNumTags("PARAM");
    if (numParamTag > 0){
        for (int v = 0; v < numParamTag; v++){
            string name_ = XML.getAttribute("PARAM","name","",v);
            int nodeID_ = XML.getAttribute("PARAM","nodeID",-1,v);
            if (name_ != "" && nodeID_ != -1) {
                addParameter(nodeID_, name_);
            }
        }
    }

    ofxPatch::loadSettings(XML, nTag_, nodesCount_);
    
    XML.popTag();
    
    return loaded;
}

//------------------------------------------------------------------
bool OSCReceiver::saveSettings(ofxXmlSettings &XML) {
    
    bool saved = false;
    
    // Search for the patch ID to update information
    // If the patch ID doesn't exists.. then I need to add it to the .xml
    //
    
    // Get the total number of nodes of the same type ...
    //
    int totalNodes = XML.getNumTags("NODE");
    
    // ... and search for the right id for loading
    //
    for (int i = 0; i <= totalNodes; i++){
        
        
        // Once it found the right surface that match the id ...
        //
        if ( XML.getAttribute("NODE", "id", -1, i) == nId){
            
            XML.setAttribute("NODE", "name", name, i);
            XML.addAttribute("NODE", "port", port, i);
            XML.addAttribute("NODE", "address", address, i);
            XML.pushTag("NODE", i);
            
            int numParamTag = XML.getNumTags("PARAM");
            for (int v = 0; v < numParamTag; v++){
                XML.removeTag("PARAM");
            }
            
            for (int v = 0; v < paramsLabels.size(); v++){
                XML.addTag("PARAM");
                XML.addAttribute("PARAM", "name", paramsLabels[v].paramName, v);
                XML.addAttribute("PARAM", "nodeID", paramsLabels[v].nodeId, v);
            }
            
            ofxPatch::saveSettings(XML, false, i);
            
            XML.popTag();
            
            break;
        }
        
        // If it was the last node in the XML and it wasn't me..
        // I need to add myself in the .xml file
        //
        else if (i >= totalNodes-1) {
            
            // Insert a new NODE tag at the end
            // and fill it with the proper structure
            //
            int lastPlace = XML.addTag("NODE");
            
            XML.addAttribute("NODE", "id", nId, lastPlace);
            XML.addAttribute("NODE", "name", name, lastPlace);
            XML.addAttribute("NODE", "type", "OSC_RECEIVER", lastPlace);
            XML.addAttribute("NODE", "port", port, lastPlace);
            XML.addAttribute("NODE", "address", address, lastPlace);
            
            if (XML.pushTag("NODE", lastPlace)){
                
                for (int v = 0; v < paramsLabels.size(); v++){
                    XML.addTag("PARAM");
                    XML.addAttribute("PARAM", "name", paramsLabels[v].paramName, v);
                    XML.addAttribute("PARAM", "nodeID", paramsLabels[v].nodeId, v);
                }
                
                ofxPatch::saveSettings(XML, true, lastPlace);
                
                XML.popTag();
            }
        }
    }
    
    return saved;
    
}