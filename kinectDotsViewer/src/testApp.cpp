#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    useKinect = false;
    
    totalPointsHit = 0;
    totalPointCount = 0;
    
    loadData();
    showNums = true;
    showAllPoints = true;
    showLines = true;
    
    pointBaseNeedsRefresh = false;
    
    currentPointNum = 0;
    currentLineNum = 0;
    
    nextPointNum = 1;
    nextLineNum = 0;
    
    ink.loadImage("ink2.png");
    perlin.loadImage("perlin1.png");
    
    
    currentPoint =   getCurrentPoint();
    nextPoint = getNextPoint();
    
    canvasScale = 3 ;
    
    canvas.allocate(ofGetWidth(), ofGetHeight());
    drawingCanvas.allocate(ofGetWidth(), ofGetHeight());
    preview.allocate(ofGetWidth(), ofGetHeight());
    
    drawing.push_back(ofPolyline());
    
    drawingCanvas.begin();
    ofClear(255, 0);
    drawingCanvas.end();
    

    if(useKinect){
    
        float filterFactor = 0.1f;
    
        recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
        //recordContext.setupUsingXMLFile();
        recordDepth.setup(&recordContext);
        recordImage.setup(&recordContext);
    
        recordUser.setup(&recordContext);
        recordUser.setSmoothing(filterFactor);				// built in openni skeleton smoothing...
        recordUser.setUseMaskPixels(true);
        recordUser.setUseCloudPoints(false);
        recordUser.setMaxNumberOfUsers(2);					// use this to set dynamic max number of users (NB: that a hard upper limit is defined by MAX_NUMBER_USERS in ofxUserGenerator)
    
        recordHandTracker.setup(&recordContext, 4);
        recordHandTracker.setSmoothing(filterFactor);		// built in openni hand track smoothing...
        recordHandTracker.setFilterFactors(filterFactor);	// custom smoothing/filtering (can also set per hand with setFilterFactor)...set them all to 0.1f to begin with
    
        recordContext.toggleRegisterViewport();
        recordContext.toggleMirror();
    }
}


//--------------------------------------------------------------
void testApp::update(){
    
    if(useKinect){
        recordContext.update();
        recordDepth.update();
    }
    
    if(!tween.isRunning()){
        ofPoint pointInDrawingSpace;
        
        if(useKinect){
            if ( recordHandTracker.getNumTrackedHands() >0){  
                ofxTrackedHand * hand = recordHandTracker.getHand(0);
                if(hand->isBeingTracked){
                    cout << "x: " << hand->projectPos.x << " y: " << hand->projectPos.y;
                    //l-r 70,590
                    //b-t 80,400
                    ofPoint screenHand;
                    screenHand.x = ofMap(hand->projectPos.x, 80, 590,  0, ofGetWidth());
                    screenHand.y = ofMap(hand->projectPos.y, 70, 400, 0, ofGetHeight());
                    cout << " Tx: " << screenHand.x << " Ty: " << screenHand.y << endl;
                    
                    pointInDrawingSpace = convertToDrawingPoint(screenHand);
                    
                }
            }
            
        } else {
            pointInDrawingSpace = convertToDrawingPoint(ofPoint(mouseX,mouseY));
        }
        
        if(pointBaseNeedsRefresh){
            pointBase = pointInDrawingSpace;
            pointBaseNeedsRefresh = false;
        }
        
        pointInDrawingSpace.x = currentPoint.x + (pointInDrawingSpace.x - pointBase.x);
        pointInDrawingSpace.y = currentPoint.y + (pointInDrawingSpace.y - pointBase.y);

        
        if(ofDist(pointInDrawingSpace.x, pointInDrawingSpace.y, nextPoint.x, nextPoint.y) <= 0.5 ){
            keyPressed(' ');
        } else {
            
            drawing.at(0).addVertex(pointInDrawingSpace);
        }
    } else {
        pointBaseNeedsRefresh = true;
    }
    
}

void testApp::transformView(){    
    ofTranslate(-1 * (tween.update() * tween.getTarget(2)) + ofGetWidth()/2, -1 * (tween.getTarget(1) * tween.getTarget(2)) + ofGetHeight()/2);
    ofScale(tween.getTarget(2), tween.getTarget(2));
    
}

float testApp::currentScale(){
    float d = ofDist(currentPoint.x, currentPoint.y, nextPoint.x, nextPoint.y);
    
    float dw = abs(currentPoint.x - nextPoint.x) + 10;
    float dh = abs(currentPoint.y - nextPoint.y) + 10;

    float m;
    
    if(dw > dh){
        m = ofGetWidth()/dw;
        cout << "wider. scale: " << m << endl;

    } else {
        m = ofGetHeight()/dh;
        cout << "taller. scale: " << m << endl;

    }
    
   // float m = ofMap(d, 6, 38, 65, 5 ,true);
    

    
    return m;
}

void testApp::animateView(){
    totalPointsHit++;
    unsigned duration= 1000;
    unsigned delay = 0;
    
    ofPoint nextViewTarget = getViewTarget();
    
    
    tween.setParameters(easingcubic,ofxTween::easeInOut,previousViewTarget.x,nextViewTarget.x,duration,delay); // viewTarget.x
    tween.addValue(previousViewTarget.y, nextViewTarget.y); // viewTarget.y
    tween.addValue(prevCanvasScale, currentScale()); // canvasScale
    tween.start();
}

