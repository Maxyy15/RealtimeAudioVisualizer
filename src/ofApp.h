#pragma once

#include "ofMain.h"
#include "ofxGist.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void audioIn(ofSoundBuffer& input);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		// Audio input and analysis
		ofSoundStream soundStream;
		ofxGist gistAnalyzer;  // Initialize Gist analyzer
		
		// Audio buffer and features
		vector<float> audioBuffer;
		vector<float> mfcc;
		vector<float> lastMFCC;
		vector<std::vector<float>> mfccHistory;
		int historySize = 10;
		int coeffNum;  // Number of MFCC coefficients

		int sampleSize;
		int bufferSize;

		// Smoothing factor for MFCC values
		float mfccSmoothFactor = 0.1f;  // Adjust this factor to control the smoothness


		float pitch = 0.0f;
		float note = 0.0f;
		float rms = 0.0f;
		float spectralCentroid = 0.0f;
		float spectralCrest = 0.0f;
		float zeroCrossingRate = 0.0f;
		float peakEnergy = 0.0f;
		float spectralFlatness = 0.0f;
		float spectralDifference = 0.0f;
		float spectralDifferenceComplex = 0.0f;
		float spectralDifferenceHalfway = 0.0f;
		float highFreqContent = 0.0f;

		float smoothFactor = 0.1f;  // The smoothing factor, adjust this value
		float lastPitch = 0;
		float lastNote = 0;
		float lastRMS = 0;
		float lastSpectralCentroid = 0;
		float lastSpectralCrest = 0;
		float lastZeroCrossingRate = 0.0f;
		float lastPeakEnergy = 0.0f;
		float lastSpectralFlatness = 0.0f;
		float lastSpectralDifference = 0.0f;
		float lastSpectralDifferenceComplex = 0.0f;
		float lastSpectralDifferenceHalfway = 0.0f;
		float lastHighFreqContent = 0.0f;
};

