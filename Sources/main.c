#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#define LRESET	PTCD_PTCD2

unsigned int i,j;//VARIABLES PARA QUE FUNCIONEN LAS SURUTINAS
const unsigned char Temperatura[17]={"TEMPERATURA:   C"};
const unsigned char Presion[17]	= 	{"PRESION:     KPA"};
const unsigned char Humedad[17]	= 	{"HUMEDAD:       %"};
const unsigned char lluvia[17]	= 	{"LLUVIA:    CM3/S"};
const unsigned char Viento1[17]	= 	{"VELOCIDAD DE    "};
const unsigned char Viento2[17]	= 	{"VIENTO:     KM/H"};
volatile unsigned char cont = 0;
volatile unsigned long temp=0;
volatile unsigned long aux=0,hume=0,velo=0,pre=0,lluvi=0;
volatile unsigned char aux2=0;
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

void act_dato(unsigned long help,unsigned char punto){
	unsigned int Chelp=0, Dhelp=0, Uhelp=0, vtemp=0; 
	Chelp=(unsigned int)help/100;
	vtemp=(unsigned int)help%100;
	Dhelp=vtemp/10;
	Uhelp=vtemp%10;
	cursor(punto);
	PTED=Chelp+0x30;
	dato();
	cursor(punto+1);
	PTED=Dhelp+0x30;
	dato();
	cursor(punto+2);
	PTED=Uhelp+0x30;
	dato(); 
	return;
}
void	act_msj(){
	if(cont==0){
		cursor(0b10000000);
		for(j=0;j<sizeof(Temperatura)-1;j++){
			PTED=Temperatura[j];
			dato();
		}
		act_dato(temp,0b10001100);
		cursor(0b11000000);
		for(j=0;j<sizeof(Humedad)-1;j++){
			PTED=Humedad[j];
			dato();
		}
		act_dato(hume,0b11001100);
	}
	else if(cont==1){
		cursor(0b10000000);
		for(j=0;j<sizeof(Presion)-1;j++){
			PTED=Presion[j];
			dato();
		}
		act_dato(pre,0b10001001);
		cursor(0b11000000);
		for(j=0;j<sizeof(lluvia)-1;j++){
			PTED=lluvia[j];
			dato();
		}
		act_dato(lluvi,0b11001000);
	}else{
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
		act_dato(velo,0b11001000);
	}
		
}


//-------------------FIN LCD------------
void main(void) {
	//---------------Configuraci�n del microcontrolador-------------
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
	//----------------Configura SPI-------------------------------
		SCI2BD=13;//9600 
		SCI2C2_RE=1;//habilita
	//------------------------------------------------------------	
		//---------Configuracion Timer 1-----------
		TPM2SC = 0b01000111;
		TPM2MOD = 64500;//Para 1 segundo
		TPM2SC_CLKSA = 1;	 //Habilita Timer 2
		EnableInterrupts;
  for(;;) {
	  //ENVIO DE DATOS AL TRANSMISO
  } 

}  
 
interrupt VectorNumber_Vtpm2ovf void TPM2_ISR(void){
  	TPM2SC_TOF=0;
  	//while(SCI2S1_RDRF==0);//Espera a que se llene el registro (LLega completa informacion)
  	//pre=SCI2D;//Se obtiene el dato recibido
  	if(cont==0){
  		ADCSC1=0b0000000;//Inicia conversion con canal 0- Temperatura
  		while(ADCSC1_COCO==0);//Pregunta si ya finalizo la conversion, NO SE ACTUALIZA BIT POR QE DESPUES SE SOBRE ESCRIBE
  		aux=ADCR;//Lee dato de la conversion
  		temp=(50*aux)/1023;//conversiones
  		ADCSC1=0b0000001;//Inicia conversion con canal 1- Humedad
  		while(ADCSC1_COCO==0);//Pregunta si ya finalizo la conversion, NO SE ACTUALIZA BIT POR QE DESPUES SE SOBRE ESCRIBE
  		aux=ADCR;//Lee dato de la conversion
  		hume=(120*aux)/1023;//conversiones
  		act_msj();
  		cont=cont+1;
  	}
  	else if(cont==1){
  		//ADCSC1=0b0000010;//Inicia conversion con canal 2- presion
  	  	while(SCI2S1_RDRF==0);//Espera a que se llene el registro (LLega completa informacion)
  	  	pre=SCI2D;//Se obtiene el dato recibido
  	  	pre=(pre*75)/221;
  		ADCSC1=0b0000011;//Inicia conversion con canal 3- cantidad de lluvia
  		while(ADCSC1_COCO==0);//Pregunta si ya finalizo la conversion, NO SE ACTUALIZA BIT POR QE DESPUES SE SOBRE ESCRIBE
  		aux=ADCR;//Lee dato de la conversion
  		lluvi=50-(50*aux)/1023-;//conversiones

  		
  		act_msj();
  		cont=cont+1;
  	}
  	else{
  		ADCSC1=0b0000010;//Inicia conversion con canal 4- cantidad de lluvia
  		while(ADCSC1_COCO==0);//Pregunta si ya finalizo la conversion, NO SE ACTUALIZA BIT POR QE DESPUES SE SOBRE ESCRIBE
  		aux=ADCR;//Lee dato de la conversion
  		velo=(10*aux)/1023;//conversiones
  		act_msj();
  		cont=0;
  	}
  	
  }
  
  


