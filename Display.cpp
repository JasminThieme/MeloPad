#include "Display.h"

Display::Display(){
	//gStop = 0;
	gActiveTarget = 0; 
	estado = 0;
	for(int i=0; i<15; i++){
		text[i] = "";
	}
	
	const unsigned int gI2cBus = 1;
 
	gDisplay = {
		// use `-1` as the last value to indicate that the display is not behind a mux, or a number between 0 and 7 for its muxed channel number
		{U8G2LinuxI2C(U8G2_R3, gI2cBus, 0x3c, u8g2_Setup_ssd1309_i2c_128x64_noname2_f), -1}
	};
			
	#ifdef I2C_MUX
	#include "TCA9548A.h"
	const unsigned int gMuxAddress = 0x70;
	TCA9548A gTca;
	#endif // I2C_MUX 
} 

void Display::switchTarget(int target){
#ifdef I2C_MUX
	if(target >= gDisplay.size())
		return;
	int mux = gDisplay[target].mux;
	static int oldMux = -1;
	if(oldMux != mux)
		gTca.select(mux);
#endif // I2C_MUX
	gActiveTarget = target;
}

void Display::process_state(int estado, int sabana, int instrumento, int octava, int contador){
	for(int i=0; i<15; i++){
		text[i] = "";
	}
	if(sabana==NOSABANA){
		if(estado == INITIAL){
			text[3] = "  MELOPAD";  
	        text[4] = "    dice  ";
	        text[5] = "  HOLA! ;)";
	        text[9] = "  Pulsa un";
	        text[10] = "   boton";
		} else if (estado == LISTA) {
			text[3] = " Melo "+std::to_string(contador);
			text[5] = "Siguiente >";
			text[7] = "Reproducir>";
			text[9] = "Anterior  >"; 
		} else if (estado == PLAY) {
			text[2] = "Escuchando";
			text[3] = " Melo "+std::to_string(contador); 
			text[7] = "Detener   >";
			text[9] = "Eliminar  >"; 
		} else if (estado == ELIMINADO) {
			text[4] = "  "+std::to_string(contador);  
	        text[5] = " eliminado "; 
	        text[9] = "  Pulsa un ";
	        text[10]= "   boton   ";
		}
	} else {
		if(estado == INITIAL){
			text[3] = "  MELOPAD";  
	        text[4] = "    dice  ";
	        text[5] = "  HOLA! ;)";
	        text[9] = "  Pulsa un";
	        text[10] = "   boton";
		} else if (estado == CHOOSE_INST) { 
	        if(sabana==TECLADO){
	    		text[0] = "Piano";
				text[3] = "Elige:";
				text[5] = "Rhodey    >";
				text[7] = "BeeThree  >";
				text[9] = "Wurley    >"; 
	        } else if (sabana==BATERIA){
	    		text[0] = "Bateria";
	    		text[3] = "  Pulsa";
				text[4] = "  para";
				text[5] = " comenzar!";
	        }
			text[1] = "conectado";
		} else if (estado == CHOOSE_OCT) {
			text[0] = "Elige";
	        text[1] = "Octava";
	        text[2] = "Para el";
			if(instrumento==RHODEY){
				text[3] = "Rhodey:"; 
			} else if (instrumento==BEETHREE){ 
				text[3] = "BeeThree:";
			} else if (instrumento==WURLEY) {
				text[3] = "Wurley:";
			} else {
	            text[3] = "Error";
	        }
			text[5] = "OCTs 3y4 >";
			text[7] = "OCTs 4y5 >";
			text[9] = "OCTs 5y6 >"; 
		} else if (estado == INFO){
	        if(instrumento==RHODEY){
				text[2] = "Rhodey"; 
			} else if (instrumento==BEETHREE){ 
				text[2] = "BeeThree";
			} else if (instrumento==WURLEY) {
				text[2] = "Wurley";
			} else {
	            text[2] = "Error";
	        }
	        text[3] = "sonando en";
	        text[4] = "  octavas";
	        if(octava==OCT34){
				text[5] = "    3y4"; 
			} else if (octava==OCT45){ 
				text[5] = "    4y5";
			} else if (octava==OCT56){
				text[5] = "    5y6";
			} else {
	            text[5] = "Error";
	        }
			text[7] = "Grabar    >";
	        text[9] = "Restart   >";
	    } else if (estado == RECORD) {
			text[4] = "Grabando...";
			text[9] = "Stop      >";
		} else if (estado == STOP_REC) {
			text[3] = "   Apunte  ";
			text[4] = "sonando...";
			text[7] = "Retry     >";
			text[9] = "Guardar   >";
		} else if (estado == SAVE_MEL) {
			text[4] = "  Apunte  ";
			text[5] = " guardado ";
			text[6] = "Genial! :)";  
	        text[9] = "  Pulsa un";
	        text[10] = "   boton";
		}	
	}
}

int Display::process(){
	if(0 == gDisplay.size()){
		fprintf(stderr, "No displays in gDisplays\n");
		return 1;
	}
#ifdef I2C_MUX
	if(gTca.initI2C_RW(gI2cBus, gMuxAddress, -1) || gTca.select(-1)){
		fprintf(stderr, "Unable to initialise the TCA9548A multiplexer. Are the address and bus correct?\n");
		return 1;
	}
#endif // I2C_MUX
	for(unsigned int n = 0; n < gDisplay.size(); ++n){
		switchTarget(n);
		U8G2& u8g2 = gDisplay[gActiveTarget].d;
#ifndef I2C_MUX
		int mux = gDisplay[gActiveTarget].mux;
		if(-1 != mux){
			fprintf(stderr, "Display %u requires mux %d but I2C_MUX is disabled\n", n, mux);
			return 1;
		}
#endif // I2C_MUX
		u8g2.initDisplay();
		u8g2.setPowerSave(0);
		u8g2.clearBuffer();
		u8g2.setFont(u8g2_font_6x10_tr); //u8g2.setFont(u8g2_font_4x6_tf);
		u8g2.setFontRefHeightText();
		u8g2.setFontPosTop(); 
		u8g2.drawStr(0, 10, text[0].c_str());
		u8g2.drawStr(0, 20, text[1].c_str());
		u8g2.drawStr(0, 30, text[2].c_str());
		u8g2.drawStr(0, 40, text[3].c_str()); 
		u8g2.drawStr(0, 50, text[4].c_str());
		u8g2.drawStr(0, 60, text[5].c_str());
		u8g2.drawStr(0, 70, text[6].c_str());
		u8g2.drawStr(0, 80, text[7].c_str());
		u8g2.drawStr(0, 90, text[8].c_str());
		u8g2.drawStr(0, 100, text[9].c_str());
		u8g2.drawStr(0, 110, text[10].c_str()); 
		if(gDisplay.size() > 1)	{
			std::string targetString = "Target ID: " + std::to_string(n);
			u8g2.drawStr(0, 50, targetString.c_str());
		}
		u8g2.sendBuffer();
	} 
	return 0;
}

// Destructor
Display::~Display() 
{
	// Nothing to do here
}
