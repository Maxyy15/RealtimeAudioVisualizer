#include "ofApp.h"

// This audio visualizer takes in audio from the microphone, 
// I tried to find a way to get desktop audio through the code but either due to Windows 11  
// or just lack of knowledge I wasn't able to find anything so I have it set up on my computer to take in audio through
// my Stereo Mix (sound card) by setting it as my default device in both recording and playback. 
// If anyone has a better way to do this please let me know


//--------------------------------------------------------------
void ofApp::setup() {
    bufferSize = 1028;
    sampleSize = 48000;

    audioBuffer.resize(bufferSize);

    ofSoundStreamSetup(0, 2, this, sampleSize, bufferSize, 8); // Set up audio input

    // Setup the analyzer
    gistAnalyzer.setup();

    // Enable the features you want to detect
    gistAnalyzer.setDetect(GIST_PITCH, true);  // Enable pitch detection
    gistAnalyzer.setDetect(GIST_ROOT_MEAN_SQUARE, true);  // Enable RMS detection
    gistAnalyzer.setDetect(GIST_SPECTRAL_CENTROID, true);  // Enable Spectral Centroid
    gistAnalyzer.setDetect(GIST_SPECTRAL_CREST, true);  // Enable Spectral Crest
    gistAnalyzer.setDetect(GIST_ZERO_CROSSING_RATE, true);  // Enable Zero Crossing Rate
    gistAnalyzer.setDetect(GIST_NOTE, true);  // Enable Note detection
    gistAnalyzer.setDetect(GIST_PEAK_ENERGY, true);  // Enable Peak Energy detection
    gistAnalyzer.setDetect(GIST_SPECTRAL_FLATNESS, true);  // Enable Spectral Flatness
    gistAnalyzer.setDetect(GIST_SPECTRAL_DIFFERENCE, true);  // Enable Spectral Difference
    gistAnalyzer.setDetect(GIST_SPECTRAL_DIFFERENCE_COMPLEX, true);  // Enable Spectral Difference Complex
    gistAnalyzer.setDetect(GIST_SPECTRAL_DIFFERENCE_HALFWAY, true);  // Enable Spectral Difference Halfway
    gistAnalyzer.setDetect(GIST_HIGH_FREQUENCY_CONTENT, true);  // Enable High Frequency Content
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer& buffer) {
     bufferSize = buffer.getNumFrames();
    audioBuffer.resize(bufferSize);

    // Combine stereo channels into mono
    for (size_t i = 0; i < audioBuffer.size(); ++i) {
        audioBuffer[i] = (buffer[i * 2] + buffer[i * 2 + 1]) / 2.0f;
    }

    // Process audio and extract MFCC
    gistAnalyzer.processAudio(audioBuffer, audioBuffer.size(), bufferSize, sampleSize);

    coeffNum = gistAnalyzer.getMelFrequencyCepstralCoefficients().size();  // Ensure this gets the correct size
    mfcc.resize(coeffNum, 0.0f);
    lastMFCC.resize(coeffNum, 0.0f);

    mfcc = gistAnalyzer.getMelFrequencyCepstralCoefficients();

    
    // Check MFCC size and print values
    /*if (mfcc.size() > 0) {
        ofLog() << "MFCC Size: " << mfcc.size();
        for (size_t i = 0; i < mfcc.size(); ++i) {
            ofLog() << "MFCC " << i << ": " << mfcc[i];
        }
    }
    else {
        ofLog() << "MFCC coefficients are empty!";
    }*/

    // Apply smoothing
    for (size_t i = 0; i < mfcc.size(); ++i) {
        mfcc[i] = mfccSmoothFactor * mfcc[i] + (1.0f - mfccSmoothFactor) * lastMFCC[i];
        lastMFCC[i] = mfcc[i];
    }
}