void testApp::drawDots(){
    ofSetColor(0);
    for(int i = 0; i < lines.size(); i++){
        if(showLines){
            lines.at(i).draw();
        }
        vector<ofPoint> points = lines.at(i).getVertices();
        if(showAllPoints){  
            for(int i = 0; i < points.size(); i++){
                ofPoint p = points.at(i);
                ofCircle(p.x, p.y, 1);
                if(showNums){
                    ofDrawBitmapString(ofToString(i), p);
                } 
            }
        } else {
            ofCircle(nextPoint.x, nextPoint.y, 1);
        }
    }
    
    ofPushStyle();
    ofSetColor(255,0,0);
    ofCircle(currentPoint.x, currentPoint.y, 1);
    ofPopStyle();
}

void testApp::drawDrawing(){
    ofSetColor(0, 0, 255);
    ofSetLineWidth(2);
    
    
    for(int i = 0; i < drawing.size(); i++){
        drawing.at(i).draw();
        
    }
}


//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(255);
    
    canvas.begin();
    ofClear(255,0);
    transformView();
    drawDots();
    canvas.end();
    
    drawingCanvas.begin();
    ofClear(255,0);
    transformView();
    drawDrawing();
    drawingCanvas.end();
    
    if(showDebug){
        preview.begin();
        ofClear(255,0);
        bool currentShowAllPoints = showAllPoints;
        showAllPoints = true;
        drawDots();
        showAllPoints = currentShowAllPoints;
        drawDrawing();
        preview.end();
    }
    
    if(lines.size() > 0){
                
        ofSetColor(255,255,255,240);
        
        ofEnableAlphaBlending();
        
        canvas.draw(0,0);
        drawingCanvas.draw(0,0);
        
        if(showDebug){
            
            ofSetColor(0, 0, 0);
            stringstream output;
            output << totalPointsHit << "/" << totalPointCount;
            ofDrawBitmapString(output.str(), ofPoint(10,20));
            
            ofPushMatrix();
            ofSetColor(255,255,255,255);
            ofTranslate(0, ofGetHeight() - preview.getHeight()*0.3);
            ofScale(0.3, 0.3);
            preview.draw(0,0);
            ofPopMatrix();
            
            ofPushMatrix();
            ofSetColor(255,255,255,255);
            ofTranslate(ofGetWidth() - 640*0.3, ofGetHeight() - 480*0.3);
            ofScale(0.3, 0.3);
            recordDepth.draw(0,0);
            ofPopMatrix();
        }
    }   
}


ofPoint testApp::getViewTarget(){
    
    ofPoint result;
    
    result.x = (currentPoint.x + nextPoint.x)/2;
    result.y = (currentPoint.y + nextPoint.y)/2;
    
    return result;
}

ofPoint testApp::getNextPoint(){
    if(nextLineNum >= lines.size()){
        nextLineNum = 0;
    }
    
    ofPolyline line = lines.at(nextLineNum);
    ofPoint result = line[nextPointNum];
    
    nextPointNum++;
    
    if(nextPointNum >= lines.at(nextLineNum).getVertices().size()){
        nextPointNum = 0;
        nextLineNum++;
        
    }
    
    return result;
    
}

ofPoint testApp::getCurrentPoint(){
    if(currentLineNum >= lines.size()){
        currentLineNum = 0;
    }
    
    ofPolyline line = lines.at(currentLineNum);
    ofPoint result = line[currentPointNum];
    
    currentPointNum++;
    
    if(currentPointNum >= lines.at(currentLineNum).getVertices().size()){
        currentPointNum = 0;
        currentLineNum++;
        
    }
    
    return result;
}

void testApp::loadData(){
    ofFileDialogResult result = ofSystemLoadDialog("Please select a file");    
    
    ofxXmlSettings xml;
    xml.loadFile(result.filePath);
    
    xml.pushTag("Lines");
    
    int numLines = xml.getNumTags("Line");
    for(int i = 0; i < numLines; i++){
        xml.pushTag("Line", i);
        
        ofPolyline line;
        
        int numPoints = xml.getNumTags("Point");
        for(int j = 0; j < numPoints; j++){        
            line.addVertex(ofPoint(xml.getAttribute("Point", "x", 0, j), xml.getAttribute("Point", "y", 0, j)));
            totalPointCount++;
        }
        
        
        lines.push_back(line);
        
        xml.popTag();
    }
}

ofPoint testApp::convertToDrawingPoint(ofPoint p){
    ofPoint result = ofPoint(p.x, p.y);
    
    result.x = (p.x + getViewTarget().x * currentScale() - ofGetWidth()/2) / currentScale();
    result.y = (p.y + getViewTarget().y * currentScale() - ofGetHeight()/2) / currentScale();
    
    return result;
    
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'n'){
        showNums = !showNums;
    }
    
    if(key == 'l'){
        showLines = !showLines;
    }
    
    if(key == ' '){
        prevCanvasScale = currentScale();
        previousViewTarget = getViewTarget(); // store it before it changes for tweening
        
        currentPoint = getCurrentPoint();
        nextPoint = getNextPoint();
        animateView();
    }
    
    if(key == '='){
        canvasScale += 1;
        // cout << canvasScale << endl;
    }
    
    if(key == '-'){
        canvasScale -= 1;
        // cout << canvasScale << endl;
    }
    
    if(key == 'p'){
        showAllPoints = !showAllPoints;
    }
    
    if (key == 'd'){
        showDebug = !showDebug;
    }
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){  
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
    
}