/**
******************************************************
* @file ServerPic.h
* @brief Documentacion de Modulo Outlet wifi
* @author Julian Salas Bartolome
* @version 1.0
* @date Febrero 2020
*
*
* Compilado con Stino ESP 8266 2.5.2
* Opciones de compilacion
* 	Placa: 					Generic ESP8266 Module
* 	CPU Frecuency:			80 MHZ
* 	CRYSTAL Frecuency:		26 MHZ
*	Debul Level:			none
* 	Debug Pot:				Disabled
*	Esase Flash:			Only sketch
*	Expressid FW:			nonos-sdk 2.2.1 (legacy)
*   Exceptiosn:				Disabled	
*	Flash Frecuency:		40 MHZ
*	Flash Mode:				DOUT ( compatible )
*	Flash Size:				1M(64k SPIFFS)
*	IwIP Variant:			v2 Lower Memory
*	Reset Method:           ck
*	SSL SUport:             ALL SSL chiphers (most compatible)
*	Upload Speed:			115200
*	VTables:				Flash
*	
*	version: 1.0
*   Properties: xtal=80,CrystalFreq=26,lvl=None____,dbg=Disabled,wipe=none,sdk=nonosdk221,exception=disabled,FlashFreq=40,FlashMode=dout,eesz=1M,ip=lm2f,ResetMethod=ck,ssl=all,baud=115200,vt=flash 
*******************************************************/

#ifndef SERVERPIC_H
	#define SERVERPIC_H

	//----------------------------------------------
	//Includes
	//----------------------------------------------
	#include <ESP8266WiFi.h>
	#include <WiFiClient.h> 
	#include <ESP8266WebServer.h>
	#include <WiFiUdp.h>
	#include <ArduinoOTA.h>

	#include "Global.h"
	#include "Configuracion.h"
	#include "Eprom.h"
	#include "ModoAP.h"
	#include "ModoSTA.h"
	#include "Servidor.h"



	//----------------------------
	//Declaracion de variables UNIVERSALES
	//----------------------------
	ESP8266WebServer server(80);											//Definimos el objeto Servidor para utilizarlo en Modo AP
	WiFiClient Cliente;														//Definimos el objeto Cliente para utilizarlo en Servidor
	Telegrama oMensaje;									 					//Estructura mensaje donde se almacenaran los mensajes recibidos del servidor
 
	String cDispositivo = String(' ');										//Variable donde se deja el nombre del dsipositivo. Se utiliza para comunicar con HomeKit, llamar a listas ...
	String cSalida;															//Variable donde se deja el estado ( String ) para mandar en mensaje a ServerPic
	boolean lEstado = 0;													//Variable donde se deja el estado del dispositivo para reponer el estado en caso de reconexion
	boolean lConexionPerdida = 0;											//Flag de conexion perdida, se pone a 1 cuando se pierde la conexion. Se utiliza para saber si se ha perdido la conexion para restablecer estado anterior de la perdida

	boolean lHomeKit;													    //Flag para habililtar HomeKit ( 1 Habilitado, 0 deshabilitado )
	boolean lWebSocket ;  													//Flag para habililtar WebSocket ( 1 Habilitado, 0 deshabilitado )
	boolean lEstadisticas;													//Flag para habilitar Estadisticas ( 1 Habilitado, 0 Deshabilitado )
	boolean lPush ;														    //Flag donde para que indica si se usa Push o no ( 1/0 )
	String cPush = String(' ');												//Nombre del cliente push			

 
	unsigned long nMiliSegundosTest = 0;									//Variable utilizada para temporizar el Test de conexion del modulo a ServerPic


	//----------------------------
	//Declaracion de variables PARTICULARES
	//----------------------------


	boolean lOnOff = 1;														//Flag que indica si el PIR está habilitado o no												
	boolean lDispositivo = 0;												//Flag que indica que se ha dado la orden al  dispositivo remoto para que actue

	boolean lInput1 = 0;													//Flag donde se almacenará el estado de la entrada 1
	boolean lInput2 = 0;													//Flag donde se almacenará el estado de la entrada 2
	int nTipoEntrada = 0;													//Variable donde se alamcana el tipo de arranque segun tabla de definiciones tipos de netrada	

	long nTiempoSueno = 0;													//Tiempo de sueño

	long nTempoInicio = 0;													//Instante en el que se ejecuta una de las funciones de tratamiento de mode de despertado
	

	boolean lSirena = 0;
	boolean lAlarma = 0;
	long nInicioAlarma = 0;
	int nSgAlarma = 40;
	long nInicioSirena = 0;
	int nSgSirena = 30;



	//----------------------------
	//CARACTERISTICAS DISPOSITIVO
	//----------------------------
	#define ESP12							//Modelo de ESP8266
	#define Ino "pir"						//Nombre del programa principal
	#define VIno "1.0"						//Version del programa principal
	#define Placa "Nodemcu"				    //Placa utilizada
	#define VBuild  "13"
	//---------------------------------
	//CARACTERISTICAS DE LA COMPILACION
	//---------------------------------
	#define Compiler "Stino";				//Compilador
	#define VCore "2.5.2";					//Versión del Core Arduino

	#include "IO.h";