//--------------------------------------------------------------
void ofApp::update(){
    //mfcc.clear();
    //mfcc.resize(coeffNum, 0.0f);

    //lastMFCC.clear();
    //lastMFCC.resize(coeffNum, 0.0f);
    // Extract the MFCC coefficients
    //mfcc = gistAnalyzer.getMelFrequencyCepstralCoefficients();


    // Ensure the sizes match before proceeding
    if (lastMFCC.size() != mfcc.size()) {
        // Resize lastMFCC only when necessary
        lastMFCC.resize(mfcc.size(), 0.0f);
       // ofLog() << "Resized lastMFCC to match mfcc size.";
    }

    // Apply exponential moving average smoothing for each MFCC coefficient
    for (size_t i = 0; i < mfcc.size(); ++i) {
        if (i < lastMFCC.size()) {
            mfcc[i] = mfccSmoothFactor * mfcc[i] + (1.0f - mfccSmoothFactor) * lastMFCC[i];
            lastMFCC[i] = mfcc[i];  // Store the smoothed value for the next frame
        }
    }

    pitch = gistAnalyzer.getValue(GIST_PITCH);
    note = gistAnalyzer.getValue(GIST_NOTE);
    rms = gistAnalyzer.getValue(GIST_ROOT_MEAN_SQUARE);
    spectralCentroid = gistAnalyzer.getValue(GIST_SPECTRAL_CENTROID);
    spectralCrest = gistAnalyzer.getValue(GIST_SPECTRAL_CREST);
    zeroCrossingRate = gistAnalyzer.getValue(GIST_ZERO_CROSSING_RATE);
    peakEnergy = gistAnalyzer.getValue(GIST_PEAK_ENERGY);
    spectralFlatness = gistAnalyzer.getValue(GIST_SPECTRAL_FLATNESS);
    spectralDifference = gistAnalyzer.getValue(GIST_SPECTRAL_DIFFERENCE);
    spectralDifferenceComplex = gistAnalyzer.getValue(GIST_SPECTRAL_DIFFERENCE_COMPLEX);
    spectralDifferenceHalfway = gistAnalyzer.getValue(GIST_SPECTRAL_DIFFERENCE_HALFWAY);
    highFreqContent = gistAnalyzer.getValue(GIST_HIGH_FREQUENCY_CONTENT);

    // Ensure MFCC coefficients are not cleared
    if (lastMFCC.size() != mfcc.size()) {
        lastMFCC.resize(mfcc.size(), 0.0f);
    }

    // Apply exponential moving average smoothing for each MFCC coefficient
    for (size_t i = 0; i < mfcc.size(); ++i) {
        mfcc[i] = mfccSmoothFactor * mfcc[i] + (1.0f - mfccSmoothFactor) * lastMFCC[i];
        lastMFCC[i] = mfcc[i];  // Store the smoothed value for the next frame
    }


    // Apply exponential moving average smoothing to reduce fluctuations
    pitch = smoothFactor * pitch + (1.0f - smoothFactor) * lastPitch;
    note = smoothFactor * note + (1.0f - smoothFactor) * lastNote;
    rms = smoothFactor * rms + (1.0f - smoothFactor) * lastRMS;
    spectralCentroid = smoothFactor * spectralCentroid + (1.0f - smoothFactor) * lastSpectralCentroid;
    spectralCrest = smoothFactor * spectralCrest + (1.0f - smoothFactor) * lastSpectralCrest;
    zeroCrossingRate = smoothFactor * zeroCrossingRate + (1.0f - smoothFactor) * lastZeroCrossingRate;
    peakEnergy = smoothFactor * peakEnergy + (1.0f - smoothFactor) * lastPeakEnergy;
    spectralFlatness = smoothFactor * spectralFlatness + (1.0f - smoothFactor) * lastSpectralFlatness;
    spectralDifference = smoothFactor * spectralDifference + (1.0f - smoothFactor) * lastSpectralDifference;
    spectralDifferenceComplex = smoothFactor * spectralDifferenceComplex + (1.0f - smoothFactor) * lastSpectralDifferenceComplex;
    spectralDifferenceHalfway = smoothFactor * spectralDifferenceHalfway + (1.0f - smoothFactor) * lastSpectralDifferenceHalfway;
    highFreqContent = smoothFactor * highFreqContent + (1.0f - smoothFactor) * lastHighFreqContent;

    lastPitch = pitch;
    lastNote = note;
    lastRMS = rms;
    lastSpectralCentroid = spectralCentroid;
    lastSpectralCrest = spectralCrest;
    lastZeroCrossingRate = zeroCrossingRate;
    lastPeakEnergy = peakEnergy;
    lastSpectralDifference = spectralDifference;
    lastSpectralDifferenceComplex = spectralDifferenceComplex;
    lastSpectralDifferenceHalfway = spectralDifferenceHalfway;
    lastHighFreqContent = highFreqContent;
    lastSpectralFlatness = spectralFlatness;

    pitch = ofClamp(pitch, 0.0f, 1000.0f);  // Example: Clamp pitch value
    note = ofClamp(note, 0.0f, 1000.0f);    // Example: Clamp note value
    rms = ofClamp(rms, 0.0f, 1000.0f);  // Example: Clamp pitch value
    spectralCentroid = ofClamp(spectralCentroid, 0.0f, 1000.0f);    // Example: Clamp note value
    spectralCrest = ofClamp(spectralCrest, 0.0f, 1000.0f);  // Example: Clamp pitch value
    zeroCrossingRate = ofClamp(note, 0.0f, 1000.0f);    // Example: Clamp note value
    peakEnergy = ofClamp(peakEnergy, 0.0f, 1000.0f);  // Example: Clamp pitch value
    spectralDifference = ofClamp(spectralDifference, 0.0f, 1000.0f);    // Example: Clamp note value
    spectralDifferenceComplex = ofClamp(spectralDifferenceComplex, 0.0f, 1000.0f);  // Example: Clamp pitch value
    spectralDifferenceHalfway = ofClamp(spectralDifferenceHalfway, 0.0f, 1000.0f);    // Example: Clamp note value
    highFreqContent = ofClamp(highFreqContent, 0.0f, 1000.0f);  // Example: Clamp pitch value
    spectralFlatness = ofClamp(spectralFlatness, 0.0f, 1000.0f);  // Example: Clamp pitch value

}

