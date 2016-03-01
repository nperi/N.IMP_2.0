 #include "ofApp.h"

using namespace cv;
using namespace ofxCv;


/* ================================================ */
/*                      SETUP                       */
/* ================================================ */

void ofApp::setup() {
    
//    ofHideCursor();
    
    //*** WINDOWS SETUP ***//
    //
    glfw = (ofxMultiGLFWWindow*)ofGetWindowPtr();
    // vector of windows, count set in main
    windows = &glfw->windows;
    
    // configure console window
    //
    glfw->setWindow(windows->at(CONSOLE_WINDOW));
    glfw->initializeWindow();
    //glfw->setWindowShape(400,300);
    glfw->setWindowPosition(ofGetWidth()/2 - 200, -ofGetHeight()/2);
    glfw->setWindowTitle("Console");
    
    // configure main window
    //
    glfw->setWindow(windows->at(MAIN_WINDOW));  // set window pointer
    glfw->initializeWindow();                   // initialize events (mouse, keyboard, etc) on window (optional)
    //glfw->setWindowShape(1280,700);
    glfw->setWindowTitle("n.imp");
    //ofSetFullscreen(true);                    // order important with fullscreen
    // ******* END WINDOWS SETUP ******//
    
    
    //*** MAIN SETUP ***//
    //
    ofSetFrameRate(30);
    loadingOK              = false;
    isFullScreen           = false;
    midiLearnActive        = false;
    editLeftAudioInActive  = false;
    editRightAudioInActive = false;
    rightAudioPatch        = NULL;
    leftAudioPatch         = NULL;
    audioAnalizer          = NULL;
    
    console = ConsoleLog::getInstance();
    ofSetLogLevel(OF_LOG_ERROR);
    
    //populating string dictionaries for simple comparison used in LoadFromXML
    inputTypes.insert(std::pair<string,InputType>("AUDIO_ANALIZER",AUDIO_ANALIZER));
    inputTypes.insert(std::pair<string,InputType>("VIDEO",VIDEO));
    inputTypes.insert(std::pair<string,InputType>("CAM",CAM));
    inputTypes.insert(std::pair<string,InputType>("IMAGE",IMAGE));
    inputTypes.insert(std::pair<string,InputType>("PARTICLE",PARTICLE));
    inputTypes.insert(std::pair<string,InputType>("LEFT_AUDIO_IN",LEFT_AUDIO_IN));
    inputTypes.insert(std::pair<string,InputType>("RIGHT_AUDIO_IN",RIGHT_AUDIO_IN));
    visualLayerTypes.insert(std::pair<string,VisualLayerType>("IKEDA", IKEDA));
    visualLayerTypes.insert(std::pair<string,VisualLayerType>("GLITCH_1", GLITCH_1));
    visualLayerTypes.insert(std::pair<string,VisualLayerType>("GLITCH_2", GLITCH_2));
    visualLayerTypes.insert(std::pair<string,VisualLayerType>("IMAGE_PROCESSOR", IMAGE_PROCESSOR));
    mixerTypes.insert(std::pair<string,MixerType>("MASK", MASK));
    mixerTypes.insert(std::pair<string,MixerType>("SIMPLE_BLEND", SIMPLE_BLEND));
    mixerTypes.insert(std::pair<string,MixerType>("MULTI_CHANNEL", MULTI_CHANNEL));
    inputGenTypes.insert(std::pair<string,paramInputType>("MIDI", MIDI));
    inputGenTypes.insert(std::pair<string,paramInputType>("FFT", FFT));
    inputGenTypes.insert(std::pair<string,paramInputType>("OSC", OSC));
    
    
    //*** TRACKPAD SETUP ***//
    //
    pad = ofxMultiTouchPad();
    
    
    //*** CAMERA SETUP ***//
    //
    cam.setDistance(600);
    cam.disableMouseInput();
    cam.enableOrtho();
    cam.setVFlip(true);
    scale = 1.f;
    
    //*** TOP MENU ***//
    //
    menu = new ofxUISuperCanvas("menu", 0, MENU_TOP_PADDING, ofGetWidth(), MENU_HEIGHT);
    menu->getCanvasTitle()->ofxUIWidget::setVisible(false);
    menu->setColorBack(ofxUIColor(140, 140, 140,255));
    ofxUISpacer* spacer;
    
    new menuItem(menu, "MultiImageButton", "New Patcher", "assets/new_file.png", false, RIGHT_MENU_WIDTH, 20);
    new menuItem(menu, "MultiImageButton", "Open Patcher", "assets/open_file.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE + MENU_ITEM_PADDING, 20);
    new menuItem(menu, "MultiImageButton", "Save Patcher", "assets/save_file.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*2 + MENU_ITEM_PADDING*2, 20);
    
    spacer = new ofxUISpacer(RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*3 + MENU_ITEM_PADDING*3, 20, 1,MENU_ITEM_SIZE);
    menu->addWidget(spacer);
    spacer->setColorFill(ofxUIColor(120, 120, 120, 200));
    new menuItem(menu, "MultiImageButton", "Create Node", "assets/node.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*3 + MENU_ITEM_PADDING*4, 20);
    
    spacer = new ofxUISpacer(RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*4 + MENU_ITEM_PADDING*5, 20, 1,MENU_ITEM_SIZE);
    menu->addWidget(spacer);
    spacer->setColorFill(ofxUIColor(120, 120, 120, 200));
    new menuItem(menu, "MultiImageButton", "Save Snippet", "assets/save_snippet.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*4 + MENU_ITEM_PADDING*6, 20);
    new menuItem(menu, "MultiImageButton", "Open Snippet", "assets/open_snippet.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*5 + MENU_ITEM_PADDING*7, 20);
    
    spacer = new ofxUISpacer(RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*6 + MENU_ITEM_PADDING*8, 20, 1,MENU_ITEM_SIZE);
    menu->addWidget(spacer);
    spacer->setColorFill(ofxUIColor(120, 120, 120, 200));
    new menuItem(menu, "MultiImageToggle", "Straight Links", "assets/line.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*6 + MENU_ITEM_PADDING*9, 20);
    new menuItem(menu, "MultiImageToggle", "Curved Links", "assets/curve_line.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*7 + MENU_ITEM_PADDING*10, 20);
    new menuItem(menu, "MultiImageToggle", "Segmented Links", "assets/path_line.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*8 + MENU_ITEM_PADDING*11, 20);
    
    spacer = new ofxUISpacer(RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*9 + MENU_ITEM_PADDING*12, 20, 1,MENU_ITEM_SIZE);
    menu->addWidget(spacer);
    spacer->setColorFill(ofxUIColor(120, 120, 120, 200));
    new menuItem(menu, "MultiImageToggle", "Edit Mode on/off", "assets/edit_mode.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*9 + MENU_ITEM_PADDING*13, 20);
    
    spacer = new ofxUISpacer(RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*10 + MENU_ITEM_PADDING*14, 20, 1,MENU_ITEM_SIZE);
    menu->addWidget(spacer);
    spacer->setColorFill(ofxUIColor(120, 120, 120, 200));
    new menuItem(menu, "MultiImageToggle", "Console on/off", "assets/console.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*10 + MENU_ITEM_PADDING*15, 20);
    new menuItem(menu, "MultiImageButton", "Clear Console", "assets/clear_console.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*11 + MENU_ITEM_PADDING*16, 20);
    
    spacer = new ofxUISpacer(RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*12 + MENU_ITEM_PADDING*17, 20, 1,MENU_ITEM_SIZE);
    menu->addWidget(spacer);
    spacer->setColorFill(ofxUIColor(120, 120, 120, 200));
    new menuItem(menu, "MultiImageButton", "Encapsulate", "assets/encapsulate.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*12 + MENU_ITEM_PADDING*18, 20);
    new menuItem(menu, "MultiImageButton", "Uncapsulate", "assets/uncapsulate.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*13 + MENU_ITEM_PADDING*19, 20);
    new menuItem(menu, "MultiImageButton", "Open encapsulated", "assets/open_encapsulated.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*14 + MENU_ITEM_PADDING*20, 20);
    
    spacer = new ofxUISpacer(RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*15 + MENU_ITEM_PADDING*21, 20, 1,MENU_ITEM_SIZE);
    menu->addWidget(spacer);
    spacer->setColorFill(ofxUIColor(120, 120, 120, 200));
    new menuItem(menu, "MultiImageToggle", "Midi Learn", "assets/midi_learn.png", false, RIGHT_MENU_WIDTH + MENU_ITEM_SIZE*15 + MENU_ITEM_PADDING*22, 20);
    
    ofAddListener(menu->newGUIEvent,this,&ofApp::menuEvent);
    
    
    //*** RIGHT MENU ***//
    //
    right_menu = new ofxUISuperCanvas("menu", 0, MENU_HEIGHT + MENU_TOP_PADDING, RIGHT_MENU_WIDTH, ofGetHeight() - (MENU_HEIGHT + MENU_TOP_PADDING));
    right_menu->getCanvasTitle()->ofxUIWidget::setVisible(false);
    right_menu->setColorBack(ofxUIColor(140, 140, 140,255));
    
    new menuItem(right_menu, "MultiImageButton", "Zoom In", "assets/zoom_in.png", false, 3, right_menu->getRect()->getHeight()-30);
    new menuItem(right_menu, "MultiImageButton", "Zoom Out", "assets/zoom_out.png", false, 3, right_menu->getRect()->getHeight()-60);
    new menuItem(right_menu, "MultiImageToggle", "Analizer", "assets/analizer.png", true, 3, right_menu->getRect()->getHeight()-100);
    
    ofAddListener(right_menu->newGUIEvent,this,&ofApp::menuEvent);
    
    
    //*** MAIN CANVAS ***//
    //
    gui = new ofxUISuperCanvas("", RIGHT_MENU_WIDTH, MENU_HEIGHT + MENU_TOP_PADDING, ofGetWidth() - RIGHT_MENU_WIDTH, ofGetHeight() - (MENU_HEIGHT +MENU_TOP_PADDING));
    gui->setColorBack(ofxUIColor(255,255,255,0));
    gui->setDraggable(false);

    
    //*** LOADING NODES FROM XML ***//
    //
    ofDrawBitmapString("LOADING XML ...", 50, 50);
    loadingOK = loadFromXML();
    
    if(loadingOK){
        
        ConsoleLog::getInstance()->pushMessage("All nodes loaded correctly.");
        
        //TODO: change mixtable assignment.
        //  ofAddListener(mixtables[0]->textureEvent, this, &ofApp::updateSyphon);
        
        //*** invoking input generators setup functions  ***//
        //
        for(int i=0; i<inputGenerators.size(); i++){
            inputGenerators[i]->setup();
        }
        
        //*** invoking nodes setup functions and setting main canvas and camera  ***//
        //
        for (int i=0; i<nodesVector.size(); ++i) {
            initNode(nodesVector[i]);
        }
        
        //*** SYPHON SERVERS SETUP ***//
        //
        for(int i=0; i<syphonServers.size();i++){
            syphonServers[i]->setup();
        }
        
        //*** NODE VIEWR SETUP (COMPOSER) ***//
        //
        setCurrentViewer(0);
        nodeViewers[currentViewer]->setParent(cam);
        gui->setMainComposer(nodeViewers[currentViewer]);
        
        //*** SCROLL BAR SETUP ***//
        //
        this->scrollBars = new scrollBar(nodeViewers[currentViewer], &this->pad, &cam, SCROLL_BAR_EVENT_PRIORITY);
        scrollBars->setup();
        
        //*** AUDIO SETUP ***//
        //
        setupAudio();
        
        //*** starting input generators theads (the not threadeds will not start)  ***//
        //
        for(int i=0; i<inputGenerators.size(); i++){
            inputGenerators[i]->start();
        }
        
        //*** SYPHON INPUT SERVER ***//
        //
        SyphonClientHandler::getInstance();
        
    }
    
    //*** CONSOLE LOGGER ***//
    //
    glfw->setWindow(windows->at(CONSOLE_WINDOW));
    console->setupScrollBar(&pad);
    glfw->iconify(!showConsole);
    
    glfw->setWindow(windows->at(MAIN_WINDOW));
  }

//------------------------------------------------------------------
void ofApp::setupAudio(){
    
    bufferCounter	= 0;
   
    // 0 output channels,
    // 2 input channels
    // 44100 samples per second
    // BUFFER_SIZE samples per buffer
    // 4 num buffers (latency)
    
    ofSoundStreamSetup(0, 2, this, 44100, BUFFER_SIZE, 4);
    left = new float[BUFFER_SIZE];
    right = new float[BUFFER_SIZE];
    
    if (leftAudioPatch != NULL) {
        leftAudioPatch->getWaveForm()->setBuffer(left);
    }
    if (rightAudioPatch != NULL) {
        rightAudioPatch->getWaveForm()->setBuffer(right);
    }
    
    // setting nodes attributes selected for audio in
    //
    vector<DTAudioMap*>* am;
    bool left_ = false;
    std::map<int, ImageOutput*>::iterator node_;
    for (int i = 0; i < audioListeners.size(); i++){
        am = audioListeners[i]->getAudioMap();
        left_ = ((AudioIn*)nodes.find(audioListeners[i]->getNodeID())->second)->getAudioInType() == LEFT;
        for (int j = 0; j < am->size(); j++){
            node_ = nodes.find(am->at(j)->nodeId);
            if (node_ != nodes.end()) {
                node_->second->setAttributesForAudioIn(am->at(j)->paramId, left_);
            }
        }
    }
    
    //soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
}
/* ================================================ */
/* ================================================ */


/* ================================================ */
/*                      LOOPS                       */
/* ================================================ */

void ofApp::update() {
    
    if(loadingOK){
        
        //if pressing zoom in/out buttons
        if (menu_zoom_in) {
            scale = cam.getScale().x;
            scale -= 10*SCALE_SENSITIVITY;
            cam.setScale(scale);
        }
        else if (menu_zoom_out) {
            scale = cam.getScale().x;
            scale += 10*SCALE_SENSITIVITY;
            cam.setScale(scale);
        }
    
        //resetting processed flags
//        for (int i=0; i<nodesVector.size(); ++i) {
//            //nodesVector[i]->resetProcessedFlag();
//            nodesVector[i]->update();
//        }
        
        //getting new paraminputs
        for (int i=0; i<inputGenerators.size(); ++i) {
            ParamInputGenerator* p = inputGenerators[i];
            
            int maxParams = 0;
            bool continueProcessing = true;
            while(maxParams<=5 && continueProcessing){
                Param* param = p->getNextInputMessage();
                
                //sending param info to the destination node
                if(param != NULL){
                    
                    if (p->getParamInputType() == MIDI && midiLearnActive) {
                        map<int, vector<string> > attributesSelected = nodeViewers[currentViewer]->getAttributesSelectedForMidiLearn();
                        std::map<int, ImageOutput*>::iterator node_;
                        
                        for(map<int, vector<string> >::iterator it = attributesSelected.begin(); it != attributesSelected.end(); it++ ){
                            node_ = nodes.find(it->first);
                            if (node_ != nodes.end()) {
                                ((MidiInputGenerator*)p)->addNewMidiMap(param->midiControl, node_->second, it->second);
                            }
                        }
                    }
                    else if (!midiLearnActive){
                        ImageOutput* destinationNode = NULL;
                        std::map<int,ImageOutput*>::iterator it;

                        it = nodes.find(param->imageInputId);
                        
                        if(it!=nodes.end()){
                            it->second->updateParameter(param);
                            delete param;
                        }
                        maxParams += 1;
                    }
                }
                else{
                    continueProcessing = false;
                }
            }
        }

        //delete removed widgets
        if (widgetsToDelete.size() > 0) {
            for(auto widget : widgetsToDelete) {
                if (newNodeInput == widget)
                    newNodeInput = NULL;
                gui->removeWidget(widget);
            }
            widgetsToDelete.clear();
        }
    
        //update right menu menuitems in case window have been resized
        right_menu->getWidget("Zoom In")->getRect()->setY(right_menu->getRect()->getHeight()-30);
        right_menu->getWidget("Zoom Out")->getRect()->setY(right_menu->getRect()->getHeight()-60);
        right_menu->getWidget("Analizer")->getRect()->setY(right_menu->getRect()->getHeight()-100);
        
        scrollBars->update();
        nodeViewers[currentViewer]->update();
        
        for(int i=0; i<syphonServers.size();i++){
            syphonServers[i]->publishTexture();
        }
        
        
        // hide/show console
        if(glfwWindowShouldClose(windows->at(CONSOLE_WINDOW))){
            glfw->hideWindow(windows->at(CONSOLE_WINDOW));
            glfwSetWindowShouldClose(windows->at(CONSOLE_WINDOW), 0);
            ((ofxUIImageToggle*)menu->getWidget("Console on/off"))->setValue(false);
            showConsole = !showConsole;
        }
        
        // destroy encapsulated windows
        for(int i=2; i < windows->size(); i++){
            encapsulatedWindowsScrollBars.at(i-2)->update();
            
            if(glfwWindowShouldClose(windows->at(i))){
                nodeViewers[currentViewer]->setParent(cam);
                nodeViewers[currentViewer]->setCameraForWindow(i, cam);
                nodeViewers[currentViewer]->restoreWindowsForEncapsulated(i);
                
                glfw->destroyWindow(windows->at(i));
                windows->erase(windows->begin() + i);
                delete encapsulatedWindowsScrollBars.at(i-2);
                delete encapsulatedWindowsCameras.at(i-2);
                encapsulatedWindowsScrollBars.erase(encapsulatedWindowsScrollBars.begin() + i - 2);
                encapsulatedWindowsCameras.erase(encapsulatedWindowsCameras.begin() + i -2);
            }
        }
    }
}

//------------------------------------------------------------------
void ofApp::draw() {
    wIndex = glfw->getWindowIndex();
    
    // set in global variable the id of the windows which is being drawn
    EventHandler::getInstance()->setWindowIdDraw(wIndex);
    
    switch (wIndex) { // switch on window index
        case MAIN_WINDOW:
            //create bg
            ofClear(35);
            
            // draw nodes
            if(loadingOK){
                
                cam.begin();
                nodeViewers[currentViewer]->draw();
                cam.end();
                
//                string info = "";
//                info += "  (";
//                info += ofToString(currentViewer+1);
//                info += "/";
//                info += ofToString(nodeViewers.size());
//                info += ")";
//                info += "    switch layers <- ->";
//                
//                ofDrawBitmapString(info, 20, ofGetHeight()-20);
//                string info2 = nodeViewers[currentViewer]->getName();
//                ofDrawBitmapString(ofToString(info2,0), ofGetWidth() - 300, ofGetHeight()-20);
                
                ofDrawBitmapString(ofToString(ofGetFrameRate(),0), ofGetWidth() - 50, ofGetHeight()-35);
                
                
                //update menu's width and height
                menu->setWidth(ofGetWidth());
                right_menu->setHeight(ofGetHeight() - (MENU_HEIGHT + MENU_TOP_PADDING));
                
                //draw inspectors
                nodeViewers[currentViewer]->drawInspectorGUIs();
                
                //draw scrollbars
                scrollBars->draw();
            }
            else{
                ofDrawBitmapString("ERROR LOADING XML", 50, 50);
            }
            break;
        case CONSOLE_WINDOW:
            ofBackground(0,0,0); // change background color on each window
            ofSetColor(200, 200, 200);
            console->printMessages();
            break;
        default:
//            glfw->ofAppBaseWindow::showCursor();
            ofClear(35);
            encapsulatedWindowsCameras[wIndex-2]->begin();
            nodeViewers[currentViewer]->draw();
            encapsulatedWindowsCameras[wIndex-2]->end();
            
            //draw inspectors
            nodeViewers[currentViewer]->drawInspectorGUIs();
            
            encapsulatedWindowsScrollBars.at(wIndex-2)->draw();

            break;
    }
}

void ofApp::updateSyphon(ofFbo & img){
    if(loadingOK){
        
    }
}

//------------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    // samples are "interleaved"
    for (int i = 0; i < bufferSize; i++){
        left[i] = input[i*2];
        right[i] = input[i*2+1];
    }
    
    for (int i=0; i<audioListeners.size(); i++){
        // for each audio listener, i copy the audiobuffer and send it to it.
        
        float * leftToGo = new float[BUFFER_SIZE];
        float * rightToGo = new float[BUFFER_SIZE];
        
        memcpy ( leftToGo, left, sizeof (float)*BUFFER_SIZE );
        memcpy ( rightToGo, right, sizeof (float)*BUFFER_SIZE );
        
        if(!audioListeners[i]->fillNewData(leftToGo, rightToGo, BUFFER_SIZE)){
            // if data copy was not successful, then we delete the memory (because the input gen wont delete it)
            delete[] leftToGo;
            delete[] rightToGo;
        }
    }
    bufferCounter++;
    
    audioAnalizer->analyze(input);
}
/* ================================================ */
/* ================================================ */


/* ================================================ */
/*                     EVENTS                       */
/* ================================================ */

void ofApp::keyPressed  (int key){
    bool notAvailable = false;
    switch(key){
            //switch Node Views
        case OF_KEY_LEFT:
            previousViewer();
            break;
        case OF_KEY_RIGHT:
            nextViewer();
            break;
        case '1':
            setCurrentViewer(0);
            break;
        case '2':
            setCurrentViewer(1);
            break;
        case '3':
            setCurrentViewer(2);
            break;
        case '4':
            setCurrentViewer(3);
            break;
        case '5':
            setCurrentViewer(4);
            break;
        case '6':
            setCurrentViewer(5);
            break;
        case '7':
            setCurrentViewer(6);
            break;
        case '8':
            setCurrentViewer(7);
            break;
        case '9':
            setCurrentViewer(8);
            break;
        case '0':
            setCurrentViewer(9);
            break;
        case 'f':
            isFullScreen = !isFullScreen;
            ofSetFullscreen(isFullScreen);
            break;
        case OF_KEY_DEL :
            if (((ofxUITextInput*) newNodeInput)->isClicked()) {
                gui->removeWidget(newNodeInput);
                newNodeInput = NULL;
            }
            break;
        case 'n': case 'N' :
            if (newNodeInput == NULL)
                this->createNodeInput();
            else if (!newNodeInput->isClicked()){
                newNodeInput->getRect()->setX(ofGetMouseX());
                newNodeInput->getRect()->setY(ofGetMouseY());
                
                newNodeInput->getDropdownList()->getRect()->setX(ofGetMouseX());
                newNodeInput->getDropdownList()->getRect()->setY(ofGetMouseY());
            }
            break;
            
            case 'j': case 'J' :
                if ((newNodeInput == NULL) || (!newNodeInput->isClicked())) {
                    loadSnippet();
                }
            break;
            case 'k': case 'K' :
                if ((newNodeInput == NULL) || (!newNodeInput->isClicked())) {
                    saveSnippet();
                }
            break;
        default:
            notAvailable = true;
            break;
    }
    if (notAvailable) ofLogNotice() << "key function not available";
    
    // sending keyboard input to all input generators
    for (int i = 0; i<inputGenerators.size();i++){
        inputGenerators[i]->keyPressed(key);
    }
}

//------------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    switch(key){
        case 'n': case 'N' :
            if (newNodeInput != NULL && !newNodeInput->isClicked())
                newNodeInput->setFocus(true);
        break;
    }
}

//------------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    for(int i = 0; i < windows->size(); i++) {
        if (glfw->getEventWindow() == windows->at(i)) {
            EventHandler::getInstance()->setWindowEvent(i);
            break;
        }
    }
    
//    if(do_zoom){
//        ofVec3f mouse = ofVec3f(x, y,0);
//        ofVec3f mouseLast = ofVec3f(ofGetPreviousMouseX(),ofGetPreviousMouseY(),0);
//        float dify = mouse.y - mouseLast.y;
//        scale += dify*SCALE_SENSITIVITY;
//        cam.setScale(scale);
//    }
}

//------------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    for(int i = 0; i < windows->size(); i++) {
        if (glfw->getEventWindow() == windows->at(i)) {
            EventHandler::getInstance()->setWindowEvent(i);
            if(i == MAIN_WINDOW){
                nodeViewers[currentViewer]->setCameraForWindow(MAIN_WINDOW, cam);
                nodeViewers[currentViewer]->setParent(cam);
            } else if( i != CONSOLE_WINDOW){
                nodeViewers[currentViewer]->setCameraForWindow(i, cam);
                nodeViewers[currentViewer]->setParent(*(encapsulatedWindowsCameras.at(i-2)));
                cout << endl << "setting camera in other window" << endl;
            }
                
            break;
        }
    }

}

//------------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    for(int i = 0; i < windows->size(); i++) {
        if (glfw->getEventWindow() == windows->at(i)) {
            EventHandler::getInstance()->setWindowEvent(i);
            break;
        }
    }
    
    menu_zoom_in = false;
    menu_zoom_out = false;
}

//------------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    for(int i = 0; i < windows->size(); i++) {
        if (glfw->getEventWindow() == windows->at(i)) {
            EventHandler::getInstance()->setWindowEvent(i);
            break;
        }
    }
    
    if( dragInfo.files.size() > 0 ){
        for(int i = 0; i < dragInfo.files.size(); i++){
            
            textInputEvent* ev = new textInputEvent();
            ofFile file = ofFile(dragInfo.files[i]);
            string ext = file.getExtension();
            
            if ((ext == "jpg") || (ext == "JPG") ||
                (ext == "jpeg")|| (ext == "JPEG")||
                (ext == "png") || (ext == "PNG") ||
                (ext == "gif") || (ext == "GIF") ||
                (ext == "bmp") || (ext == "BMP") ){
                
                ev->type = "image";
            }
            else if ((ext == "mov") || (ext == "MOV") ||
                     (ext == "mpg") || (ext == "MPG") ||
                     (ext == "mp4") || (ext == "MP4") ||
                     (ext == "m4v") || (ext == "M4V") ){
                
                 ev->type = "video player";
            }
            else {
                console->pushError("Not valid format to create a node.");
                return;
            }
            
            for (int j = 0; j < inputs.size(); j ++) {
                if (inputs[j]->getTypeName() == "Image" && inputs[j]->isOver(dragInfo.position)){
                    ((ImageInputList*)inputs[j])->loadImage(file.getFileName(), dragInfo.files[i]);
                    console->pushSuccess("File " + file.getFileName()
                                         + " was succesfully added as a new sequence to the node "
                                         + inputs[j]->getName() + ".");
                    file.close();
                    return;
                }
            }
            
            ev->path   = dragInfo.files[i];
            ev->point  = dragInfo.position;
            ev->name   = file.getFileName();
            ev->widget = NULL;
            createNode(*ev);
            file.close();
        }
    }
}

//------------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    ofShowCursor();

}

//------------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    for(int i = 0; i < windows->size(); i++) {
        if (glfw->getEventWindow() == windows->at(i)) {
            EventHandler::getInstance()->setWindowEvent(i);
            break;
        }
    }
}

