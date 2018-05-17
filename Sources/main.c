#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#define LRESET	PTCD_PTCD2

unsigned int i,j;//VARIABLES PARA QUE FUNCIONEN LAS SURUTINAS
const unsigned char Temperatura[17]={"TEMPERATURA:  C°"};
const unsigned char Presion[17]	= {"PRESION:     KPA"};
const unsigned char Humedad[17]	= {"HUMEDAD:	   %"};
const unsigned char lluvia[17]	= {"LLUVIA: 	    "};
const unsigned char Viento1[17]	= {"VELOCIDAD DE    "};
const unsigned char Viento2[17]	= {"VIENTO:     KM/H"};
char cont = 0;
int aux=0,temp=0,hume=0,velo=0,pre=0,lluvi=0;
//----------RUTINAS LCD-----------------------
//--------------------------------------------

void tiempo(int t){
	for(i=0;i<t;i++);
	return;	
}
void salto(){
	PTDD_PTDD1=1;
	for(i=0;i<20;i++);
	PTDD_PTDD1=0;
	tiempo(50);
	return;
}

void comando(){
	PTDD_PTDD0=0;
	salto();
	return;
}
void cursor(char pos){
	PTED=pos;
	comando();
	return;
}
void dato(){
	PTDD_PTDD0=1;
	salto();		
	return;
}

void	act_msj(char m){
	if(m==0){
		cursor(0b10000000);
		for(j=0;j<sizeof(Temperatura)-1;j++){
			PTED=Temperatura[j];
			dato();
		}
		cursor(0b11000000);
		for(j=0;j<sizeof(Humedad)-1;j++){
			PTED=Humedad[j];
			dato();
		}
	}
	if(m==1){
		cursor(0b10000000);
		for(j=0;j<sizeof(Presion)-1;j++){
			PTED=Presion[j];
			dato();
		}
		cursor(0b11000000);
		for(j=0;j<sizeof(lluvia)-1;j++){
			PTED=lluvia[j];
			dato();
		}
	}
	if(m==2){
		cursor(0b10000000);
		for(j=0;j<sizeof(Viento1)-1;j++){
			PTED=Viento1[j];
			dato();
		}
		cursor(0b11000000);
		for(j=0;j<sizeof(Viento2)-1;j++){
			PTED=Viento2[j];
			dato();
		}
	}
		
}


//-------------------FIN LCD------------
void main(void) {
	//---------------Configuración del microcontrolador-------------
	//--------------------------------------------------------------
	SOPT1 = 0;//Deshabilita W D
	MCGTRM = 0xAA; // Configura freciencia a 8 MHz
	MCGC1 = 6;//Bus interno
	while(MCGSC_LOCK == 0){}; // Espera que se enganche
	PTCDD=0b00000100;//Habilita las salidas
	LRESET=1;
	tiempo(5000);
	LRESET=0;
	//-------Configuracion ADC--------------
	ADCCFG=0b01001000;//selecciona modo de operacion(2-3) 10 bit , fuente de reloj bus, configuracion de tiempo de muestreo corto, rapido
	APCTL1=0b00011111; // Habilita pines B0-B5
	ADCSC1=0b00001111;// Conversion desabilitada, deshabilitado conversion continua, interupcion desabilitada, Coco
	//--------------------------------------------------------
	//-------Configuracion LCD---------------
	//--------------------------------------------------------
		PTEDD=0XFF;//Configura salidas Para LCD
		tiempo(50000);
		//PTGPE=0XFF;
		PTDDD=3;//CONFIGURA SALIDAS R-W
		PTDD_PTDD1=0;
		PTED=0b00111000;
		comando();
		PTED=0b00000110;
		comando();
		PTED=0b00001100;
		comando();
		PTED=0b00000001;
		comando();
		tiempo(20000);
		act_msj(0);
	//------------------------------------------------------------	
		//---------Configuracion Timer 1-----------
		TPM2SC = 0b01000111;
		TPM2MOD = 62000;//Para 1 segundo
		TPM2SC_CLKSA = 1;	 //Habilita Timer 2
		EnableInterrupts;
  for(;;) {
  } 

}  
 
interrupt VectorNumber_Vtpm2ovf void TPM2_ISR(void){
  	TPM2SC_TOF=0;
  	if(cont==0){
  		ADCSC1=0b0000000;//Inicia conversion con canal 0- Temperatura
  		while(ADCSC1_COCO==0);//Pregunta si ya finalizo la conversion, NO SE ACTUALIZA BIT POR QE DESPUES SE SOBRE ESCRIBE
  		aux=ADCR;//Lee dato de la conversion
  		temp=(5*aux)/1023;//conversiones
  		
  		ADCSC1=0b0000001;//Inicia conversion con canal 1- Humedad
  		while(ADCSC1_COCO==0);//Pregunta si ya finalizo la conversion, NO SE ACTUALIZA BIT POR QE DESPUES SE SOBRE ESCRIBE
  		aux=ADCR;//Lee dato de la conversion
  		hume=(5*aux)/1023;//conversiones
  		cont=1;
  	}
  	if(cont==1){
  		ADCSC1=0b0000010;//Inicia conversion con canal 2- presion
  		while(ADCSC1_COCO==0);//Pregunta si ya finalizo la conversion, NO SE ACTUALIZA BIT POR QE DESPUES SE SOBRE ESCRIBE
  		aux=ADCR;//Lee dato de la conversion
  		pre=(5*aux)/1023;//conversiones
  		  		
  		ADCSC1=0b0000011;//Inicia conversion con canal 3- cantidad de lluvia
  		while(ADCSC1_COCO==0);//Pregunta si ya finalizo la conversion, NO SE ACTUALIZA BIT POR QE DESPUES SE SOBRE ESCRIBE
  		aux=ADCR;//Lee dato de la conversion
  		lluvi=(5*aux)/1023;//conversiones
   		cont=2;
  	}
  	if(cont==2){
  		ADCSC1=0b0000100;//Inicia conversion con canal 4- cantidad de lluvia
  		while(ADCSC1_COCO==0);//Pregunta si ya finalizo la conversion, NO SE ACTUALIZA BIT POR QE DESPUES SE SOBRE ESCRIBE
  		aux=ADCR;//Lee dato de la conversion
  		velo=(5*aux)/1023;//conversiones
  		cont=0;
  	}
  	act_msj(cont);
  }
  
  