void lOnOffToRTC ( void );
void RTCTolOnOff( void );


	//----------------------------------------------
	//DEBUG
	//----------------------------------------------
	#define Debug
	//----------------------------------------------
	//Pulsador-LED !SOLO PARA OUTLET!
	//----------------------------------------------	
	#define Led
	//----------------------------------------------
	//HOME KIT
	//----------------------------------------------
	//#define HomeKit

	//----------------------------------------------
	//Teimpo de Test de conexion
	//----------------------------------------------
	#define TiempoTest	14000												//Tiempo en milisegundos para Test de conexion a servidor


	//----------------------------------------------
	//TiEmpo de rebotes
	//----------------------------------------------
 	#define TempoRebotes 150


	//----------------------------------------------
 	//Definiciones tipo de entrada
	//----------------------------------------------
	#define TipoEntrada1				1
	#define TipoEntrada2				2
	#define TipoEntradaTemporizacion	3
	#define TipoEntradaNormal			4
	#define TipoEntradaOrden			5 	

	//-----------------------------------------------
	//Tiempos de sueño en segundos
	//-----------------------------------------------
	#define TiempoSuenoNormal			60


	//Variables donde se almacenan los datos definidos anteriormente para pasarlos a Serverpic.h
	//para mandar la información del Hardware y Software utilizados
	//En la libreria ServerPic.h estan definidos como datos externos y se utilizan en la funcion
	//Mensaje () para responder al comando generico #Info.
	//Con ese comando, el dispositivo mandara toda esta información al cliente que se lo pida
	// ESTOS DATOS NO SON PARA MODIFICAR POR USUARIO
	//----------------------------
	//Datos del programa principal
	//----------------------------
	String VMain = VIno;
	String Main = Ino; 
	String VModulo = VBuild;
	//----------------------------
	//Datos de compilación
	//----------------------------	
	String Compilador = Compiler;
	String VersionCore = VCore;

	//----------------------------
	//Datos de Hardware
	//----------------------------	
	String ModeloESP = Modelo;
	String Board = Placa;



void GrabaTiempoSueno ( int nSegundos )
{
	GrabaIntEprom ( nPosicionDataUsuario, nSegundos );
	EEPROM.commit();
}
int LeeTiempoSueno ( void )
{
	int nSegundos = LeeIntEprom ( nPosicionDataUsuario );
	return ( nSegundos );
}
/**
******************************************************
* @brief Enciende Sirena
*
*/
void EnciendeSirena (void)
{
	nInicioSirena = millis();
	lSirena = 1;
	digitalWrite(PinSirena, LOW);	
}
/**
******************************************************
* @brief Apaga SIrena
*
*/
void ApagaSirena (void)
{
  digitalWrite(PinSirena, HIGH);	
  lSirena = 0;

}
#ifdef Led															
	/**
	******************************************************
	* @brief Enciende el led
	*
	*/
	void EnciendeLed (void)
	{
  		digitalWrite(PinLed, LOW);	
	}
	/**
	******************************************************
	* @brief Apaga el led
	*
	*/
	void ApagaLed (void)
	{
  		digitalWrite(PinLed, HIGH);	
	}
	/**
	******************************************************
	* @brief Hace un flash en el led
	*
	*/
	void FlashLed (void)
	{
  		EnciendeLed();
  		delay(50);
  		ApagaLed();
  		delay(50);
  		EnciendeLed();
  		delay(50);
  		ApagaLed();
  		delay(50);
  		EnciendeLed();
  		delay(50);
  		ApagaLed();
	}
#endif		
/**
******************************************************
* @brief Devielve el estado del dispositivo
*
* @return devuelve <b>1</b> si el dispositivo esta conectado o <b>0<\b> en caso de que no este conectado
*/
boolean GetDispositivo (void)
{

	return ( lOnOff );

}
/**
******************************************************
* @brief Pone el dispositivo en On
*
*/
void DispositivoOn (void)
{
	lOnOff=1;
	lOnOffToRTC();
}
/**
******************************************************
* @brief Pone el dispositivo en OPff
*
*/
void DispositivoOff (void)
{
	lOnOff=0;
	lOnOffToRTC();
}

/**
******************************************************
* @brief  Manda un Push a la cuanta de Julian
*/