//------------------------------------------------------------------
void ofApp::menuEvent(ofxUIEventArgs &e) {
    
    string name = e.getName();
    if (name == "Straight Links") {
        ((ofxUIImageToggle*)menu->getWidget("Straight Links"))->setValue(true);
        ((ofxUIImageToggle*)menu->getWidget("Segmented Links"))->setValue(false);
        ((ofxUIImageToggle*)menu->getWidget("Curved Links"))->setValue(false);
        nodeViewers[currentViewer]->setLinkType(STRAIGHT_LINKS);
    }
    else if (name == "Curved Links") {
        ((ofxUIImageToggle*)menu->getWidget("Curved Links"))->setValue(true);
        ((ofxUIImageToggle*)menu->getWidget("Segmented Links"))->setValue(false);
        ((ofxUIImageToggle*)menu->getWidget("Straight Links"))->setValue(false);
        nodeViewers[currentViewer]->setLinkType(CURVE_LINKS);
    }
    else if (name == "Segmented Links") {
        ((ofxUIImageToggle*)menu->getWidget("Segmented Links"))->setValue(true);
        ((ofxUIImageToggle*)menu->getWidget("Curved Links"))->setValue(false);
        ((ofxUIImageToggle*)menu->getWidget("Straight Links"))->setValue(false);
        nodeViewers[currentViewer]->setLinkType(PATH_LINKS);
    }
    else if (name == "Create Node") {
        
        if (newNodeInput == NULL) {
            this->createNodeInput((ofGetWidth()/2)-75, ofGetHeight()/2);
        }
        else if (!newNodeInput->isClicked()){
            newNodeInput->getRect()->setX((ofGetWidth()/2)-75);
            newNodeInput->getRect()->setY(ofGetHeight()/2);
            
            newNodeInput->getDropdownList()->getRect()->setX((ofGetWidth()/2)-75);
            newNodeInput->getDropdownList()->getRect()->setY(ofGetHeight()/2);
            
            newNodeInput->setFocus(true);
        }
    }
    else if (name == "Edit Mode on/off") {
        
        if (nodeViewers[currentViewer]->getEdit())
            nodeViewers[currentViewer]->setEdit(false);
        else nodeViewers[currentViewer]->setEdit(true);
    }
    /*else if (name == "Inspect"){
     if (open_flyout) open_flyout = false;
     else open_flyout = true;
     }*/
    else if (name == "Zoom In"){
        
        if(((ofxUIMultiImageButton*)e.widget)->getValue() == 1){
            menu_zoom_in = true;
        }
    }
    else if (name == "Zoom Out"){
        
        if(((ofxUIMultiImageButton*)e.widget)->getValue() == 1){
            menu_zoom_out = true;
        }
    }
    else if (name == "Analizer"){
        
        if(((ofxUIMultiImageToggle*)e.widget)->getValue() == false){
            audioAnalizer->setDrawAudioAnalizer(false);
            audioAnalizer->setDrawInspector(false);
        }
        else {
            audioAnalizer->setDrawAudioAnalizer(true);
        }
    }
    else if (name == "Save Snippet"){
        if(((ofxUIMultiImageButton*)e.widget)->getValue() == 1){
            saveSnippet();
        }
    }
    else if (name == "Open Snippet"){
        if(((ofxUIMultiImageButton*)e.widget)->getValue() == 1){
            loadSnippet();
        }
    }
    else if (name == "Save Patcher"){
        if(((ofxUIMultiImageButton*)e.widget)->getValue() == 1){
            saveToXML();
        }
    }
    else if (name == "Console on/off"){
        if(showConsole){
            glfw->hideWindow(windows->at(CONSOLE_WINDOW));
        }else{
            glfw->showWindow(windows->at(CONSOLE_WINDOW));
            glfw->setWindow(windows->at(CONSOLE_WINDOW));
        }
        showConsole = !showConsole;
    }
    else if (name == "Clear Console"){
        console->clearMessages();
    }
    
    else if (name == "Encapsulate"){
        if(((ofxUIMultiImageButton*)e.widget)->getValue() == 1){
            int encapsulatedId = nodeViewers[currentViewer]->encapsulate();
            encapsulatedIds.push_back(encapsulatedId);
        }
    }
    
    else if (name == "Uncapsulate"){
        if(((ofxUIMultiImageButton*)e.widget)->getValue() == 1){
            int encapsulatedId = nodeViewers[currentViewer]->getSelectedEncapsulated();
            if(encapsulatedId < 0){
                console->pushError("No encapsulated patch is selected");
            } else {
                int lastPatchId = nodeViewers[currentViewer]->getLastPatchEncapsulated(encapsulatedId);
                nodeViewers[currentViewer]->restoreOutputEncapsulated(lastPatchId);
                nodeViewers[currentViewer]->uncapsulate(encapsulatedId);
                for (std::vector<int>::iterator it = encapsulatedIds.begin() ; it != encapsulatedIds.end(); ++it){
                    if(*it == encapsulatedId){
                        encapsulatedIds.erase(it);
                        break;
                    }
                }
            }
        }
    }
    else if (name == "Open encapsulated"){
        if(((ofxUIMultiImageButton*)e.widget)->getValue() == 1){
            int encapsulatedId = nodeViewers[currentViewer]->getSelectedEncapsulated();
            if(encapsulatedId < 0){
                console->pushError("No encapsulated patch is selected");
            } else {
                glfw->createWindow();
                glfw->setWindow(windows->at(windows->size()-1));
                glfw->initializeWindow();
                
                nodeViewers[currentViewer]->setWindowsIdForEncapsulated(encapsulatedId, windows->size() - 1);
                
                ofSetWindowPosition(ofGetWindowPositionX(), ofGetWindowPositionX());
                ofSetWindowShape(ofGetWidth(), ofGetHeight());
                ofSetWindowTitle(nodeViewers[currentViewer]->getLastEncapsulatedName(encapsulatedId));
                
                ofEasyCam* newCam = new ofEasyCam();
                newCam->setDistance(cam.getDistance());
                newCam->disableMouseInput();
                newCam->enableOrtho();
                newCam->setVFlip(true);
                newCam->setScale(cam.getScale());
                newCam->setPosition(cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);
                encapsulatedWindowsCameras.push_back(newCam);
                
                nodeViewers[currentViewer]->setParent(*newCam);
                nodeViewers[currentViewer]->setCameraForWindow(windows->size()-1, *newCam);
                
                scrollBar* newScroll = new scrollBar(nodeViewers[currentViewer], &this->pad, newCam, SCROLL_BAR_EVENT_PRIORITY, windows->size()-1);
                newScroll->setup();
                encapsulatedWindowsScrollBars.push_back(newScroll);
            }
        }
    }
    else if (name == "Midi Learn") {
        midiLearnActive = !midiLearnActive;
        nodeViewers[currentViewer]->setMidiLearnActive(midiLearnActive);
        
        int i = 0;
        while (i < inputGenerators.size() && inputGenerators[i]->getParamInputType() != MIDI) {
            i++;
        }
        if (i < inputGenerators.size()) {
            ((MidiInputGenerator*)inputGenerators[i])->setMidiLearnActive(midiLearnActive);
        }
    }
}
/* ================================================ */
/* ================================================ */


