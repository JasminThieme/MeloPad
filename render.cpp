#include <stdexcept> // Para std::runtime_error
#include <Debouncer.h>
#include <libraries/Trill/Trill.h> 
#include <libraries/AudioFile/AudioFile.h> 
#include "Display.h" 
#include "Teclado.h"
#include "Drums.h"

//using namespace stk;
#define NUM_CAP_CHANNELS 30   
 
#define Button1 1
#define Button2 2
#define Button3 3

int gInputPin0 = 0; // digital pin 0 
int gInputPin1 = 1; // digital pin 1 
int gInputPin2 = 2; // digital pin 2 
int gInputPin3 = 3; // digital pin 3 

// Button debouncer object
Debouncer gDebouncer1;
Debouncer gDebouncer2;
Debouncer gDebouncer3; 
Debouncer gDebouncer4; 

Display pantalla;
Teclado teclado; 
//Drums drum;

// Trill object declaration
Trill touchSensor;
// Sleep time between reads from the device
unsigned int gTaskSleepTime = 12000; // microseconds

// Initialisation of the buffer
bool initialised=false, grabado=true, copiado=false; 
std::vector<float> soundBuffer;
std::vector<float> recordedBuffer;
AudioFileWriter writer;
//const char* outPath = "./recording.wav";
const char* outPath;
std::string filename = "./melo";
std::string formato = ".wav"; 
// how many seconds to record
const double gDurationSec = 20; 
// how many frame have actually been written to soundBuffer 
unsigned int posicion_buffer; 
float gElapsedTime;

//Carga los nombres de grabaciones guardadas para su consulta
std::vector<std::string> recordings; 
std::vector<float> datos; //guarda en buffer una grabación

int estado=INITIAL, sabana=NOSABANA, octava=-1, instrumento=-1, button=-1, 
evento=1, lectura_buffer=0, contador_mel=1, next=0, ant=0, contador_grabacion=1;  

void loop(void*){
	while(!Bela_stopRequested()){
		// Read raw data from sensor
		touchSensor.readI2C();
		for(unsigned int i = 0; i < NUM_CAP_CHANNELS; i++){
			teclado.gSensorReading[i] = touchSensor.rawData[i];
			//drum.gSensorReadingDrum[i] = touchSensor.rawData[i]; 
		}
		usleep(gTaskSleepTime);
	}
} 

