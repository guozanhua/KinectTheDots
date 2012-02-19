#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxTween.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void loadData();
    ofPoint getNextPoint();
    ofPoint getCurrentPoint();
    void animateView();
    void transformView();
    ofPoint getViewTarget();
    
    void drawDots();
    void drawDrawing();
    
    float currentScale();
    
    ofPoint previousViewTarget;
    
    int totalPointCount;
    int totalPointsHit;
    
    ofxTween tween;
    ofxEasingCubic easingcubic;
    
    ofImage ink;
    
    ofPoint convertToDrawingPoint(ofPoint p);
    
    vector<ofPolyline> lines;
    
    vector<ofPolyline> drawing;
    
    bool showNums;
    bool showLines;
    bool showAllPoints;
    bool showDebug;
    
    int canvasScale, prevCanvasScale;
    
    ofFbo canvas, drawingCanvas, preview;
    
    ofPoint currentPoint;
    ofPoint nextPoint;
    
    int currentPointNum;
    int currentLineNum;
    
    int nextPointNum;
    int nextLineNum;
    
        
};