/* ================================================ */
/*                 OTHER FUNCTIONS                  */
/* ================================================ */

void ofApp::nextViewer(){
    int tViewer = currentViewer;
    ++tViewer;
    tViewer %= nodeViewers.size();
    setCurrentViewer(tViewer);
}

//------------------------------------------------------------------
void ofApp::previousViewer(){
    int tViewer = currentViewer;
    tViewer = (tViewer == 0) ? nodeViewers.size()-1 : --tViewer;
    setCurrentViewer(tViewer);
}

//------------------------------------------------------------------
void ofApp::setCurrentViewer(int currentViewer_){
    if (currentViewer_ >= 0 && currentViewer_ < nodeViewers.size()) {
        currentViewer = currentViewer_;
        
        //reset the gui positions
        //
        //nodeViewers[currentViewer]->setupGuiPositions();
    }else{
        ofLogNotice() << "choosen viewer not available";
    }
}

//------------------------------------------------------------------
void ofApp::createNodeInput(float _x, float _y){
    
    textInput *node = new textInput("", "", 210, 20, _x, _y);
    vector<string> nodes;
    ofxUIDropDownList *dlist = new ofxUIDropDownList("", nodes, 210, _x, _y);
    
    gui->addWidget(dlist);
    gui->addWidget(node);
    
    node->setColorBack(ofxUIColor (80,80,80,200));
    node->setColorOutlineHighlight(ofxUIColor(150,150,250));
    node->setDropdownList(dlist);
    
    ofAddListener( node->createNode , this, &ofApp::createNode);
    
    newNodeInput = node;
}

