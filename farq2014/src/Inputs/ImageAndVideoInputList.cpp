/*
 *  ImageAndVideoInputList.cpp
 *  ofApp
 *
 *  Created by Brian Eschrich on 12/12/14
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "ImageAndVideoInputList.h"


ImageAndVideoInputList::ImageAndVideoInputList(string name, int id_) : InputSource(name, "Image & Video List", id_){
    
    currentSequence = 0;
    lastSequence = currentSequence;
    isEnabled = false;
    videoPlayer = NULL;
    hasMovie = false;
}

//------------------------------------------------------------------
void ImageAndVideoInputList::setup(){
    
    if (inputs[currentSequence]) {
        
        width  = inputs[currentSequence]->getWidth();
        height = inputs[currentSequence]->getHeight();
        
        inputs[currentSequence]->getFileExtension() == PNG
        ? img.allocate(width, height, OF_IMAGE_COLOR_ALPHA)
        : img.allocate(width, height, OF_IMAGE_COLOR);

        img.setUseTexture(true);
        isEnabledOn = true;
    }
    else {
        drawNoInputs = true;
    }
}

//------------------------------------------------------------------
void ImageAndVideoInputList::update(){
    
    inputs[currentSequence]->update(img);
    playPos2 = inputs[currentSequence]->getPosition();
}

//------------------------------------------------------------------
void ImageAndVideoInputList::updateParameter(Param* inputParam){
    
    if(inputParam->name.compare("BPM")==0){
        this->bpm = inputParam->floatVal;
    }
//    else if(inputParam->name.compare("BPM Multiplier")==0 || inputParam->name.compare("Loop length in beats")==0){
//        isMatchBpmToSequenceLength
//        ? this->bpmMultiplier = ofMap(inputParam->value, inputParam->inputMin, inputParam->inputMax, 0, 16)
//        : this->bpmMultiplier = ofMap(inputParam->value, inputParam->inputMin, inputParam->inputMax, 0, 120);
//    }
    else if(inputParam->name.compare("BPM Multiplier")==0){
        this->bpmMultiplier = inputParam->intVal;
    }
    else if(inputParam->name.compare("Loop length in beats")==0){
        this->bpmMultiplier = inputParam->intVal;
    }
    else if(inputParam->name.compare("Play Position")==0){
        this->playPosition = inputParam->floatVal;
    }
    else if(inputParam->name.compare("POS")==0){
        this->playPos2 = inputParam->floatVal;
    }
    else if(inputParam->name.compare("Current Sequence")==0){
//        this->currentSequence = ofMap(inputParam->value, inputParam->inputMin, inputParam->inputMax, 0, getMidiMax("Current Sequence"));
        this->currentSequence = inputParam->intVal % inputs.size();
    }
    else if(inputParam->name.compare("Play")==0){
        this->isPlaying = inputParam->intVal;
    }
    else if(inputParam->name.compare("Loop Palindrom")==0){
        this->isPalindromLoop = inputParam->intVal;
    }
    else if(inputParam->name.compare("BPM = Seq. Length")==0){
        this->isMatchBpmToSequenceLength = inputParam->intVal;
    }
    else if(inputParam->name.compare("Enabled")==0){
        this->isEnabledOn = inputParam->intVal;
    }
}

//------------------------------------------------------------------
float ImageAndVideoInputList::getMidiMin(string param_){
    
    if(param_.compare("BPM")==0){
        return 10;
    }else if(param_.compare("BPM Multiplier")==0 || param_.compare("Loop length in beats")==0){
        return 0;
    }else if(param_.compare("Loop length in beats")==0){
        return 1;
    }else if(param_.compare("Play Position")==0){
        return 0;
    }else if(param_.compare("Pos")==0){
        return 0;
    }else if(param_.compare("Current Sequence")==0){
        return 0;
    }else if(param_.compare("Play")==0){
        return 0;
    }else if(param_.compare("Loop Palindrom")==0){
        return 0;
    }else if(param_.compare("BPM = Seq. Length")==0){
        return 0;
    }else if(param_.compare("Enabled")==0){
        return 0;
    }
}

//------------------------------------------------------------------
float ImageAndVideoInputList::getMidiMax(string param_){
    
    if(param_.compare("BPM")==0){
        return 200;
    }else if(param_.compare("BPM Multiplier")==0){
        return 120;
    }else if(param_.compare("Loop length in beats")==0){
        return 16;
    }else if(param_.compare("Play Position")==0){
        return 1.0;
    }else if(param_.compare("Pos")==0){
        return 1.0;
    }else if(param_.compare("Current Sequence")==0){
        return inputs.size()-1;
    }else if(param_.compare("Play")==0){
        return 1;
    }else if(param_.compare("Loop Palindrom")==0){
        return 1;
    }else if(param_.compare("BPM = Seq. Length")==0){
        return 1;
    }else if(param_.compare("Enabled")==0){
        return 1;
    }
}

//------------------------------------------------------------------
ofImage* ImageAndVideoInputList::getImage(){
    return &img;
}

//------------------------------------------------------------------
ofTexture* ImageAndVideoInputList::getTexture(){
    return &img.getTextureReference();
}

//------------------------------------------------------------------
void ImageAndVideoInputList::loadImage(string name_, string path_){
    
    if (ofIsStringInString(path_, ".mov") || ofIsStringInString(path_, ".mp4") ||
        ofIsStringInString(path_, ".mpg") || ofIsStringInString(path_, ".mpg") ) {
        
        if (inputs.size() == 0 || videoPlayer == NULL)
            videoPlayer = VideoPool::getInstance()->getPlayer();
        inputs.push_back(new ImageTypeMovie(name_,path_,videoPlayer));
        hasMovie = true;
    }
    //load image sequence
    else if (!ofIsStringInString(path_, ".")) {
        inputs.push_back(new ImageTypePictureSequence(name_,path_));
    }
    //load single image
    else{
        inputs.push_back(new ImageTypePicture(name_,path_));
    }
    currentSequence.setMax(inputs.size()-1);
    
    if (inputs.size() == 1) {
        
        width = inputs[0]->getWidth();
        height = inputs[0]->getHeight();
        
        //create gui
        isPalindromLoop.addListener(this, &ImageAndVideoInputList::loopTypeChanged);
        bpm.addListener(this, &ImageAndVideoInputList::bpmChanged);
        setOriginalPlaySpeed.addListener(this,&ImageAndVideoInputList::setOriginalPlaySpeedChanged);
        bpmMultiplier.addListener(this, &ImageAndVideoInputList::bpmMultiplierChanged);
        nextFrame.addListener(this, &ImageAndVideoInputList::nextFrameChanged);
        previousFrame.addListener(this, &ImageAndVideoInputList::previousFrameChanged);
        isMatchBpmToSequenceLength.addListener(this, &ImageAndVideoInputList::isMatchBpmToSequenceLengthChanged);
        playPosition.addListener(this, &ImageAndVideoInputList::playPositionChanged);
        isPlaying.addListener(this, &ImageAndVideoInputList::isPlayingChanged);
        isEnabledOn.addListener(this, &ImageAndVideoInputList::setEnableChanged);
        
        nextSequence.addListener(this, &ImageAndVideoInputList::nextSequenceChanged);
        prevSequence.addListener(this, &ImageAndVideoInputList::prevSequenceChanged);
        currentSequence.addListener(this, &ImageAndVideoInputList::sequenceChanged);
        deleteCurrentSequence.addListener(this, &ImageAndVideoInputList::deleteSequence);
        addVideoOrImage.addListener(this, &ImageAndVideoInputList::addNewInput);
        
        gui.add(isEnabledOn.set("Enabled",false));
        gui.add(nextSequence.setup(">> next"));
        gui.add(prevSequence.setup("<< prev"));
        gui.add(currentSequence.set("Current Sequence", 0, 0, 0));
        gui.add(deleteCurrentSequence.setup("Delete Current Seq."));
        gui.add(addVideoOrImage.setup("Add Image or Video"));
        
        seqSettings.setName("Sequence Settings");
        seqSettings.add(isPlaying.set("Play",true));
        seqSettings.add(isPalindromLoop.set("Loop Palindrom",true));
        seqSettings.add(isMatchBpmToSequenceLength.set("BPM = Seq. Length",false));
        seqSettings.add(bpm.set("BPM", 100, 10, 200));
        seqSettings.add(bpmMultiplier.set("BPM Multiplier", 4, 1, 120));
        seqSettings.add(playPosition.set("Play Position",0.0,0.0,1.0));
        seqSettings.add(playPos2.set("Pos",0.0,0.0,1.0));
        gui.add(seqSettings);
        
        gui.setWidthElements(INSPECTOR_WIDTH);
        
        //gui.add(nextFrame.setup("nextFrame"));
        //gui.add(previousFrame.setup("previousFrame"));
        isPlayingBackwards = false;
        //start first scene
        inputs[0]->isPlaying = isPlaying;
        ofLoopType l = (isPalindromLoop) ? OF_LOOP_PALINDROME : OF_LOOP_NORMAL;
        inputs[0]->bpm = bpm;
        inputs[0]->bpmMultiplier = bpmMultiplier;
        inputs[0]->isMatchBpmToSequenceLength = isMatchBpmToSequenceLength;
        inputs[0]->isPlayingBackwards = isPlayingBackwards;
        inputs[0]->setLoopState(l);
        inputs[0]->calculateFPS();
        inputs[0]->activate(img);
    }
}

//------------------------------------------------------------------
void ImageAndVideoInputList::loopTypeChanged(bool &b){
    if (b) {
        inputs[currentSequence]->setLoopState(OF_LOOP_PALINDROME);
    }else{
        inputs[currentSequence]->setLoopState(OF_LOOP_NORMAL);
    }
}

//------------------------------------------------------------------
void ImageAndVideoInputList::isPlayingBackwardsChanged(bool &b){
    inputs[currentSequence]->isPlayingBackwards = b;
    inputs[currentSequence]->calculateFPS();
}

//------------------------------------------------------------------
void ImageAndVideoInputList::bpmChanged(float &b){
    inputs[currentSequence]->bpm = b;
    inputs[currentSequence]->calculateFPS();
}

//------------------------------------------------------------------
void ImageAndVideoInputList::bpmMultiplierChanged(int &b){//bpm to fps
    inputs[currentSequence]->bpmMultiplier = b;
    inputs[currentSequence]->calculateFPS();
}

//------------------------------------------------------------------
void ImageAndVideoInputList::isMatchBpmToSequenceLengthChanged(bool &b){
    if (b) {
        bpmMultiplier.set("Loop length in beats", bpmMultiplier, 1, 16 );
    }
    else{
        bpmMultiplier.set("BPM Multiplier", bpmMultiplier, 1, 120);
    }
    inputs[currentSequence]->isMatchBpmToSequenceLength = b;
    inputs[currentSequence]->calculateFPS();
}

//------------------------------------------------------------------
void ImageAndVideoInputList::previousFrameChanged(){
    /*player.previousFrame();
    tex = *player.getTexture();*/
}

