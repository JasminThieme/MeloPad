#pragma once 
#include <Bela.h>
#include <stk/Rhodey.h> 
#include <stk/BeeThree.h>  
#include <stk/Wurley.h>  
#include "Display.h"
#include <memory>
using namespace stk;

class Teclado {
public:
	// Constructor
	Teclado(); 
	
    void setSampleRate(double sampleRate);

    void setAudioFrames(double audioFrames);

	void process_state(int instrumento, int octava);  
	
	void process(); 
    
	// Readings for all the different pads on the Trill Craft
	StkFloat gSensorReading[30] = { 0.0 };
	
    //generated sample
    StkFloat output;
	
	// Destructor
	~Teclado();

private:    
    #define NUM_CAP_CHANNELS 30 
    #define TECLAS2OCT 24
    #define F_OCT34 130.81
    #define F_OCT45 261.62
    #define F_OCT56 523.25
    #define MAX_NOTES 3   

    // Create an instrument instance    
    std::unique_ptr<Instrmnt> oscT[MAX_NOTES];
    double frequencyOCT34[TECLAS2OCT] = { 0.0 }; 
    double frequencyOCT45[TECLAS2OCT] = { 0.0 };
    double frequencyOCT56[TECLAS2OCT] = { 0.0 };
    double ChosenFrequency[TECLAS2OCT] = { 0.0 };

    // Array to store frequencies for each note
    double frequencies[MAX_NOTES] = { 0.0 };    

    //sample rate & audioframes 
    double sampleRate_; 
};