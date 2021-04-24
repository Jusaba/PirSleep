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


	boolean lOnOff = 1;						//Flag que indica si el PIR está habilitado o no												
	boolean lDispositivo = 0;				//Flag que indica que se ha dado la orden al  dispositivo remoto para que actue

	boolean lInput1 = 0;					//Flag donde se almacenará el estado de la entrada 1
	boolean lInput2 = 0;					//Flag donde se almacenará el estado de la entrada 2
	int nTipoEntrada = 0;					//Variable donde se alamcana el tipo de arranque segun tabla de definiciones tipos de netrada	

	long nTiempoSueno = 0;					//Tiempo de sueño


	boolean lSirena = 0;					//Flag que indica si la sirena esta On
	boolean lAlarma = 0;					//Flag que indica si esta en alarma
	long nInicioAlarma = 0;					//Variable que almacena el instante del inicio de la alarma
	long nInicioDespierto = 0;				//Variable donde se almacena el instante en que se despierta el pir
	long nInicioSirena = 0;					//Variable donde se almacena el inicio de activacion de la sirena	
	int nSgAlarma = 40;						//Numero de segundos de la alarma. Tiempo que esperara el pir para dormir una vez activada la alarma de movimiento
	int nSgSirena = 30;						//Numero de segundos de la sirena

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

	//----------------------------
	//Declaracion de funciones UNIVERSALES
	//----------------------------

	boolean GetDispositivo (void);
	void DispositivoOn (void);
	void DispositivoOff (void);

	//----------------------------
	//Declaracion de funciones PARTICULARES
	//----------------------------


	void GrabaTiempoSueno ( int nSegundos );
	int LeeTiempoSueno ( void );
	void EnciendeSirena (void);
	void ApagaSirena (void);
	void BeepSirena (void);
	void lOnOffToRTC ( void );
	void RTCTolOnOff( void );

	#ifdef Led															
		void EnciendeLed (void);
		void ApagaLed (void);	
		void FlashLed (void);
	#endif


	void PirOn (void);
	void PirOff (void);

	void  InputsEnable (void);
	void  InputsDisable (void);	

	void Dormir (void);
	void Despertar (void);

	boolean ServicioInput1 (void);
	void ServicioInput2 (void);

	void ServicioInputTemporizacion (void);

	void lOnOffToRTC ( void );
	void RTCTolOnOff ( void );

	void lConexionPerdidaToRTC ( void );
	void RTCTolConexionPerdida ( void );

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
	#define TiempoDespiertoNormal		60

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
void GrabaTiempoSirena ( int nSegundos )
{
	GrabaIntEprom ( nPosicionDataUsuario + 2, nSegundos );
	EEPROM.commit();
}
int LeeTiempoSirena ( void )
{
	int nSegundos = LeeIntEprom ( nPosicionDataUsuario + 2 );
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
/**
******************************************************
* @brief BeepSIrena
*
*/
void BeepSirena (void)
{
  digitalWrite(PinSirena, LOW);
  delay(100);
  digitalWrite(PinSirena, HIGH);
  delay(100);  
  digitalWrite(PinSirena, LOW);
  delay(100);
  digitalWrite(PinSirena, HIGH);
  delay(100);    
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
	if ( !lSirena )
	{
		BeepSirena();
	}	
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
	BeepSirena();

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
* @brief Funcion de servicio para entrada 1
*
* Comprueba si el pir esta deshabilitado o no, si lo esta, pone el flag de Alarma a 1
* @return.- Devuelve el estado 1/0 ( Pir habilidtado o deshabilitado )
*/
boolean ServicioInput1 ()
{
	boolean lSalida = 0;
	nTipoEntrada = TipoEntrada1;									//Asignar Tipo de Entrada
	if ( lOnOff )
	{ 
		lAlarma = 1;
		EnciendeSirena();
		lSalida = 1;
	}	
	return (lSalida);
}
/**
* @brief Funcion de servicio para entrada 2
*
*/
void ServicioInput2 (void)
{
	nTipoEntrada = TipoEntrada2;									//Asignar Tipo de Entrada

}
/**
* @brief Funcion de servicio para entrada por temporizacion
*
*/
void ServicioInputTemporizacion (void)
{
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
/**
* @brief Funcion que graba lConexionPerdida en el bloque 101 RTC
*
*/
void lConexionPerdidaToRTC ( void )
{
	ESP.rtcUserMemoryWrite(101, (uint32 *) &lConexionPerdida, 4);
}
/**
* @brief Funcion que lee lConexionPerdida en el bloque 101 de RTC
*
*/
void RTCTolConexionPerdida ( void )
{
	uint32 lConexionPerdidaRTC;
	ESP.rtcUserMemoryRead(101, (uint32*) &lConexionPerdidaRTC, sizeof(lConexionPerdidaRTC));
	lConexionPerdida=lConexionPerdidaRTC;
}