//------------------------------------------------------------------
void ImageAndVideoInputList::nextFrameChanged(){
    /*player.nextFrame();
    tex = *player.getTexture();*/
}

//------------------------------------------------------------------
void ImageAndVideoInputList::isPlayingChanged(bool &b){
    inputs[currentSequence]->isPlaying = b;
}

//------------------------------------------------------------------
void ImageAndVideoInputList::setEnableChanged(bool &b){
    this->setEnable(b);
}

//------------------------------------------------------------------
void ImageAndVideoInputList::playPositionChanged(float &pos){
    isPlaying = false;
    inputs[currentSequence]->setPosition(pos,img);
}

//------------------------------------------------------------------
void ImageAndVideoInputList::nextSequenceChanged(){
    if (isEnabled) {
        currentSequence = (currentSequence+1)%inputs.size();
        getNodeViewerIBelong()->updateConnectionsSize(this);
        
        inputs[currentSequence]->getFileExtension() == PNG
        ? img.setImageType(OF_IMAGE_COLOR_ALPHA)
        : img.setImageType(OF_IMAGE_COLOR);
    }
}

//------------------------------------------------------------------
void ImageAndVideoInputList::prevSequenceChanged(){
    if (isEnabled) {
        currentSequence = (currentSequence -1 <0) ? currentSequence = inputs.size()-1 : currentSequence-1;
        getNodeViewerIBelong()->updateConnectionsSize(this);
        
        inputs[currentSequence]->getFileExtension() == PNG
        ? img.setImageType(OF_IMAGE_COLOR_ALPHA)
        : img.setImageType(OF_IMAGE_COLOR);
    }
}