//------------------------------------------------------------------
void ofApp::createNode(textInputEvent &args){
    
    NodeElement* nE;
    ImageOutput* newPatch = NULL;
    ofVec3f position(args.point.x, args.point.y, 0.0);
    position = position*cam.getGlobalTransformMatrix();
    bool exist = false;
    
    if (args.type == "audio in - left channel") {
        if (leftAudioPatch) {
            nodeViewers[currentViewer]->deactivateAllPatches();
            leftAudioPatch->bActive = true;
            exist = true;
        }
        else {
            newPatch = new AudioIn(gui, left, "Audio In - Left Channel", "New Audio In - Left Channel");
            ((AudioIn*)newPatch)->setChannel(0);
            inputs.push_back((AudioIn*)newPatch);
            ofAddListener( ((AudioIn*)newPatch)->editAudioIn , this, &ofApp::editLeftAudioIn);
//            ofAddListener( ((AudioIn*)newPatch)->editAudioInChannel , this, &ofApp::editAudioInChannel);
            leftAudioPatch = (AudioIn*)newPatch;
        }
    }
    else if (args.type == "audio in - right channel") {
        if (rightAudioPatch) {
            nodeViewers[currentViewer]->deactivateAllPatches();
            rightAudioPatch->bActive = true;
            exist = true;
        }
        else {
            newPatch = new AudioIn(gui, right, "Audio In - Right Channel", "New Audio In - Right Channel");
            ((AudioIn*)newPatch)->setChannel(1);
            inputs.push_back((AudioIn*)newPatch);
            ofAddListener( ((AudioIn*)newPatch)->editAudioIn , this, &ofApp::editRightAudioIn);
//            ofAddListener( ((AudioIn*)newPatch)->editAudioInChannel , this, &ofApp::editAudioInChannel);
            rightAudioPatch = (AudioIn*)newPatch;
        }
    }
    else if (args.type == "camera") {
        newPatch = new InputCamera();
        inputs.push_back((InputCamera*)newPatch);
    }
    else if (args.type == "glitch 1") {
        newPatch = new GlitchLayer();
        visualLayers.push_back((GlitchLayer*)newPatch);
    }
    else if (args.type == "glitch 2") {
        newPatch = new GlitchLayerAlt();
        visualLayers.push_back((GlitchLayerAlt*)newPatch);
    }
    else if (args.type == "ikeda") {
        newPatch = new IkedaLayer();
        visualLayers.push_back((IkedaLayer*)newPatch);
    }
    else if (args.type == "image") {
        newPatch = new ImageInputList();
        ((ImageInputList*)newPatch)->loadImage(args.name, args.path);
        inputs.push_back((ImageInputList*)newPatch);
    }
    else if (args.type == "image processor") {
        newPatch = new ImageProcessor();
        visualLayers.push_back((ImageProcessor*)newPatch);
    }
    else if (args.type == "mix mask") {
        newPatch = new MixMask();
        mixtables.push_back((MixMask*)newPatch);
    }
    else if (args.type == "mix simple blend") {
        newPatch = new MixSimpleBlend();
        mixtables.push_back((MixSimpleBlend*)newPatch);
    }
    else if (args.type == "multi channel switch") {
        newPatch = new MultiChannelSwitch();
        mixtables.push_back((MultiChannelSwitch*)newPatch);
    }
    else if (args.type == "particle generator") {
        newPatch = new ParticleGenerator();
        inputs.push_back((ParticleGenerator*)newPatch);
    }
    else if (args.type == "video player") {
        newPatch = new VideoPlayerMac();
        ((VideoPlayerMac*)newPatch)->loadVideo(args.path);
        inputs.push_back((VideoPlayerMac*)newPatch);
    }
    else if (args.type == "syphon server"){
        newPatch = SyphonClientHandler::getInstance()->createSyphonPatch();
        inputs.push_back((InputSyphon*)newPatch);
    }
    
    if (newPatch && !exist) {
        nE = new NodeElement(newPatch);
        nodeViewers[currentViewer]->addElement(nE, position);
        nodes.insert(std::pair<int, ImageOutput*>(newPatch->getId(), newPatch));
        nodesVector.push_back(newPatch);
        initNode(newPatch);
        newPatch->resetSize();
        
        if (newPatch->getIsAudio()){
            
            AudioInputGenerator* aI = new AudioInputGenerator(newPatch->getName(), newPatch->getId());
//            aI->loadSettings(XML);
            aI->setChannel(((AudioIn*)newPatch)->getChannel());
            
            inputGenerators.push_back(aI);
            audioListeners.push_back(aI);
            aI->setup();
            aI->start();
        }
    }
    if (args.widget != NULL) {
        ofRemoveListener(((textInput*)args.widget)->createNode , this, &ofApp::createNode);
        widgetsToDelete.push_back(args.widget); // delete input from canvas
    }
}

