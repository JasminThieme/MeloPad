#pragma once 
  
#include "u8g2/U8g2LinuxI2C.h"
#include <array>
#include <vector> 

class Display {
public:
	// Constructor
	Display(); 
	
	void process_state(int estado, int sabana, int instrumento, int octava, int contador);
	
	int process();
	
	// Destructor
	~Display();

private:
	// sabanas
	#define NOSABANA 0
	#define TECLADO 1
	#define BATERIA 2
	
	// octavas
	#define OCT34 0
	#define OCT45 1
	#define OCT56 2
	
	//instrumentos
	#define RHODEY 0
	#define BEETHREE 1
	#define WURLEY 2
	
	//estados
	#define INITIAL 0
	#define CHOOSE_INST 1
	#define CHOOSE_OCT 2
	#define INFO 3  
	#define RECORD 4
	#define STOP_REC 5
	#define SAVE_MEL 6
	#define LISTA 7 
	#define PLAY 8
	#define ELIMINADO 9
	
	void switchTarget(int target); 
	
	// State variables and parameters, not accessible to the outside world 
	struct sDisplay {U8G2 d; int mux;};
	std::vector<sDisplay> gDisplay; 
	//int gStop;
	unsigned int gActiveTarget; 
	int estado;
	//std::string text[15];
	std::array<std::string,15>text;
};