//------------------------------------------------------------------
void ImageAndVideoInputList::sequenceChanged(int &s){
    if (isEnabled && lastSequence != s) {
        inputs[lastSequence]->isPlaying = false;
        lastSequence = currentSequence;
        
        inputs[currentSequence]->activate(img);
        inputs[currentSequence]->isPlaying = isPlaying;
        
        inputs[0]->bpm = bpm;
        inputs[0]->bpmMultiplier = bpmMultiplier;
        inputs[0]->isMatchBpmToSequenceLength = isMatchBpmToSequenceLength;
        inputs[0]->isPlayingBackwards = isPlayingBackwards;
        ofLoopType l = (isPalindromLoop) ? OF_LOOP_PALINDROME : OF_LOOP_NORMAL;
        inputs[0]->setLoopState(l);
        inputs[0]->calculateFPS();
        
//        width  = getBox().width/SCALE_RATIO;
        width  = (textureCorners[1].x - textureCorners[0].x)/SCALE_RATIO;
        height = (width*inputs[currentSequence]->getHeight())/inputs[currentSequence]->getWidth();
        
        resetSize();
    }
}

//------------------------------------------------------------------
void ImageAndVideoInputList::deleteSequence() {
    if (isEnabled && inputs.size() > 1) {
        inputs.erase(inputs.begin() + currentSequence);
        if (currentSequence == inputs.size()) {
            currentSequence--;
        }
        else {
            currentSequence = currentSequence;
        }
        currentSequence.setMax(inputs.size()-1);
    }
}