//------------------------------------------------------------------
void ofApp::initNode(ofxPatch* node) {
    node->setParent(cam);
    node->setup();
    node->setLinkType(nodeViewers[currentViewer]->getLinkType());
    ofAddListener( node->title->close , this, &ofApp::closePatch);
}

//------------------------------------------------------------------
void ofApp::closePatch(int &_nID) {
    
    if (audioAnalizer->getId() == _nID) {
        audioAnalizer->setDrawAudioAnalizer(false);
        audioAnalizer->setDrawInspector(false);
        ((ofxUIMultiImageToggle*)right_menu->getWidget("Analizer"))->setValue(false);
    }
    else {
        ImageOutput* n = nodes.find(_nID)->second;
        int i = 0;
        
        if (n->getIsAudio()) {
            while (i < inputGenerators.size()) {
                if (inputGenerators[i]->getParamInputType() == FFT && ((AudioListenerInput*)inputGenerators[i])->getNodeID() == _nID) {
                    inputGenerators.erase(inputGenerators.begin() + i);
                    leftAudioPatch == n ? leftAudioPatch = NULL : rightAudioPatch = NULL;
                }
                i++;
            }
            i = 0;
            while (i < audioListeners.size() && audioListeners[i]->getParamInputType() != MIDI) {
                if (audioListeners[i]->getParamInputType() == FFT && ((AudioListenerInput*)audioListeners[i])->getNodeID() == _nID) {
                    audioListeners.erase(audioListeners.begin() + i);
                }
                i++;
            }
        }
        else {
            for(int i = 0; i < inputGenerators.size(); i++) {
                inputGenerators[i]->removeNodeFromParams(_nID);
            }
        }
        
        bool found = false;
        i = 0;
        
        if (n->getNodeType() == INPUT) {
            while (i < inputs.size() && !found) {
                if (inputs[i]->getId() == _nID) {
                    inputs.erase(inputs.begin() + i);
                    found = true;
                }
                i++;
            }
        }
        else if (n->getNodeType() == VISUAL_LAYER) {
            while (i < visualLayers.size() && !found) {
                if (visualLayers[i]->getId() == _nID) {
                    visualLayers.erase(visualLayers.begin() + i);
                    found = true;
                }
                i++;
            }
        }
        else if (n->getNodeType() == MIXER) {
            while (i < mixtables.size() && !found) {
                if (mixtables[i]->getId() == _nID) {
                    mixtables.erase(mixtables.begin() + i);
                    found = true;
                }
                i++;
            }
        }
        
        nodes.erase(_nID);
        nodeViewers[currentViewer]->closePatch(_nID);
    }
}

//------------------------------------------------------------------
//void ofApp::editAudioInChannel(AudioInEvent &e_){
//
//    for (int i = 0; i < audioListeners.size(); ++i) {
//        if (audioListeners[i]->getNodeID() == e_.nodeId) {
//            vector<DTAudioMap*>* am = audioListeners[i]->getAudioMap();
//            for (int j = 0; j < am->size(); j++){
//                am->at(j)->band = e_.channel;
//            }
//            
//            break;
//        }
//    }
//}

//------------------------------------------------------------------
void ofApp::editLeftAudioIn(bool &edit_){

    editAudioIn();
    editLeftAudioInActive = !editLeftAudioInActive;
    nodeViewers[currentViewer]->setEditLeftAudioInActive(editLeftAudioInActive);
}

//------------------------------------------------------------------
void ofApp::editRightAudioIn(bool &edit_){
    
    editAudioIn();
    editRightAudioInActive = !editRightAudioInActive;
    nodeViewers[currentViewer]->setEditRightAudioInActive(editRightAudioInActive);
}

//------------------------------------------------------------------
void ofApp::editAudioIn(){
    
    if (midiLearnActive) {
        ((ofxUIMultiImageToggle*)menu->getWidget("Midi Learn"))->setValue(false);
        
        midiLearnActive = false;
        
        int i = 0;
        while (i < inputGenerators.size() && inputGenerators[i]->getParamInputType() != MIDI) {
            i++;
        }
        if (i < inputGenerators.size()) {
            ((MidiInputGenerator*)inputGenerators[i])->setMidiLearnActive(false);
        }
    }
    
    if (editLeftAudioInActive || editRightAudioInActive){
        
        for (int i=0; i<inputGenerators.size(); ++i) {
            ParamInputGenerator* p = inputGenerators[i];
            if (p->getParamInputType() == FFT) {
                
                std::map<int, ImageOutput*>::iterator audioInNode_;
                audioInNode_ = nodes.find(((AudioInputGenerator*)p)->getNodeID());
                
                if (audioInNode_ != nodes.end() &&
                    ((((AudioIn*)audioInNode_->second)->getAudioInType() == LEFT && editLeftAudioInActive) ||
                    (((AudioIn*)audioInNode_->second)->getAudioInType() == RIGHT && editRightAudioInActive))) {
                
                    ((AudioInputGenerator*)p)->clearAudioMap();
                    
                    map<int, vector<string> > attributesSelected = nodeViewers[currentViewer]->getAttributesSelectedForAudioIn();
                    std::map<int, ImageOutput*>::iterator node_;
                    
                    for(map<int, vector<string> >::iterator it = attributesSelected.begin(); it != attributesSelected.end(); it++ ){
                        node_ = nodes.find(it->first);
                        if (node_ != nodes.end()) {
                            if (editRightAudioInActive)
                                ((AudioInputGenerator*)p)->addNewAudioMap(((AudioIn*)audioInNode_->second)->getChannel(), node_->second, it->second);
                            if(editLeftAudioInActive)
                                ((AudioInputGenerator*)p)->addNewAudioMap(((AudioIn*)audioInNode_->second)->getChannel(), node_->second, it->second);
                        }
                    }
                }
            }
        }
    }
};