//--------------------------------------------------------------
void ofApp::draw(){
    // Basic visualization on the screen
    ofBackground(0);
    ofSetColor(255);
    // Clear the screen
    ofClear(0);  // Black background

    // Visualization for each feature
    float yOffset = 30;
    ofSetColor(255, 255, 255);  // White color for text

    // Visualize Pitch
    ofDrawBitmapString("Pitch", 20, yOffset);
    ofDrawRectangle(120, yOffset - 10, pitch * 10, 20);  // Bar with height proportional to pitch value
    yOffset += 30;

    // Visualize Note
    ofDrawBitmapString("Note", 20, yOffset);
    ofDrawRectangle(120, yOffset - 10, note * 10, 20);  // Bar with height proportional to note value
    yOffset += 30;

    // Visualize RMS
    ofDrawBitmapString("RMS", 20, yOffset);
    ofDrawRectangle(120, yOffset - 10, rms * 100, 20);  // Bar with height proportional to RMS
    yOffset += 30;

    // Visualize Spectral Centroid
    ofDrawBitmapString("Spectral Centroid", 20, yOffset);
    ofDrawRectangle(120, yOffset - 10, spectralCentroid * 5, 20);  // Bar with height proportional to Spectral Centroid
    yOffset += 30;

    // Visualize Spectral Crest
    ofDrawBitmapString("Spectral Crest", 20, yOffset);
    ofDrawRectangle(120, yOffset - 10, spectralCrest * 5, 20);  // Bar with height proportional to Spectral Crest
    yOffset += 30;

    // Visualize Zero Crossing Rate
    ofDrawBitmapString("Zero Crossing Rate", 20, yOffset);
    ofDrawRectangle(120, yOffset - 10, zeroCrossingRate * 10, 20);  // Bar with height proportional to Zero Crossing Rate
    yOffset += 30;

    // Visualize Peak Energy
    ofDrawBitmapString("Peak Energy", 20, yOffset);
    ofDrawRectangle(120, yOffset - 10, peakEnergy * 10, 20);  // Bar with height proportional to Peak Energy
    yOffset += 30;

    // Visualize Spectral Flatness
    ofDrawBitmapString("Spectral Flatness", 20, yOffset);
    ofDrawRectangle(120, yOffset - 10, spectralFlatness * 50, 20);  // Bar with height proportional to Spectral Flatness
    yOffset += 30;

    // Visualize Spectral Difference
    ofDrawBitmapString("Spectral Difference", 20, yOffset);
    ofDrawRectangle(120, yOffset - 10, spectralDifference * 5, 20);  // Bar with height proportional to Spectral Difference
    yOffset += 30;

    // Visualize Spectral Difference Complex
    ofDrawBitmapString("Spectral Difference Complex", 20, yOffset);
    ofDrawRectangle(120, yOffset - 10, spectralDifferenceComplex * 5, 20);  // Bar with height proportional to Spectral Difference Complex
    yOffset += 30;

    // Visualize Spectral Difference Halfway
    ofDrawBitmapString("Spectral Difference Halfway", 20, yOffset);
    ofDrawRectangle(120, yOffset - 10, spectralDifferenceHalfway * 5, 20);  // Bar with height proportional to Spectral Difference Halfway
    yOffset += 30;

    // Visualize High Frequency Content
    ofDrawBitmapString("High Frequency Content", 20, yOffset);
    ofDrawRectangle(120, yOffset - 10, highFreqContent * 5, 20);  // Bar with height proportional to High Frequency Content
    yOffset += 30;

    // Visualize MFCC coefficients with smoothing (if needed)
    yOffset += 20;
    ofDrawBitmapString("MFCC", 20, yOffset);
    for (size_t i = 0; i < mfcc.size(); ++i) {
        yOffset += 20;
        float mfccValue = std::max(mfcc[i], 0.0f);  // Clamp to 0 if negative
        ofDrawBitmapString("MFCC " + ofToString(i), 40, yOffset);
        ofDrawRectangle(120, yOffset - 10, mfccValue * 50, 20);  // Scaled bar height for MFCC
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
//void ofApp::visualizeAudio(const vector<float>& melSpectrum) {
//    // Draw a simple visualization for now (you can modify this)
//    for (size_t i = 0; i < melSpectrum.size(); i++) {
//        float val = melSpectrum[i] * 500.0f; // Scaling factor
//        ofDrawRectangle(i * 10, ofGetHeight() - val, 8, val);
//    }
//}


    //// Update MFCC coefficients
    //mfcc = gistAnalyzer.getMelFrequencyCepstralCoefficients();

    //// Ensure that lastMFCC has the correct size
    //if (lastMFCC.size() != mfcc.size()) {
    //    lastMFCC.resize(mfcc.size(), 0.0f);  // Resize lastMFCC to match mfcc size
    //}

    //// Apply exponential moving average smoothing for each MFCC coefficient
    //for (size_t i = 0; i < mfcc.size(); ++i) {
    //    mfcc[i] = mfccSmoothFactor * mfcc[i] + (1.0f - mfccSmoothFactor) * lastMFCC[i];
    //    lastMFCC[i] = mfcc[i];  // Store the smoothed value for the next frame
    //}

//// Ensure bufferSize is consistent and doesn't change dynamically
    //bufferSize = buffer.getNumFrames();
    //// Resize audioBuffer to match the number of frames in the input buffer
    //audioBuffer.resize(bufferSize);

    //if (buffer.getNumChannels() >= 2) {
    //    // Combine left and right channels into a mono signal (average the two channels)
    //    for (size_t i = 0; i < audioBuffer.size(); ++i) {
    //        audioBuffer[i] = (buffer[i * 2] + buffer[i * 2 + 1]) / 2.0f;  // Average left and right channels
    //    }
    //}
    //else {
    //    // If it's mono, just copy the data as is
    //    for (size_t i = 0; i < audioBuffer.size(); ++i) {
    //        audioBuffer[i] = buffer[i];
    //    }
    //}

    //// Process audio buffer
    //gistAnalyzer.processAudio(audioBuffer, audioBuffer.size(), bufferSize, sampleSize);

    //// Extract MFCC coefficients
    //mfcc = gistAnalyzer.getMelFrequencyCepstralCoefficients();

    //// Apply smoothing to the MFCC data before updating the visualizer
    //for (size_t i = 0; i < mfcc.size(); ++i) {
    //    mfcc[i] = mfccSmoothFactor * mfcc[i] + (1.0f - mfccSmoothFactor) * lastMFCC[i];
    //    lastMFCC[i] = mfcc[i];
    //}
//    // Dynamically set bufferSize to match the number of frames in the incoming audio buffer
//    bufferSize = buffer.getNumFrames(); // Set bufferSize to the actual size of the buffer
//
//    // Resize audioBuffer to match the number of frames in the input buffer
//    audioBuffer.resize(bufferSize);
//
//    // Check if the buffer has stereo channels (left and right)
//    if (buffer.getNumChannels() >= 2) {
//        // Combine left and right channels into a mono signal (average the two channels)
//        for (size_t i = 0; i < audioBuffer.size(); ++i) {
//            audioBuffer[i] = (buffer[i * 2] + buffer[i * 2 + 1]) / 2.0f;  // Average left and right channels
//        }
//    }
//    else {
//        // If it's mono, just copy the data as is
//        for (size_t i = 0; i < audioBuffer.size(); ++i) {
//            audioBuffer[i] = buffer[i];
//        }
//    }
//
//    // Process the audio buffer with the analyzer
//    if (bufferSize > 0) {
//        gistAnalyzer.processAudio(audioBuffer, audioBuffer.size(), bufferSize, sampleSize);
//    }
//
//    // For MFCC, use getMelFrequencyCepstralCoefficients()
//    mfcc = gistAnalyzer.getMelFrequencyCepstralCoefficients();
//
//    // Add the current MFCC to the history
//    mfccHistory.push_back(mfcc);
//
//    // Keep the history size limited
//    if (mfccHistory.size() > historySize) {
//        mfccHistory.erase(mfccHistory.begin());  // Remove the oldest MFCC
//    }
//
//    // Calculate the average MFCC
//    std::vector<float> avgMFCC(mfcc.size(), 0.0f);
//    for (auto& frame : mfccHistory) {
//        for (size_t i = 0; i < frame.size(); ++i) {
//            avgMFCC[i] += frame[i];
//        }
//    }
//
//    // Average over all frames
//    for (size_t i = 0; i < avgMFCC.size(); ++i) {
//        avgMFCC[i] /= mfccHistory.size();
//    }
//
//    // Update the current MFCC with the average
//    mfcc = avgMFCC;
//
//    // Apply exponential smoothing (same as your current update logic)
//    for (size_t i = 0; i < mfcc.size(); ++i) {
//        if (i < lastMFCC.size()) {
//            mfcc[i] = mfccSmoothFactor * mfcc[i] + (1.0f - mfccSmoothFactor) * lastMFCC[i];
//            lastMFCC[i] = mfcc[i];  // Store the smoothed value for the next frame
//        }
//    }