void state_machine (){
	if(sabana==TECLADO){
		switch (estado) {
	    case INITIAL: 
	        if(button>0){
		        estado=CHOOSE_INST;
		        evento=1;
	        }
	    break;
	    case CHOOSE_INST:
	    	//sabana=TECLADO; 
	        if(button==Button1){
	        	instrumento=RHODEY;
	        	estado=CHOOSE_OCT; 
		        evento=1; 
	        } else if(button==Button2){
	        	instrumento=BEETHREE;
	        	estado=CHOOSE_OCT; 
		        evento=1; 
	        } else if(button==Button3){
	        	instrumento=WURLEY;
	        	estado=CHOOSE_OCT; 
		        evento=1; 
	        } 
	    break;
	    case CHOOSE_OCT: 
	        if(button==Button1){
	        	octava=OCT34;
	        	estado=INFO; 
		        evento=1;
	        } else if(button==Button2){
	        	octava=OCT45;
	        	estado=INFO;
		        evento=1;
	        } else if(button==Button3){
	        	octava=OCT56;
	        	estado=INFO;
		        evento=1; 
	        }
	    break;
	    case INFO:  
			if(button==Button2){ //Grabar
				posicion_buffer = 0; 
				gElapsedTime = 0;
				grabado=false;
				estado=RECORD;
				evento=1;
			}
		    if(button==Button3){ //Volver al inicio
	        	estado=INITIAL; 
		        evento=1;
	        }
	    break;
		case RECORD:  
			//graba melodia y guardar en soundBuffer
		    if(button==Button3){ //Stop grabacion
		    	grabado=true;
	        	estado=STOP_REC; 
		        evento=1;
	        }
		break;
		case STOP_REC: 
			//reproduce melodia grabada
			if(button==Button2){ //Retry
	        	estado=INFO; 
		        evento=1;
	        }  
		    if(button==Button3){ //Guardar
	        	estado=SAVE_MEL; 
		        evento=1;
	        }
		break;
		case SAVE_MEL:
			//guarda melodía en memoria
	        if(button>0){ //Pulsa cualquier boton
		        estado=INFO;
		        evento=1;
	        }
	    break;
	    default: 
		    estado=INITIAL;
		    break;
	    } 
	} else if (sabana==BATERIA) {
		switch (estado) {
	    case INITIAL: 
	        if(button>0){
		        estado=INFO;
		        evento=1;
	        }
	    break;
	    case INFO:  
			if(button==Button2){ //Grabar
				posicion_buffer = 0; 
				gElapsedTime = 0;
				grabado=false;
				estado=RECORD;
				evento=1;
			}
		    if(button==Button3){ //Volver al inicio
	        	estado=INITIAL; 
		        evento=1;
	        }
	    break;
	    case RECORD:  
			//graba melodia y guardar en soundBuffer
		    if(button==Button3){ //Stop grabacion
		    	grabado=true;
	        	estado=STOP_REC; 
		        evento=1;
	        }
		break;
		case STOP_REC: 
			//reproduce melodia grabada
			if(button==Button2){ //Retry
	        	estado=INFO; 
		        evento=1;
	        }  
		    if(button==Button3){ //Guardar
	        	estado=SAVE_MEL; 
		        evento=1;
	        }
		break;
		case SAVE_MEL:
			//guarda melodía en memoria
	        if(button>0){ //Pulsa cualquier boton
		        estado=INFO;
		        evento=1;
	        }
	    break;
	    default: 
		    estado=INITIAL;
		    break;
	    } 
	} else if (sabana==NOSABANA) {
		switch (estado) {
	    case INITIAL: 
	        if(button>0){
		        estado=LISTA;
		        evento=1; 
	        }
	    break;
	    case LISTA: 
	    	if(button==Button1){
	    		next=1;
	    		evento=1; 
	    	}
	    	if(button==Button2){
	    		estado=PLAY;
	    		evento=1; 
	    	}
	    	if(button==Button3){
	    		ant=1;
	    		evento=1;
	    	}
    	break;
	    case PLAY:
	    	if(button==Button2){
	    		estado=LISTA;
	    		evento=1; 
	    	}
	    	if (button==Button3){
	    		estado=ELIMINADO;
	    		evento=1; 
	    	}
    	break;
    	case ELIMINADO: 
	        if(button>0){
		        estado=LISTA;
		        evento=1;
	        }
	    break;
    	default: 
		    estado=INITIAL;
		    break;
	    } 
	} 
}

void crearBuffer (unsigned int numFrames, unsigned int sampleRate) {
	if(writer.setup(outPath, numFrames, 1, sampleRate))
		fprintf(stderr, "Can't open output file\n");  
	initialised=true;
}

bool setup(BelaContext *context, void *userData){ 
    // Setup a Trill Craft sensor on i2c bus 1, using the default mode and address
	if(touchSensor.setup(1, Trill::CRAFT) != 0) {
		fprintf(stderr, "Unable to initialise Trill Craft\n"); 
	}
	touchSensor.setPrescaler(1); //sensibilidad de las entradas
	touchSensor.printDetails();
	
	// Set and schedule auxiliary task for reading sensor data from the I2C bus
	Bela_runAuxiliaryTask(loop); 
	
	// Set the mode of digital pins
	pinMode(context, 0, gInputPin0, INPUT); //set input 
	pinMode(context, 1, gInputPin1, INPUT); //set input 
	pinMode(context, 2, gInputPin2, INPUT); //set input 
	pinMode(context, 3, gInputPin3, INPUT); //set input 
	
	// Initialise the debouncer with 50ms interval
	gDebouncer1.setup(context->audioSampleRate, .05);
	gDebouncer2.setup(context->audioSampleRate, .05);
	gDebouncer3.setup(context->audioSampleRate, .05);  
	gDebouncer4.setup(context->audioSampleRate, .05);  
	
    teclado.setSampleRate(context->audioSampleRate); 
    
    //Buscar el último archivo guardado para evitar sobreescribir
    //y guardar los nombres en un vector 
	std::string str_rec; 
	bool ultimo=false;
	while(!ultimo && contador_mel<20){
		str_rec = filename + std::to_string(contador_mel) + formato;  
		try{
			if(AudioFileUtilities::loadMono(str_rec).size()>0){ 
				recordings.push_back(str_rec.c_str()); //Añade al vector de nombres existentes
				contador_mel++; 
			} else {
				throw std::runtime_error("Archivo de audio vacío");
			}
		} catch (std::runtime_error& e) {
			ultimo=true; 
	        rt_printf("Ultimo es %i\n",contador_mel);
		} 
	} 
    if (recordings.empty()) {
        rt_printf("No recordings found\n");
    } else {
    	rt_printf("%i recordings found\n",recordings.size());
    }
    
	return true;
}