//------------------------------------------------------------------
bool ofApp::loadFromXML(){
    
    bool result = true;
    string message = "";
    nodeLinkType nodeLinkType;
    int nodesCount;
    
    if( XML.loadFile("appSettings.xml") ){
        
        int numMainSettingsTag = XML.getNumTags("MAIN_SETTINGS");
        
        if(numMainSettingsTag==1){
            XML.pushTag("MAIN_SETTINGS");
            
            int linkType = XML.getValue("link_type", 0);
            switch (linkType) {
                case 0:
                    nodeLinkType = STRAIGHT_LINKS;
                    ((ofxUIImageToggle*)menu->getWidget("Segmented Links"))->setValue(false);
                    ((ofxUIImageToggle*)menu->getWidget("Curved Links"))->setValue(false);
                    ((ofxUIImageToggle*)menu->getWidget("Straight Links"))->setValue(true);
                    break;
                case 1:
                    nodeLinkType = CURVE_LINKS;
                    ((ofxUIImageToggle*)menu->getWidget("Segmented Links"))->setValue(false);
                    ((ofxUIImageToggle*)menu->getWidget("Curved Links"))->setValue(true);
                    ((ofxUIImageToggle*)menu->getWidget("Straight Links"))->setValue(false);
                    break;
                case 2:
                    nodeLinkType = PATH_LINKS;
                    ((ofxUIImageToggle*)menu->getWidget("Segmented Links"))->setValue(true);
                    ((ofxUIImageToggle*)menu->getWidget("Curved Links"))->setValue(false);
                    ((ofxUIImageToggle*)menu->getWidget("Straight Links"))->setValue(false);
                    break;
            }
            
            nodesCount  = XML.getValue("total_nodes", -1);
            showConsole = ofToBool(XML.getValue("show_console", "0"));
            
            int numSettingsTag = XML.getNumTags("SETTINGS");
            
            if(numSettingsTag==1){
                
                XML.pushTag("SETTINGS");

                result = this->loadNodes(XML);
                
                XML.popTag();
            }
            else {
                result = false;
                message = "missing SETTINGS tag!";
            }
            
            if(result){
                //PROCESSING NODE_VIEWS
                
                int numNodeViews = XML.getNumTags("NODE_VIEWS");
                
                if(numNodeViews==1){
                    
                    XML.pushTag("NODE_VIEWS");
                    
                    int numNodeView = XML.getNumTags("NODE_VIEW");
                    
                    for(int i = 0; i < numNodeView; i++){
                        string nodeViewName = XML.getAttribute("NODE_VIEW","name","default",i);
                        
                        NodeViewer* nV = new NodeViewer(nodeViewName);
                        nV->setNodesCount(nodesCount);
                        
                        XML.pushTag("NODE_VIEW",i);
                        int numNODETag = XML.getNumTags("NODE");
                        
                        for(int j = 0; j < numNODETag; j++){
                            int    nodeId   = XML.getAttribute("NODE","id",0,j);
                            string nodeName = XML.getAttribute("NODE","name","default",j);
                            
                            int x            = ofToInt(XML.getAttribute("NODE","x","20",j));
                            int y            = ofToInt(XML.getAttribute("NODE","y","20",j));
                            int guiX         = ofToInt(XML.getAttribute("NODE","guiX","20",j));
                            int guiY         = ofToInt(XML.getAttribute("NODE","guiY","20",j));
                            int guiWidth     = ofToInt(XML.getAttribute("NODE","guiWidth","120",j));
                            float imageScale = ofToFloat(XML.getAttribute("NODE","imageScale","1",j));
                            
                            std::map<int,ImageOutput*>::iterator it;
                            
                            it = nodes.find(nodeId);
                            
                            if(it != nodes.end()){
                                ImageOutput* iO = it->second;
                                NodeElement* nE = new NodeElement(iO, x, y, guiX, guiY, guiWidth, imageScale);
                                
                                nV->addElement(nE);
                            }
                            else{
                                result = false;
                                message = "node not found!";
                            }
                            
                        }
                        
                        nodeViewers.push_back(nV);
                        
                        //NODE_VIEW
                        XML.popTag();
                        
                    }
                    
                    //NODE_VIEWS POP
                    XML.popTag();
                    
                }
                else{
                    result = false;
                    message = "missing NODE_VIEWS tag!";
                }
                
                // PROCESSING PARAM INPUT GENERATORS
                
                if (result){
                    
                    int numParamInputs = XML.getNumTags("PARAM_INPUT_GENERATORS");
                    
                    
                    if(numParamInputs==1){
                        
                        XML.pushTag("PARAM_INPUT_GENERATORS");

                        int numInputGen = XML.getNumTags("INPUT_GEN");
                        
                        for(int j = 0; j < numInputGen; j++){
                            string inputName = XML.getAttribute("INPUT_GEN","name","default",j);
                            string inputType = XML.getAttribute("INPUT_GEN","type","MIDI",j);
                            string midiDeviceName = XML.getAttribute("INPUT_GEN","midiDeviceName","Oxygen 25",j);
                            int    nodeID = XML.getAttribute("INPUT_GEN","nodeId",-1,j);
                            XML.pushTag("INPUT_GEN",j);
                            switch(inputGenTypes[inputType]){
                                case MIDI:
                                {
                                    MidiInputGenerator* mI = new MidiInputGenerator(inputName, midiDeviceName);
                                    mI->loadSettings(XML, nodes);
                                    
                                    inputGenerators.push_back(mI);
                                    
                                    break;
                                }
                                case FFT:
                                {
                                    std::map<int,ImageOutput*>::iterator it = nodes.find(nodeID);
                                    
                                    if(it!=nodes.end()){
                                        AudioInputGenerator* aI = new AudioInputGenerator(inputName, nodeID);
                                        aI->loadSettings(XML);
                                        
                                        inputGenerators.push_back(aI);
                                        audioListeners.push_back(aI);
                                    }
                                    break;
                                }
                                case OSC:
                                {
                                    OscInputGenerator* oI = new OscInputGenerator(inputName);
                                    oI->loadSettings(XML);
                                    
                                    inputGenerators.push_back(oI);
                                    
                                    break;
                                }
                                default:
                                {
                                    result = false;
                                    message = "unknown mixer type!";
                                    break;
                                };
                                    
                            }
                            XML.popTag();
                            
                        }
                        
                        //Popping PARAM_INPUT_GENERATORS tags
                        XML.popTag();

                    }
                    else{
                        result = false;
                        message = "missing PARAM_INPUT_GENERATORS tag!";
                    }
                    
                }
                
                //PROCESSING SYPHON SERVERS
                if (result){
                    int numServers = XML.getNumTags("SYPHON_SERVERS");
                    
                    
                    if(numServers==1){
                        XML.pushTag("SYPHON_SERVERS");
                        
                        int numSyphonServer = XML.getNumTags("SERVER");
                        
                        for(int j=0; j<numSyphonServer; j++){
                            int inputId         = XML.getAttribute("SERVER","inputId",0,j);
                            string inputName    = XML.getAttribute("SERVER","inputName","mainMix",j);
                            string exportName   = XML.getAttribute("SERVER","exportName","syphon1",j);
                            
                            std::map<int,ImageOutput*>::iterator it;
                            
                            it=nodes.find(inputId);
                            
                            if(it!=nodes.end()){
                                ImageOutput* iO = it->second;
                                CustomSyphonServer* cSS = new CustomSyphonServer(exportName,iO);
                                
                                syphonServers.push_back(cSS);
                                
                            }
                            else{
                                result = false;
                                message = "node not found!";
                                break;
                            }
                            
                        }
                        
                        //Popping SYPHON_SERVERS tags
                        XML.popTag();
                        
                    }
                    else{
                        result = false;
                        message = "missing SYPHON_SERVERS tag!";
                    }
                }
            }
        }
        else{
            result = false;
            message = "missing MAIN_SETTINGS tag!";
        }
        
    }else{
        
        //file not loaded
        message = "file not loaded!";
        result = false;
        
    }
    
    if(!result){
        ofLogNotice() << message;
    }
    else{
        //populating the nodes vector for fast iteration in the main loop.
        for(int i=0; i<inputs.size();i++){
            nodesVector.push_back(inputs[i]);
        }
        for(int i=0; i<visualLayers.size();i++){
            nodesVector.push_back(visualLayers[i]);
        }
        for(int i=0; i<mixtables.size();i++){
            nodesVector.push_back(mixtables[i]);
        }
        
        //setting inputs to every node
        bool error = false;
        for(int i=0; i<nodesVector.size(); i++){
            error = nodesVector[i]->findAndAssignInputs(nodes);
            if(error){
                message = "node not found";
                result = false;
                break;
            }
        }
        
        //create connections in nodeView
        for (int i=0; i<nodeViewers.size(); ++i) {
            nodeViewers[i]->createConnections();
            nodeViewers[i]->setLinkType(nodeLinkType);
        }
        
        if (audioAnalizer == NULL) {
            audioAnalizer = new AudioAnalizer();
            audioAnalizer->setDrawAudioAnalizer(false);
            ((ofxUIMultiImageToggle*)right_menu->getWidget("Analizer"))->setValue(false);
        }
        initNode(audioAnalizer);
        ((ofxUIMultiImageToggle*)right_menu->getWidget("Analizer"))->setValue(audioAnalizer->getDrawAudioAnalizer());
        nodeViewers[currentViewer]->addPatch(audioAnalizer, ofPoint(audioAnalizer->getLowestXCoord(),audioAnalizer->getHighestYCoord()));
    }
    
    return result;
    
}


//------------------------------------------------------------------
bool ofApp::saveToXML() {
    
    ofxXmlSettings XML;
    bool file = true;
    
    // Open/create clean xml settings file
    //
    file = XML.loadFile("appSettings.xml");
    if (file) {
        XML.clear();
    } else {
        file = XML.saveFile("appSettings.xml");
        XML.loadFile("appSettings.xml");
    }
    
    if (file){
        
        XML.clear();
        
        XML.addTag("MAIN_SETTINGS");
        XML.pushTag("MAIN_SETTINGS");
        
        XML.setValue("link_type", nodeViewers[currentViewer]->getLinkType() );
        XML.setValue("total_nodes", nodeViewers[currentViewer]->getNodesCount() );
        XML.setValue("show_console", showConsole );
        
        XML.addTag("SETTINGS");
        XML.pushTag("SETTINGS");
        
        // Save Inputs
        //
        XML.addTag("INPUTS");
        XML.pushTag("INPUTS");
        audioAnalizer->saveSettings(XML);
        for (int i = 0; i < inputs.size(); i++) {
            inputs[i]->saveSettings(XML);
        }
        XML.popTag();
        
        
        // Save Visual Layers
        //
        XML.addTag("VISUAL_LAYERS");
        XML.pushTag("VISUAL_LAYERS");
        for (int vl = 0; vl < visualLayers.size(); vl++) {
            visualLayers[vl]->saveSettings(XML);
        }
        XML.popTag();
        
        
        // Save Mixers
        //
        XML.addTag("MIXERS");
        XML.pushTag("MIXERS");
        for (int m = 0; m < mixtables.size(); m++) {
            mixtables[m]->saveSettings(XML);
        }
        XML.popTag();
        
        
        // Save Encapsulated Nodes
        //
        XML.addTag("ENCAPSULATED_NODES");
        XML.pushTag("ENCAPSULATED_NODES");
        for(int e = 0; e < encapsulatedIds.size(); e++){
            nodeViewers[currentViewer]->saveEncapsulatedSettings(XML, encapsulatedIds[e]);
        }
        XML.popTag();
        
        
        XML.popTag(); // tag SETTINGS
        
        
        // Save Node Views
        //
        XML.addTag("NODE_VIEWS");
        XML.pushTag("NODE_VIEWS");
        for (int nv = 0; nv < nodeViewers.size(); nv++) {
            nodeViewers[nv]->saveSettings(XML);
        }
        XML.popTag(); // tag NODE_VIEWS
        
        
        // Save Param Input Generators
        //
        XML.addTag("PARAM_INPUT_GENERATORS");
        XML.pushTag("PARAM_INPUT_GENERATORS");
        for (int ig = 0; ig < inputGenerators.size(); ig++) {
            inputGenerators[ig]->saveSettings(XML);
        }
        XML.popTag(); // tag PARAM_INPUT_GENERATORS
        
        
        // Save Syphon Servers
        //
        XML.addTag("SYPHON_SERVERS");
        XML.pushTag("SYPHON_SERVERS");
        for (int ss = 0; ss < syphonServers.size(); ss++) {
            syphonServers[ss]->saveSettings(XML);
        }
        XML.popTag(); // tag SYPHON_SERVERS
        
        
        XML.popTag(); // tag MAIN_SETTINGS
        
        if (XML.saveFile()) {
            ofLog(OF_LOG_NOTICE, "Settings saved succesfully");
            console->pushSuccess("Settings saved succesfully");
        }
        else {
            ofLog(OF_LOG_ERROR, "Couldn't save settings. An error occurred");
            console->pushError("Couldn't save settings. An error occurred");
        }
    }
    else {
        ofLog(OF_LOG_ERROR, "Couldn't load the .xml file. The file appSettings.xml was not found");
        console->pushError("Couldn't load the .xml file. The file appSettings.xml was not found");
    }
}