boolean PushJulilan (void)
{
	Serial.print("Envio Push:");
	Serial.println(lPush);
	long nMiliSegundos = 0;
	boolean lSalida = 0;
	Cliente.print("push-:-a1q5w28e6t1yoa39en7kz73afh5txd-:-utjnmdhooubzbsozeu7vkqzjkg4j4n-:-Iphone_Julian-:-Deteccion Movimiento-:-Se ha detectado movimiento-:-updown");
	Cliente.print("\n");
	delay(200) ;					
   	nMiliSegundos = millis();
   	while ( !Cliente.available() && millis() < nMiliSegundos + 5000 ){}                         //Mientras recibamos datos del servidor
  	if ( Cliente.readStringUntil('\n') == Ok )                                                                         //Si se recibe, se ha realizado la conexion con exito
  	{
  		lSalida=1;
  	}  
  	return ( lSalida );
}
/**
******************************************************
* @brief  Manda un Push a la cuanta de Jesus
*
*/
boolean PushJesus (void)
{
	long nMiliSegundos = 0;
	boolean lSalida = 0;
	Cliente.print("push-:-ag8i5hnczdm9incg8b42zm4xpt2n7n-:-uc4niuf2dj3v82eakpduxfvkkxymtr-:-jesus-:-Sirena-:-La sirena esta encnedida-:-updown");
	Cliente.print("\n");
	delay(200) ;					
   	nMiliSegundos = millis();
   	while ( !Cliente.available() && millis() < nMiliSegundos + 5000 ){}                         //Mientras recibamos datos del servidor
  	if ( Cliente.readStringUntil('\n') == Ok )                                                                         //Si se recibe, se ha realizado la conexion con exito
  	{
  		lSalida=1;
  	}  
  	return ( lSalida );
}
/**
******************************************************
* @brief  Función que se ejecuta cuando se detecta movimiento
*
*/

void PirOn (void)
{

    oMensaje.Destinatario = cDispositivo + String("#");
    oMensaje.Mensaje = "On";
    EnviaMensaje(oMensaje);
    delay(200); 			

// 	 PushJulilan();
//  PushJesus();
}
/**
******************************************************
* @brief  Función que se ejecuta ha transcurrido un tiempo desde que se detecto presencia
*
* Serviria para desconectar  o conectar algo
*
*/
void PirOff (void)
{

    oMensaje.Destinatario = cDispositivo + String("#");
	oMensaje.Mensaje = "Off";
	EnviaMensaje(oMensaje);
	delay(200);
}
/**
******************************************************
* @brief Habilita las entradas
*
*/
void  InputsEnable (void)
{
	
	digitalWrite(Enable_1, LOW);
	digitalWrite(Enable_2, LOW);	
	
}
/**
******************************************************
* @brief Deshabilita las entradas
*
*/
void  InputsDisable (void)
{
	
	digitalWrite(Enable_1, HIGH);
	digitalWrite(Enable_2, HIGH);	
	
}
/**
* @brief Pone a dormir el módulo
*
*/
void Dormir (void)
{
	ApagaLed();
	Cliente.print("exit");                                              //Pedimos al servidor que cierre el socket
	Cliente.print("\n");                                                                  
	delay(2000);														//Damos tiempo a que se envia la petición
	//InputsEnable();														//Hbilitamos las entradas
	ESP.deepSleep (nTiempoSueno*1e6, WAKE_RF_DEFAULT);					//Ponemos el micro a dormir
}
/**
* @brief Pone en modo normal al modulo
*
*/
void Despertar (void)
{
	nTipoEntrada = TipoEntradaOrden;
	InputsDisable ();	
	EnciendeLed();
}

/**
* @brief Funciondssd de servicio para entrada 1
*
*/
void ServicioInput1 ()
{
	nTempoInicio=millis();											//Inicio temporizador para dormir
	nTipoEntrada = TipoEntrada1;										//Asignar Tipo de Entrada
	if ( lOnOff )
	{ 
		lAlarma = 1;
		EnciendeSirena();
	}	
}
/**
* @brief Funcion de servicio para entrada 2
*
*/
void ServicioInput2 (void)
{
	nTempoInicio=millis();											//Inicio temporizador para dormir
	nTipoEntrada = TipoEntrada2;									//Asignar Tipo de Entrada

}
/**
* @brief Funcion de servicio para entrada por temporizacion
*
*/
void ServicioInputTemporizacion (void)
{
	nTempoInicio=millis();												//Inicio temporizador para dormir
	nTipoEntrada = TipoEntradaTemporizacion;							//Asignar Tipo de Entrada
	nTiempoSueno = TiempoSuenoNormal;									//Tiempo normal de temporizacion
}
#endif

/**
* @brief Funcion que graba lOnOff en el bloque 100 RTC
*
*/
void lOnOffToRTC ( void )
{
	ESP.rtcUserMemoryWrite(100, (uint32 *) &lOnOff, 4);
}
/**
* @brief Funcion que lee lOnOff en el bloque 100 de RTC
*
*/
void RTCTolOnOff ( void )
{
	uint32 lOnOffRTC;
	ESP.rtcUserMemoryRead(100, (uint32*) &lOnOffRTC, sizeof(lOnOffRTC));
	lOnOff=lOnOffRTC;
}