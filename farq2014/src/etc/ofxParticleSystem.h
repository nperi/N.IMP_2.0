/*
 *  ofxParticleSystem.h
 *  ofxParticleSystem
 *
 *  Created by nathan_laptop on 7/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include "ofxParticle.h"
#include "ofMain.h"

class ofxParticleSystem {
public:
	// constructors
	ofxParticleSystem(int initialAmount,int width_ = 1024, int heigth_ = 768);
	
	// methods
	void update();
	void addForce(ofVec2f force);
	void draw();
	
	void addParticles(int amount);
	void removeParticles(int amount);
    
    void resetForce();
    void addForce(float x, float y);
    void addRepulsionForce(float x, float y, float radius, float scale);
    void addAttractionForce(float x, float y, float radius, float scale);
	
	int numParticles();

private:
	// properties
	vector<ofxParticle> particles;
    bool	bRepel;
	float	radius;
	float	strength;
    ofFbo   fbo;
    
    int width,height;
};