//------------------------------------------------------------------
bool ofApp::loadNodes(ofxXmlSettings &XML){
    
    bool result = true;
    int  numInputTag = XML.getNumTags("INPUTS");
    
    // LOADING INPUTS
    
    if(numInputTag==1){
        XML.pushTag("INPUTS");
        
        int numInputTag = XML.getNumTags("NODE");
        for(int i=0; i < numInputTag; i++){
            int    inputId   = XML.getAttribute("NODE","id",-1,i);
            string inputName = XML.getAttribute("NODE","name","default",i);
            string inputType = XML.getAttribute("NODE","type","CAM",i);
            
            switch(inputTypes[inputType]){
                case AUDIO_ANALIZER:
                {
                    audioAnalizer = new AudioAnalizer(inputName, inputId);
                    
                    if (!audioAnalizer->loadSettings(XML, i)) {
                        result = false;
                        console->pushMessage("No Audio Analizer!");
                    }
                    
                    break;
                };
                case VIDEO:
                {
                    VideoPlayerMac* vP = new VideoPlayerMac(inputName, inputId);
                    
                    if (vP->loadSettings(XML, i)) {
                        inputs.push_back(vP);
                        nodes.insert(std::pair<int,ImageOutput*>(inputId,vP));
                    }
                    else {
                        result = false;
                        console->pushMessage("no videos to be loaded!");
                    }
                    
                    break;
                };
                case CAM:
                {
                    InputCamera* iC = new InputCamera(inputName, inputId);
                    iC->loadSettings(XML, i);
                    
                    inputs.push_back(iC);
                    nodes.insert(std::pair<int,ImageOutput*>(inputId,iC));
                    
                    break;
                };
                case IMAGE:
                {
                    ImageInputList* iI = new ImageInputList(inputName, inputId);
                    
                    if (iI->loadSettings(XML, i)) {
                        inputs.push_back(iI);
                        nodes.insert(std::pair<int,ImageOutput*>(inputId,iI));
                    }
                    else {
                        result = false;
                        console->pushMessage("no images to be loaded!");
                    }
                    
                    break;
                };
                case PARTICLE:
                {
                    ParticleGenerator* iI = new ParticleGenerator(inputName, inputId);
                    iI->loadSettings(XML, i);
                    
                    inputs.push_back(iI);
                    nodes.insert(std::pair<int,ImageOutput*>(inputId,iI));
                    
                    break;
                };
                case RIGHT_AUDIO_IN:
                {
                    AudioIn* aI = new AudioIn(gui, right, "Audio In - Right Channel", inputName, inputId);
                    aI->loadSettings(XML, i);
                    
                    inputs.push_back(aI);
                    nodes.insert(std::pair<int,ImageOutput*>(inputId,aI));
                    
                    rightAudioPatch = aI;
                    ofAddListener(aI->editAudioIn , this, &ofApp::editRightAudioIn);
//                    ofAddListener(aI->editAudioInChannel , this, &ofApp::editAudioInChannel);
                    
                    break;
                };
                case LEFT_AUDIO_IN:
                {
                    AudioIn* aI = new AudioIn(gui, left, "Audio In - Left Channel", inputName, inputId);
                    aI->loadSettings(XML, i);
                    
                    inputs.push_back(aI);
                    nodes.insert(std::pair<int,ImageOutput*>(inputId,aI));
                    
                    leftAudioPatch = aI;
                    ofAddListener(aI->editAudioIn , this, &ofApp::editLeftAudioIn);
//                    ofAddListener(aI->editAudioInChannel , this, &ofApp::editAudioInChannel);
                    
                    break;
                };
                default:
                {
                    result = false;
                    console->pushMessage("unknown input type!");
                    break;
                };
            }
            
            if(!result){
                //there has been an error
                //exit the loop
                break;
            }
            
        }
        XML.popTag();
        
    }
    else{
        console->pushMessage("inputs tag missing");
        result = false;
    }
    
    //LOADING VISUAL LAYERS
    
    if(result){
        int numVLsTag = XML.getNumTags("VISUAL_LAYERS");
        if(numVLsTag==1){
            
            XML.pushTag("VISUAL_LAYERS");
            
            int numVLTag = XML.getNumTags("NODE");
            
            for(int i = 0; i < numVLTag; i++){
                int    layerId    = XML.getAttribute("NODE","id",-1,i);
                string layerName  = XML.getAttribute("NODE","name","default",i);
                string layerType  = XML.getAttribute("NODE","type","IKEDA",i);
                int inputSourceId = XML.getAttribute("NODE","inputSource",0,i);
                
                switch(visualLayerTypes[layerType]){
                    case IKEDA:
                    {
                        IkedaLayer* iL = new IkedaLayer(layerName, layerId);
                        if (inputSourceId != 0)
                            iL->addInputIdentifier(inputSourceId);
                        iL->loadSettings(XML, i);
                        
                        visualLayers.push_back(iL);
                        nodes.insert(std::pair<int,ImageOutput*>(layerId,iL));
                        
                        break;
                    };
                    case GLITCH_1:
                    {
                        GlitchLayer* gL = new GlitchLayer(layerName, layerId);
                        if (inputSourceId != 0)
                            gL->addInputIdentifier(inputSourceId);
                        gL->loadSettings(XML, i);
                        
                        visualLayers.push_back(gL);
                        nodes.insert(std::pair<int,ImageOutput*>(layerId,gL));
                        
                        break;
                    };
                    case GLITCH_2:
                    {
                        GlitchLayerAlt* gLA = new GlitchLayerAlt(layerName, layerId);
                        if (inputSourceId != 0)
                            gLA->addInputIdentifier(inputSourceId);
                        gLA->loadSettings(XML, i);
                        
                        visualLayers.push_back(gLA);
                        nodes.insert(std::pair<int,ImageOutput*>(layerId,gLA));
                        
                        break;
                    };
                    case IMAGE_PROCESSOR:
                    {
                        
                        ImageProcessor* gLA = new ImageProcessor(layerName, layerId);
                        if (inputSourceId != 0)
                            gLA->addInputIdentifier(inputSourceId);
                        gLA->loadSettings(XML, i);
                        
                        
                        visualLayers.push_back(gLA);
                        nodes.insert(std::pair<int,ImageOutput*>(layerId,gLA));
                        
                        break;
                    };
                    default:
                    {
                        result = false;
                        console->pushMessage("unknown visual layer type!");
                        break;
                    };
                }
                
                if(!result){
                    //there has been an error
                    //exit the loop
                    break;
                }
                
            }
            
            XML.popTag();
        }
        else {
            console->pushMessage("visual layers tag missing");
            result = false;
        }
    }
    
    //LOADING MIXERS
    if(result){
        int numMXsTag = XML.getNumTags("MIXERS");
        if(numMXsTag==1){
            XML.pushTag("MIXERS");
            
            int numMXTag = XML.getNumTags("NODE");
            
            for(int i = 0; i < numMXTag; i++){
                int    mixerId   = XML.getAttribute("NODE","id",-1,i);
                string mixerName = XML.getAttribute("NODE","name","default",i);
                string mixerType = XML.getAttribute("NODE","type","SIMPLE_BLEND", i);
                
                switch(mixerTypes[mixerType]){
                    case SIMPLE_BLEND:
                    {
                        MixSimpleBlend* mSB = new MixSimpleBlend(mixerName, mixerId);
                        mSB->loadSettings(XML, i);
                        
                        mixtables.push_back(mSB);
                        nodes.insert(std::pair<int, ImageOutput*>(mixerId, mSB));
                        
                        break;
                    };
                    case MASK:
                    {
                        MixMask* mMM = new MixMask(mixerName, mixerId);
                        mMM->loadSettings(XML, i);
                        
                        mixtables.push_back(mMM);
                        nodes.insert(std::pair<int, ImageOutput*>(mixerId, mMM));
                        
                        break;
                    };
                    case MULTI_CHANNEL:
                    {
                        MultiChannelSwitch* mMM = new MultiChannelSwitch(mixerName, mixerId);
                        mMM->loadSettings(XML, i);
                        
                        mixtables.push_back(mMM);
                        nodes.insert(std::pair<int, ImageOutput*>(mixerId, mMM));
                        
                        break;
                    };
                    default:
                    {
                        result = false;
                        console->pushMessage("unknown mixer type!");
                        break;
                    };
                        
                }
                
                if(!result){
                    //there has been an error
                    //exit the loop
                    break;
                }
                
            }
            //MIXERS POP
            XML.popTag();
            
        }
        
    }
    else{
        console->pushMessage("mixers tag missing");
        result = false;
    }
    
    // LOAD ENCAPSULATED NODES
    if(result){
        int numENsTag = XML.getNumTags("ENCAPSULATED_NODES");
        if(numENsTag==1){
            XML.pushTag("ENCAPSULATED_NODES");
            
            int numENTag = XML.getNumTags("ENCAPSULATED_NODE");
            
            for(int i = 0; i < numENTag; i++){
                int encapsulatedId     = XML.getAttribute("ENCAPSULATED_NODE","id",-1,i);
                int lastEncapsulatedId = XML.getAttribute("ENCAPSULATED_NODE","lastEncapsulatedId",false,i);

                nodes.at(lastEncapsulatedId)->setEncapsulatedId(encapsulatedId);
                nodes.at(lastEncapsulatedId)->setLastEncapsulated(true);
                
                XML.pushTag("ENCAPSULATED_NODE", i);
                int numEncapsulatedTag = XML.getNumTags("NODE");
                for(int j = 0; j < numEncapsulatedTag; j++){
                    int patchId = XML.getValue("NODE", -1, j);
                    if(patchId == -1){
                        result = false;
                        ConsoleLog::getInstance()->pushError("Encapsulated data is wrong");
                        break;
                    }
                    nodes.at(patchId)->setEncapsulatedId(encapsulatedId);
                    nodes.at(patchId)->setWindowId(-1);
                    nodes.at(patchId)->setToEncapsulatedId(lastEncapsulatedId);
                }
                
                encapsulatedIds.push_back(encapsulatedId);
                
                // ENCAPSULATED_NODE POP
                XML.popTag();
                
                if(!result){
                    //there has been an error
                    //exit the loop
                    break;
                }
                
            }
            //ENCAPSULATED_NODES POP
            XML.popTag();
            
        } else {
            result = false;
            ConsoleLog::getInstance()->pushError("Encapsulated tag missing");
        }
        
    }
    
    return result;
}