//------------------------------------------------------------------
void ImageAndVideoInputList::addNewInput() {
    
    bActive = false;
    
    ofFileDialogResult openFileResult = ofSystemLoadDialog("Select an image (.jpg, .jpeg, .png, .bmp or .gif) or video (.mov, .mpg, .mp4 or .m4v)");
    
    if (openFileResult.bSuccess){
        
        ofFile file (openFileResult.getPath());
        
        if (file.exists()){
            
            string fileExtension = ofToUpper(file.getExtension());
            
            //We only want images
            if (fileExtension == "JPG"  ||
                fileExtension == "PNG"  ||
                fileExtension == "JPEG" ||
                fileExtension == "GIF"  ||
                fileExtension == "BMP"  ||
                fileExtension == "MOV"  ||
                fileExtension == "MPG"  ||
                fileExtension == "MP4"  ||
                fileExtension == "M4V" ) {
                
                loadImage(file.getFileName(), openFileResult.getPath());
            }
        }
        file.close();
        bActive = true;
        return;
    }
    else {
        bActive = true;
        return;
    }
}

//------------------------------------------------------------------
void ImageAndVideoInputList::setOriginalPlaySpeedChanged(bool &b){
    if (b) {
      
    }
}

//------------------------------------------------------------------
void ImageAndVideoInputList::setEnable(bool isEnabled_){
    
    isEnabled = isEnabled_;
    
    //all to manage videoplayer
    
    //does it need player
    if (hasMovie) {
        if (isEnabled_ && nEnabled > 0) {
            //nothing
        }
        else if (isEnabled_ && nEnabled == 0)
        {
            videoPlayer = VideoPool::getInstance()->getPlayer();
            
            for (int i=0; i<inputs.size(); ++i) {
                if (inputs[i]->getType() == T_MOVIE) {
                    ImageTypeMovie* t = (ImageTypeMovie*)inputs[i];
                    t->setPlayer(videoPlayer);
                }
            }
            
            inputs[currentSequence]->activate(img);
            inputs[currentSequence]->isPlaying = isPlaying;
        }
        else if (!isEnabled_ && nEnabled>1){
            //nothing
        }
        else{
            VideoPool::getInstance()->releasePlayer(videoPlayer);
        }
    }
    InputSource::setEnable(isEnabled_);
}

//------------------------------------------------------------------
bool ImageAndVideoInputList::loadSettings(ofxXmlSettings &XML, int nTag_, int nodesCount_) {
    
    bool loaded = true;
    
    nId     = XML.getAttribute("NODE", "id", -1, nTag_) + nodesCount_;
    path    = XML.getAttribute("NODE", "path","none", nTag_);
    
    bpm             = ofToFloat(XML.getAttribute("NODE", "bpm","120", nTag_));
    bpmMultiplier   = ofToInt(XML.getAttribute("NODE", "multiplier_divider","32", nTag_));
    isPlaying       = ofToBool(XML.getAttribute("NODE", "isPlaying","true", nTag_));
    isPalindromLoop = ofToBool(XML.getAttribute("NODE", "palindrom","true", nTag_));
    isMatchBpmToSequenceLength = ofToBool(XML.getAttribute("NODE", "matchBPMtoSequence","false", nTag_));
    
    
    if ((path == "none") || (path == "")) {
        XML.pushTag("NODE",nTag_);
        int numVideoTag = XML.getNumTags("ASSET");
        if(numVideoTag>0){
            for (int v=0; v<numVideoTag; v++){
                string name_ = XML.getAttribute("ASSET","name","default",v);
                string path_ = XML.getAttribute("ASSET","path","default",v);
                loadImage(name_, path_);
            }
        }
        else{
            loaded = false;
        }
        XML.popTag();
    }
    else{
        loadImage(name, path);
    }
    
    currentSequence = ofToInt(XML.getAttribute("NODE", "currentSequence", "0", nTag_));
    
    XML.pushTag("NODE", nTag_);
    
    type        = XML.getValue("type","none");
    bVisible    = XML.getValue("visible", true);
        
    ofxPatch::loadSettings(XML, nTag_, nodesCount_);
    
    XML.popTag();
    
    return loaded;
}

