/*
 *  VideoPlayerMac.h
 *  ofApp
 *
 *  Created by Brian Eschrich on 03/12/14
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _VideoPlayerMac
#define _VideoPlayerMac

#include "ofMain.h"
#include "InputSource.h"

class VideoPlayerMac : public InputSource{
	
  public:
	
	VideoPlayerMac(string name = "video player");
	
	void setup();
	void update();
	void draw(int x,int y, float scale);
    void loadVideo(string path);
    void inputUpdated(ofImage & img){};
    void drawGui(int x,int y, int width = 240){};
    void nextVideo();
    void prevVideo();
    void playVideo();
    void stopVideo();
    void setSpeed(float speed);
    float getSpeed();
    
private:
    vector<ofQTKitPlayer*> players;
    int currentPlayer;
    ofImage img;
};

#endif