/* ================================================ */
/* ================================================ */

// -----------------------------------------------------------
// ------------------------------------------------- SNNIPPETS
// -----------------------------------------------------------
bool ofApp::loadSnippet() {
    
    string snippetName = "";
    bool result = true;
    
    ofFileDialogResult openFileResult;
    openFileResult = ofSystemLoadDialog("Select a snippet (.xml)");
    
    if (openFileResult.bSuccess){
        ofFile file (openFileResult.getPath());
        if (file.exists()){
            string fileExtension = ofToUpper(file.getExtension());
            
            if(fileExtension == "XML"){
                snippetName = openFileResult.getPath();
            } else return false;
        }
        file.close();
    }
    ofxXmlSettings XML;
    
    nodeViewers[currentViewer]->deactivateAllPatches();
    int nodesCount = nodeViewers[currentViewer]->getNodesCount();
    
    if (XML.loadFile(snippetName)){
        
        vector<ImageOutput*> aux_nodesVector;
        map<int, ImageOutput*> aux_nodes;
        vector<NodeElement*> snnipetNodeElements;
        bool result = true;
        int  numNodes = XML.getNumTags("NODE");
    
        for(int i = 0; i < numNodes; i++){
            
            int    nodeId     = XML.getAttribute("NODE","id",0,i) + nodesCount;
            string nodeName   = XML.getAttribute("NODE","name","default",i);
            string nodeType   = XML.getAttribute("NODE","type","CAM",i);
            int inputSourceId = XML.getAttribute("NODE","inputSource",0,i) + nodesCount;
            
            if (nodeType == "VIDEO") {
                
                VideoPlayerMac* vP = new VideoPlayerMac(nodeName, nodeId);
                
                if (vP->loadSettings(XML, i, nodesCount)) {
                    inputs.push_back(vP);
                    aux_nodes.insert(std::pair<int,ImageOutput*>(nodeId,vP));
                    aux_nodesVector.push_back(vP);
                }
                else {
                    result = false;
                    console->pushMessage("no videos to be loaded!");
                }
            }
            else if (nodeType == "CAM") {
                 
                InputCamera* iC = new InputCamera(nodeName, nodeId);
                iC->loadSettings(XML, i, nodesCount);
                inputs.push_back(iC);
                aux_nodes.insert(std::pair<int,ImageOutput*>(nodeId,iC));
                aux_nodesVector.push_back(iC);
            }
            else if (nodeType == "IMAGE") {
                
                ImageInputList* iI = new ImageInputList(nodeName, nodeId);
                
                if (iI->loadSettings(XML, i, nodesCount)) {
                    inputs.push_back(iI);
                    aux_nodes.insert(std::pair<int,ImageOutput*>(nodeId,iI));
                    aux_nodesVector.push_back(iI);
                }
                else {
                    result = false;
                    console->pushMessage("no images to be loaded!");
                }
            }
            else if (nodeType == "PARTICLE") {
                
                ParticleGenerator* iI = new ParticleGenerator(nodeName, nodeId);
                iI->loadSettings(XML, i, nodesCount);
                inputs.push_back(iI);
                aux_nodes.insert(std::pair<int,ImageOutput*>(nodeId,iI));
                aux_nodesVector.push_back(iI);
            }
            else if (nodeType == "IKEDA") {
                
                IkedaLayer* iL = new IkedaLayer(nodeName, nodeId);
                if (inputSourceId != nodesCount)
                    iL->addInputIdentifier(inputSourceId);
                iL->loadSettings(XML, i, nodesCount);
                visualLayers.push_back(iL);
                aux_nodes.insert(std::pair<int,ImageOutput*>(nodeId,iL));
                aux_nodesVector.push_back(iL);
            }
            else if (nodeType == "GLITCH_1") {
                
                GlitchLayer* gL = new GlitchLayer(nodeName, nodeId);
                if (inputSourceId != nodesCount)
                    gL->addInputIdentifier(inputSourceId);
                gL->loadSettings(XML, i, nodesCount);
                visualLayers.push_back(gL);
                aux_nodes.insert(std::pair<int,ImageOutput*>(nodeId,gL));
                aux_nodesVector.push_back(gL);
            }
            else if (nodeType == "GLITCH_2") {
                
                GlitchLayerAlt* gLA = new GlitchLayerAlt(nodeName, nodeId);
                if (inputSourceId != nodesCount)
                    gLA->addInputIdentifier(inputSourceId);
                gLA->loadSettings(XML, i, nodesCount);
                visualLayers.push_back(gLA);
                aux_nodes.insert(std::pair<int,ImageOutput*>(nodeId,gLA));
                aux_nodesVector.push_back(gLA);
            }
            else if (nodeType == "IMAGE_PROCESSOR") {
                
                ImageProcessor* gLA = new ImageProcessor(nodeName, nodeId);
                if (inputSourceId != nodesCount)
                    gLA->addInputIdentifier(inputSourceId);
                gLA->loadSettings(XML, i, nodesCount);
                visualLayers.push_back(gLA);
                aux_nodes.insert(std::pair<int,ImageOutput*>(nodeId,gLA));
                aux_nodesVector.push_back(gLA);
            }
            else if (nodeType == "SIMPLE_BLEND") {
                
                MixSimpleBlend* mSB = new MixSimpleBlend(nodeName, nodeId);
                mSB->loadSettings(XML, i, nodesCount);
                mixtables.push_back(mSB);
                aux_nodes.insert(std::pair<int, ImageOutput*>(nodeId, mSB));
                aux_nodesVector.push_back(mSB);
            }
            else if (nodeType == "MASK") {
                
                MixMask* mMM = new MixMask(nodeName, nodeId);
                mMM->loadSettings(XML, i, nodesCount);
                mixtables.push_back(mMM);
                aux_nodes.insert(std::pair<int, ImageOutput*>(nodeId, mMM));
                aux_nodesVector.push_back(mMM);
            }
            else if (nodeType == "MULTI_CHANNEL") {
                
                MultiChannelSwitch* mMM = new MultiChannelSwitch(nodeName, nodeId);
                mMM->loadSettings(XML, i, nodesCount);
                mixtables.push_back(mMM);
                aux_nodes.insert(std::pair<int, ImageOutput*>(nodeId, mMM));
                aux_nodesVector.push_back(mMM);
            }
            else {
                result = false;
                console->pushMessage("unknown input type!");
            }
        }
    
        XML.popTag(); // pop "NODE"
    
        if (result) {
            //setting inputs to every node
            for(int i = 0; i < aux_nodesVector.size(); i++){
                if (aux_nodesVector[i]->findAndAssignInputs(aux_nodes)) {
                    console->pushMessage("node not found");
                    result = false;
                    break;
                }
                NodeElement* nE = new NodeElement(aux_nodesVector[i]);
                snnipetNodeElements.push_back(nE);
                nodeViewers[currentViewer]->addElement(nE);
                nodesVector.push_back(aux_nodesVector[i]);
            }
        }
        
        if (result) {
            //create connections in nodeView
            nodeViewers[currentViewer]->createConnections(snnipetNodeElements);
            nodeViewers[currentViewer]->setNodesCount(nodesCount + aux_nodes.size());
            
            for(int i = 0; i < aux_nodesVector.size(); i++){
                initNode(aux_nodesVector[i]);
            }
        }
    }
    
    return result;
}

//------------------------------------------------------------------
bool ofApp::saveSnippet() {
    
    string snippetName = "";
    ofxXmlSettings XML;
    
    ofFileDialogResult openFileResult;
    openFileResult = ofSystemSaveDialog("snippet.xml", "Save your Snippet");
    
    if(openFileResult.bSuccess){
        snippetName = openFileResult.getPath();
    }
    
    bool saveOk = true;
    bool a;
    bool b;
    // Delete and create xml file
    if (XML.loadFile(snippetName)) {
        XML.clear();
    } else {
        b = XML.saveFile(snippetName);
        XML.loadFile(snippetName);
    }
    
    map<int,ofxPatch*> activePatches = nodeViewers[currentViewer]->getActivePatches();
    map<int,int> newIdsMap;
    newIdsMap.clear();
    int nextNodeId = 1;
    
    for(map<int,ofxPatch*>::iterator it = activePatches.begin(); it != activePatches.end(); it++ ){
        newIdsMap[it->second->getId()] = nextNodeId;
        nextNodeId++;
    }
    
    for(map<int,ofxPatch*>::iterator it = activePatches.begin(); it != activePatches.end(); it++ ){
        saveOk = ((ImageOutput*)it->second)->saveSettingsToSnippet(XML, newIdsMap);
    }
    
    XML.saveFile(snippetName);
    
    return saveOk;
}

/* ================================================ */
/* ================================================ */