//------------------------------------------------------------------
bool ImageAndVideoInputList::saveSettings(ofxXmlSettings &XML) {
    
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
            XML.setAttribute("NODE", "path", path, i);
            
            XML.setAttribute("NODE", "bpm", bpm, i);
            XML.setAttribute("NODE", "multiplier_divider", bpmMultiplier, i);
            XML.setAttribute("NODE", "isPlaying", isPlaying, i);
            XML.setAttribute("NODE", "palindrom", isPalindromLoop, i);
            XML.setAttribute("NODE", "matchBPMtoSequence", isMatchBpmToSequenceLength, i);
            XML.addAttribute("NODE", "currentSequence", currentSequence, i);
            
            XML.pushTag("NODE", i);
            
            if ((path == "none") || (path == "")) {
                
                int numAssetTag = XML.getNumTags("ASSET");
                for (int v = 0; v < numAssetTag; v++){
                    XML.removeTag("ASSET");
                }
                
                for (int v = 0; v < inputs.size(); v++){
                    XML.addTag("ASSET");
                    XML.addAttribute("ASSET", "name", inputs[v]->getName(), v);
                    XML.addAttribute("ASSET", "path", inputs[v]->getPath(), v);
                }
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
            XML.addAttribute("NODE", "type", "IMAGE_AND_VIDEO_LIST", lastPlace);
            XML.addAttribute("NODE", "path", path, lastPlace);
            
            XML.addAttribute("NODE", "bpm", bpm, lastPlace);
            XML.addAttribute("NODE", "multiplier_divider", bpmMultiplier, lastPlace);
            XML.addAttribute("NODE", "isPlaying", isPlaying, lastPlace);
            XML.addAttribute("NODE", "palindrom", isPalindromLoop, lastPlace);
            XML.addAttribute("NODE", "matchBPMtoSequence", isMatchBpmToSequenceLength, lastPlace);
            XML.addAttribute("NODE", "currentSequence", currentSequence, lastPlace);
            
            if (XML.pushTag("NODE", lastPlace)){
                
                if ((path == "none") || (path == "")) {
                    
                    for (int v = 0; v < inputs.size(); v++){
                        XML.addTag("ASSET");
                        XML.addAttribute("ASSET", "name", inputs[v]->getName(), v);
                        XML.addAttribute("ASSET", "path", inputs[v]->getPath(), v);
                    }
                }
                
                ofxPatch::saveSettings(XML, true, lastPlace);
                
                XML.popTag();
            }
        }
    }
    
    return saved;
    
}

//------------------------------------------------------------------
bool ImageAndVideoInputList::saveSettingsToSnippet(ofxXmlSettings &XML, map<int,int> newIdsMap) {
    
    
    bool saved = false;
    int lastPlace = XML.addTag("NODE");
    
    XML.addAttribute("NODE", "id", newIdsMap[nId], lastPlace);
    XML.addAttribute("NODE", "name", name, lastPlace);
    XML.addAttribute("NODE", "type", "IMAGE_AND_VIDEO_LIST", lastPlace);
    XML.addAttribute("NODE", "path", path, lastPlace);
    
    XML.addAttribute("NODE", "bpm", bpm, lastPlace);
    XML.addAttribute("NODE", "multiplier_divider", bpmMultiplier, lastPlace);
    XML.addAttribute("NODE", "isPlaying", isPlaying, lastPlace);
    XML.addAttribute("NODE", "palindrom", isPalindromLoop, lastPlace);
    XML.addAttribute("NODE", "matchBPMtoSequence", isMatchBpmToSequenceLength, lastPlace);
    XML.addAttribute("NODE", "currentSequence", currentSequence, lastPlace);
    
    if (XML.pushTag("NODE", lastPlace)){
        
        if ((path == "none") || (path == "")) {
            
            for (int v = 0; v < inputs.size(); v++){
                
                XML.addTag("ASSET");
                XML.addAttribute("ASSET", "name", inputs[v]->getName(), v);
                XML.addAttribute("ASSET", "path", inputs[v]->getPath(), v);
            }
        }
        
        saved = ofxPatch::saveSettingsToSnippet(XML, lastPlace, newIdsMap);
        
        XML.popTag();
    }
    
    return saved;
}
