#include "Teclado.h"

Teclado::Teclado(){
	for(unsigned int n = 0; n < TECLAS2OCT; n++) { //2 octavas = 24 notas
		float freq = F_OCT34 * pow(2.0,n/12.0);
		frequencyOCT34[n]=freq; 
	}
	for(unsigned int n = 0; n < TECLAS2OCT; n++) { //2 octavas = 24 notas
		float freq = F_OCT45 * pow(2.0,n/12.0);
		frequencyOCT45[n]=freq; 
	}
	for(unsigned int n = 0; n < TECLAS2OCT; n++) { //2 octavas = 24 notas
		float freq = F_OCT56 * pow(2.0,n/12.0);
		frequencyOCT56[n]=freq; 
	} 
} 

void Teclado::setSampleRate(double sampleRate){
    sampleRate_ = sampleRate;
	Stk::setSampleRate(sampleRate_);
}  

void Teclado::process_state(int instrumento, int octava){
	if (instrumento==RHODEY){
        // Create instances of Rhodey instrument for each note
        for(int i = 0; i < MAX_NOTES; ++i) { 
            oscT[i].reset(new Rhodey());
            oscT[i]->setFrequency(0); // Initialize frequency to 0 
        }  
    } else if (instrumento==BEETHREE){
		// Create instances of BeeThree instrument for each note
        for(int i = 0; i < MAX_NOTES; ++i) { 
            oscT[i].reset(new BeeThree());
            oscT[i]->setFrequency(0); // Initialize frequency to 0
        } 
    } else if (instrumento==WURLEY){  
		// Create instances of BeeThree instrument for each note
        for(int i = 0; i < MAX_NOTES; ++i) { 
            oscT[i].reset(new Wurley());
            oscT[i]->setFrequency(0); // Initialize frequency to 0
        } 
    }
    if(octava==OCT34){ 
		for (int i = 0; i < TECLAS2OCT; ++i) {
		    ChosenFrequency[i] = frequencyOCT34[i];
		}
	} else if(octava==OCT45){
		for (int i = 0; i < TECLAS2OCT; ++i) {
		    ChosenFrequency[i] = frequencyOCT45[i];
		};
	} else if(octava==OCT56){
		for (int i = 0; i < TECLAS2OCT; ++i) {
		    ChosenFrequency[i] = frequencyOCT56[i];
		}
	}  
}

void Teclado::process(){   
	output=0.0; 
	// Iterate over the capacitive inputs
    for(unsigned int o = 0; o < NUM_CAP_CHANNELS; ++o) {
        if(gSensorReading[o] > 0.1) {
            // Find an available slot
            int freeSlot = -1;
            for(int i = 0; i < MAX_NOTES; ++i) {
                if(frequencies[i] == 0.0) {
                    freeSlot = i;
                    break;
                }
            } 
            // If a free slot is found, assign the note to it
            if(freeSlot != -1) {
                frequencies[freeSlot] = ChosenFrequency[o]; 
                oscT[freeSlot]->noteOn(frequencies[freeSlot], 0.21);
            }
        }
    }

    // Mix the outputs of all active instruments
    for(int i = 0; i < MAX_NOTES; ++i) {
        if(frequencies[i] != 0.0) {
            output += oscT[i]->tick();
        }
    }  
	
	// Check if any note should be turned off
    for(int i = 0; i < MAX_NOTES; ++i) {
        if(frequencies[i] != 0.0 && gSensorReading[i] <= 0.1) {
            oscT[i]->noteOff(0.21);
            frequencies[i] = 0.0;
        }
    }  
}

// Destructor
Teclado::~Teclado() 
{
	// Nothing to do here
}