void render(BelaContext *context, void *userData){   
	for(unsigned int n=0; n<context->audioFrames; ++n){ 
		//Regulador volumen
    	float Ainput = analogRead(context, n/2, 0);
    	float amplitudeDB = map(Ainput, 0, 3.3 / 4.096, -30, 16); 
    	float amplitude = powf(10.0, amplitudeDB / 20);		// Convert dB to linear amplitude 
		//Entradas digitales: botones y detector sábana
		gDebouncer1.process(digitalRead(context, 0, gInputPin0)); 	
		if(gDebouncer1.fallingEdge()) 
			button=Button1; 
		gDebouncer2.process(digitalRead(context, 0, gInputPin1));
		if(gDebouncer2.fallingEdge())  
			button=Button2;
		gDebouncer3.process(digitalRead(context, 0, gInputPin2));  
	    if(gDebouncer3.fallingEdge())
			button=Button3;
		gDebouncer4.process(digitalRead(context, 0, gInputPin3));  //detector sabana
	    if(gDebouncer4.fallingEdge()) 
			sabana=TECLADO;
		
		state_machine(); // switch entre estados
	
		// Funciones que se ejecutan 1 vez para cada estado
		if(evento==1){
	        evento=0; 
	        button=-1;
			if(next==1){
				if(contador_grabacion<contador_mel-1){
					contador_grabacion++;
				} else {
					contador_grabacion=1;
				}   
				next=0;
			} else if (ant==1){
				if(contador_grabacion>1){
					contador_grabacion--;
				} else {
					contador_grabacion=contador_mel-1; 
				}
				ant=0;
			}
	        pantalla.process_state(estado, sabana, instrumento, octava, contador_grabacion);
	        pantalla.process();  
	    	teclado.process_state(instrumento, octava); 
			if(estado==RECORD){
				unsigned int numFrames = context->audioSampleRate * gDurationSec; 
				soundBuffer.resize(numFrames);
			}
			if(estado==STOP_REC){
				unsigned int numFramesRec = context->audioSampleRate * gElapsedTime;
				recordedBuffer.resize(numFramesRec);
				lectura_buffer=0;
				copiado=false;
			}
	        if(estado==SAVE_MEL){ 
	        	std::string str = filename + std::to_string(contador_mel) + formato;
	        	outPath = str.c_str();
				unsigned int numFramesSave = context->audioSampleRate * gElapsedTime;
				crearBuffer(numFramesSave, context->audioSampleRate);
				if (initialised)
					writer.setSamples(recordedBuffer);
				contador_mel++;
				recordings.push_back(outPath);
			} 
			if(estado==PLAY){
				lectura_buffer=0;
				datos = AudioFileUtilities::loadMono(recordings[contador_grabacion-1].c_str());
			}
			/*if(estado==ELIMINADO){
				while(contador_grabacion<contador_mel){
					std::string str = recordings[contador_grabacion].c_str();
					//sobreescribir y eliminar ?
					contador_grabacion++;
				}
			}*/
		} 
		
		
		//Funcion que procesa salida auriculares
		StkFloat out = 0.0, output=0.0;   
		if(estado==INFO || estado==RECORD){
			if(sabana==TECLADO){
				teclado.process(); 
				out=teclado.output; 
			} else if (sabana==BATERIA){
				//drum.process();
				//out=drum.outDrums;
			} 
		}else if(estado==STOP_REC){
			out=soundBuffer[lectura_buffer]; 
			lectura_buffer++;
			if(!copiado)
				recordedBuffer[lectura_buffer]=out;
			if(lectura_buffer==posicion_buffer-1){
				lectura_buffer=0;
				copiado=true;
			}
		}else if(estado==PLAY){
			out=datos[lectura_buffer];
			lectura_buffer++;
			if(lectura_buffer>datos.size()-1){
				lectura_buffer=0;
			}
		}
		output=out*(amplitude+10);
		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) 
			audioWrite(context, n, channel, output);
		
		//Funcion que graba
		if(!grabado && posicion_buffer<soundBuffer.size()-1){
			soundBuffer[posicion_buffer] = out;
			posicion_buffer++;
			gElapsedTime += 1.0 / context->audioSampleRate; 
		} else if (posicion_buffer==soundBuffer.size()-1){
			button=Button3;
		}
	} 
}

void cleanup(BelaContext *context, void *userData)
{
	// Nothing to do here
}