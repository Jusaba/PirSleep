




/**
******************************************************
* @file Outlet.ino
* @brief Outlet
* @author Julian Salas Bartolome
* @version 1.0
* @date Febrero Febrero 2021
*
*
*******************************************************/


#include "ServerPic.h"
#include "IO.h"

void setup() {
	pinMode( PinWake, OUTPUT );											//Salida para despertar a ESP8266
	digitalWrite( PinWake, HIGH );										//La ponemos a 1 para que ESP8266 pueda arrancar

	digitalWrite( PinSirena, HIGH );
  	pinMode(PinSirena, OUTPUT);               							//configuramos el pin de la sirena como salida
	ApagaSirena();

	RTCTolOnOff ();
  	#ifdef Debug														//Usamos el puereto serie solo para debugar	
		Serial.begin(9600);												//Si no debugamos quedan libres los pines Tx, Rx para set urilizados
		Serial.println("Iniciando........");
	#endif
	EEPROM.begin(256);													//Reservamos zona de EEPROM
	//BorraDatosEprom ( 0, 256 );	    									//Borramos 256 bytes empezando en la posicion 0		

	Serial.print ("Leido lOnOff en RTC:" );
	Serial.println ( lOnOff );


  	pinMode(PinReset, INPUT_PULLUP);           							//Configuramos el pin de reset como entrada
	
	pinMode(Test_1, INPUT);												//Entrada para comprobar el estado de la entrada 1
	pinMode(Test_2, INPUT);												//Entrada para comprobar el estado de la entrada 2
	pinMode(Enable_1, OUTPUT);											//Salida que habilita/deshabilita la entrada 1	
	pinMode(Enable_2, OUTPUT);											//Salida que habilita/deshabilita la entrada 2


	delay(100);
																		//Para saber que entrada ha despertado al micro, leemos el estado de ambas
	lInput1=digitalRead(Test_1);										//Primero la 1		
	lInput2=digitalRead(Test_2);										//Luego la 2

 	#ifdef Led															
  		pinMode(PinLed, OUTPUT);               							//configuramos el pin del led verde como salida
		EnciendeLed();													//Lo encendemos
	#endif	
	//---------------------------------------------
	//Analisis de rntradas
    //---------------------------------------------
	if ( ESP.getResetReason() == "Deep-Sleep Wake" )						//sE SE HA DESPERTADA
	{
		if ( lInput1  )	    												//Si el arranque lo ha producido la entrada 1	
		{    					
			ServicioInput1();   	
	    }	
	    if ( lInput2 )														//Si el arranque lo ha producido la entrada 2
	    {
	    	ServicioInput2();
		}
		if ( !lInput1 && !lInput2 )											//Si ha arranacado por temporización	
		{
			ServicioInputTemporizacion();
	   }
	}
	if ( ESP.getResetReason() == "External System" )						//Si el arranque se ha producido por puesta en marcha normal
	{
		nTipoEntrada = TipoEntradaNormal;
	
	}    			

	InputsDisable();

   	#ifdef Debug
   		Serial.println("Estado de entradas");
   		Serial.println("------------------");
		Serial.print("Entrada 1: ");
		Serial.println(lInput1);
		Serial.print("Entrada 2: ");
		Serial.println(lInput2);	
		Serial.print("Tipo de entrada: ");
		Serial.println(nTipoEntrada);
	#endif		

	if ( LeeByteEprom ( FlagConfiguracion ) == 0 )						//Comprobamos si el Flag de configuracion esta a 0
	{																	// y si esta
		ModoAP();														//Lo ponemos en modo AP

	}else{																//Si no esta
		if ( ClienteSTA() )												//Lo poenmos en modo STA y nos conectamos a la SSID
		{																//Si jha conseguido conectarse a ls SSID en modo STA
	        if ( ClienteServerPic () )									//Intentamos conectar a ServerPic
    		{
//				CheckFirmware();    									//Comprobamos si el firmware esta actualizado a la ultima version
		    	#ifdef Debug
        			Serial.println(" ");
        			Serial.println("Conectado al servidor");
      			#endif 

		 		nTiempoSueno = LeeTiempoSueno();
				DataConfig aCfg = EpromToConfiguracion ();     							 //Leemos la configuracin de la EEprom
				char USUARIO[1+aCfg.Usuario.length()]; 
				(aCfg.Usuario).toCharArray(USUARIO, 1+1+aCfg.Usuario.length());          //Almacenamos en el array USUARIO el nombre de usuario 
				cDispositivo = USUARIO;
				lHomeKit = aCfg.lHomeKit;
				lWebSocket = aCfg.lWebSocket;
				lEstadisticas = aCfg.lEstadisticas;
				lPush = aCfg.lPush;														 //Almacenamos el estado de habilitacion de Push
				char CLIENTEPUSH[1+aCfg.Push.length()]; 							
				(aCfg.Push).toCharArray(CLIENTEPUSH, 1+1+aCfg.Push.length());            //Almacenamos en el array CLIENTEPUSH el nombre del cliente push 
				cPush = CLIENTEPUSH;

				if (lPush)																 //Si esta habilitado el Push, notificamos el arranque
				{
					if (lAlarma)
					{
						cSalida = "sendpush-:-"+cPush+"-:-"+cDispositivo+"-:-Alarma movimiento ";
						MensajeServidor(cSalida);			
						cSalida = String(' ');	
					}	
				}

				if ( lEstadisticas )													//Si están habilitadas las estadisticas, actualizamos el numero de inicios
				{
					GrabaVariable ("inicios", 1 + LeeVariable("inicios") );
				}


	   			#ifdef  Led																 
//	      			ApagaLed();   														 //Apagamos el Led para indicar que hay conexion con Serverpic
				#endif

    			cSalida = LeeValor();													//Arrancamos con el ultimo valor
      			if ( cSalida == "ERROR")												//Si no habia ultimo valor, arrancamos con On
      			{
      				DispositivoOn();
      			}else{																	//Si existia ultimo valor, arrancamos con el valor registrado
      				if (cSalida == "On")
      				{
      					DispositivoOn();
      				}
      				if (cSalida == "Off")
      				{
      					DispositivoOff();
      				}	

      			}	
				if (nTipoEntrada == TipoEntradaTemporizacion)
				{
					Buzon();
				}      			
      			cSalida ="";


    		}
    	}		
	}

}


void loop() {

	    long Inicio, Fin, nTempo;

	    if (lSirena && (millis() > nInicioSirena + (nSgSirena*1000)))
	    {
			lAlarma = 0;
			ApagaSirena();
	    }
	    if ( nTipoEntrada == TipoEntrada2 || nTipoEntrada == TipoEntradaTemporizacion || nTipoEntrada == TipoEntradaOrden ) 	
	    {

 			/*----------------
 			Comprobacion Reset
 			------------------*/
			TestBtnReset (PinReset);

 			/*----------------
 			Comprobacion Conexion
 			------------------*/

			if ( TiempoTest > 0 )
			{
				if ( millis() > ( nMiliSegundosTest + TiempoTest ) )			//Comprobamos si existe conexion  
				{


				  	#ifdef  Led													//Si no esta definido Debug
//				        EnciendeLed();        									//Encendemos el led para indicar que se comprueba la conexion
				    #endif    
					nMiliSegundosTest = millis();
					if ( !TestConexion(lEstadisticas) )							//Si se ha perdido la conexion
					{
						lConexionPerdida = 1;									//Ponemos el flag de perdida conexion a 1
						if ( GetDispositivo() )									//Si el dispositivo estaba a On
						{
							lEstado = 1;										//Guardamos ese estado para tenerlo en cuenta en la reconexion
							DispositivoOff();									//Ponemos a Off el dispositivo	
						}	
					}else{														//Si existe conexion
					  	#ifdef  Led												//Si no esta definido Debug
//				    	    ApagaLed();											//Apagamos el led para indicar que se ha finalizado el test                                                                                      
					    #endif    
						if ( lConexionPerdida )									//Comprobamos si es una reconexion ( por perdida anterior )
						{														//Si lo es
							lConexionPerdida = 0;								//Reseteamos flag de reconexion
//							digitalWrite(PinSalida, lEstado);					//POnemos el dispositivo en el estado anterior a la perdida de conexion
							if ( lEstado )
							{
								DispositivoOn();
							}else{
								DispositivoOff();	
							}
							lEstado = 0;										//Reseteamos el Flag lEstado
						}	
					}	
 	   			}	
    		}
    	}
 		/*----------------
 		Analisis comandos
 		------------------*/
		oMensaje = Mensaje ();								 			//Iteractuamos con ServerPic, comprobamos si sigue conectado al servidor y si se ha recibido algun mensaje

		if ( oMensaje.lRxMensaje)										//Si se ha recibido ( oMensaje.lRsMensaje = 1)
		{
	    	#ifdef Debug				
				Serial.println(oMensaje.Remitente);						//Ejecutamos acciones
				Serial.println(oMensaje.Mensaje);
			#endif	
			#ifdef Led
				FlashLed();												//Flash de led que indica que se ha recibido mensaje			
			#endif
			//En este punto empieza el bloque de programa particular del dispositivo segun la utilizacion					

			//--------------------------------------------------------------------------------------------
			//Comandos Genericos
			//
			// On.- Conecta el pir
			// Off.- Desconecta la alrma del pir
			// Change.- Cambia el estado del pir
			// Get.- Devuelve al remitente el estado del pir
			// ChangeGet.- Cambia el estado del pir y devuelve el nueco estado al remitente
			//--------------------------------------------------------------------------------------------

			if (oMensaje.Mensaje == "On")								//Si se recibe "On", se pone PinSalida a '1'
			{	
				DispositivoOn();	
				cSalida = "On";
			}
			if (oMensaje.Mensaje == "Off")								//Si se recibe "Off", se pone PinSalida a '0'
			{	
				DispositivoOff();	
				ApagaSirena();
				cSalida = "Off";
			}
			if (oMensaje.Mensaje == "Change")							//Si se recibe 'Change', cambia el estado de PinSalida 
			{	
				if ( GetDispositivo() )
				{
					DispositivoOff();
					ApagaSirena();
					cSalida = "Off";
				}else{
					DispositivoOn();
					cSalida = "On";
				}
			}
			if (oMensaje.Mensaje == "Get")								//Si se recibe 'Get', se devuelve el estado de PinSalida al remitente
			{	
				if ( GetDispositivo() )
				{
					cSalida = "On";
				}else{
					cSalida = "Off";
				}
				oMensaje.Mensaje = cSalida;
				oMensaje.Destinatario = oMensaje.Remitente;
				EnviaMensaje(oMensaje);	
				cSalida = String(' ');					//No ha habido cambio de estado, Vaciamos cSalida para que no se envie a WebSocket y a HomeKit 
			}						
			if (oMensaje.Mensaje == "ChangeGet")						//Si se recibe 'ChangeGet', cambia el estado de PinSalida y devuelve el nuevo estado al remitente 
			{	
				if ( GetDispositivo() )
				{
					DispositivoOff();
					cSalida = "Off";
					ApagaSirena();
				}else{
					DispositivoOn();
					cSalida = "On";					
				}
				oMensaje.Mensaje = cSalida;								//Confeccionamos el mensaje a enviar hacia el servidor	
				oMensaje.Destinatario = oMensaje.Remitente;
				EnviaMensaje(oMensaje);									//Y lo enviamos
			}			
			//--------------------------------------------------------------------------------------------
			//Comandos Particulares
			//
			// Dormir.- Pasa el pir al modo sueño
			// Despertar.- Pasa el pir a modo normal. Esta orden, se ejecutara en el momento que el pir despierte por temporización y la recupere mediante Buzon()
			// Sirena.- 
			//		 Sirena.- Indica si la sirena está encendida o no
			//       Sirena-:-On.- Hace sonar la sirena durante el tiempo nSgSirena o hasta que se pare mediante comando
			//       Sirena-:-Off.- Para la sirena 
			// TSuenoNormal.- Establece los segundos en los que el pir permanece en modo sueño en circunstancias normales
			//       TSuenoNormal-:-nSgSueño 
			//--------------------------------------------------------------------------------------------

			if (oMensaje.Mensaje.indexOf( "Sirena") == 0)						//Si se recibe 'Sirena', Se analiza si el comando lleva la orden ( On/Off ) o no 
			{	
				if (oMensaje.Mensaje.indexOf( "Sirena-:-") == 0)				//Si lleva orden, se ejecuta
				{
					String cValor = String(oMensaje.Mensaje).substring(  3 + String(oMensaje.Mensaje).indexOf("-:-"),  String(oMensaje.Mensaje).length() ); //Extraemos el valor (On/Off) deseado para la sirena				
					if ( cValor == "On" )
					{
						lAlarma = 1;
						EnciendeSirena();
					}else{
						lAlarma = 0;
						ApagaSirena();
					}
				}else{															//Si no lleva orden, se devuelve el estado
					if ( lSirena )
					{
						cSalida = "On";
					}else{
						cSalida = "Off";
					}
					oMensaje.Mensaje = cSalida;
					oMensaje.Destinatario = oMensaje.Remitente;
					EnviaMensaje(oMensaje);	
					cSalida = String(' ');										//No ha habido cambio de estado, Vaciamos cSalida para que no se envie a WebSocket y a HomeKit 
				}	
			}			
			if (oMensaje.Mensaje == "Dormir")								//Si se recibe 'Get', se devuelve el estado de PinSalida al remitente
			{	
	    		#ifdef Debug
					Serial.println("Durmiendo");
				#endif
				Dormir();
 				cSalida = String(' ');					//No ha habido cambio de estado, no debemos actualizar ni homekit ni websocket
 
			}	
			if (oMensaje.Mensaje == "Despertar")
			{
	    		#ifdef Debug
					Serial.println("Despertar");
				#endif
				Despertar();
 				cSalida = String(' ');					//No ha habido cambio de estado, no debemos actualizar ni homekit ni websocket
 
			}

			if ( (oMensaje.Mensaje).indexOf("TSuenoNormal") == 0 )		//Si se recibe una orden de "TA-:-"
			{															
	
				if ( (oMensaje.Mensaje).indexOf("TSuenoNormal-:-") == 0 )
				{
					String cValor = String(oMensaje.Mensaje).substring(  3 + String(oMensaje.Mensaje).indexOf("-:-"),  String(oMensaje.Mensaje).length() ); //Extraemos el valor TA deseado
	        		int nValor = (cValor+'\0').toInt();						//Convertimos el valor String a variable int																			
		    		#ifdef Debug
        				Serial.print("TS: ");						
						Serial.println(nValor);
  					#endif
  					GrabaTiempoSueno(nValor);
  					delay(100);
		 			nTiempoSueno = LeeTiempoSueno();
					cSalida = String(' ');					//No ha habido cambio de estado, no debemos actualizar ni homekit ni websocket
				}else{
					nTiempoSueno = LeeTiempoSueno();
					cSalida = String(nTiempoSueno);
					oMensaje.Mensaje = cSalida;
					oMensaje.Destinatario = oMensaje.Remitente;
					EnviaMensaje(oMensaje);	
					cSalida = String(' ');					//No ha habido cambio de estado, Vaciamos cSalida para que no se envie a WebSocket y a HomeKit 
				}
			}		

			//--------------------------------------------------------------------------------------------
			//Comandos particulares de Dispositivo
			//
			// GetInicioWeb.- Comando para mandar a web los datos del dispositivo. Se utiliza cuando se arranca la pagina para recibir todos los datos del dispositivo ( web-:-Inicio )
			//                Si es termostato remoto, manda N/R ( no rele ), si es local manda el estado del rele y si es multiple, manda el estado de los tres reles
			//                                         web-:-Inicio-:-<N/R o Estado Rele 1>-:-[Estado Rele 2-:-Estado Rele 3]-:-Temperarura-:-UmbralI-:-UmbralS-:-lTipoAlarma-:-SgAlarmaI-:-SgAlarmaS-:-Zona-:-lAlarma  
			// GetEstado.- Igual que 'GetInicioWeb' pero la información se envia al remitente, se utiliza para conocer la informacion del dispositivo desde cualquier otro dispositivo
			//--------------------------------------------------------------------------------------------


			if ( (oMensaje.Mensaje).indexOf("GetInicioWeb") == 0 )					//Si se recibe una orden de "GetInicioWeb", mandamos a web los datos del termostato
			{
				nTipoEntrada = TipoEntradaOrden;									//Como se pide desde Web, le decimos que despierte y se mantenga asi hasta que lo durmamos				
				cSalida = "web-:-Inicio" ;
				if ( lSirena )
				{
					cSalida = cSalida + "-:-On";
				}else{
					cSalida = cSalida + "-:-Off";
				}
				if ( GetDispositivo() )
				{
					cSalida = cSalida + "-:-On";
				}else{
					cSalida = cSalida + "-:-Off";
				}				
				cSalida = cSalida + "-:-" + (String)nTiempoSueno;
				MensajeServidor(cSalida);
				cSalida = String(' ');												//Vaciamos cSalida para que no se envie a  HomeKit 
			}	



	 		/*----------------
 			Actualizacion ultimo valor
 			------------------*/
			if ( cSalida != String(' ') )				//Si hay cambio de estado
			{	
				EnviaValor (cSalida);					//Actualizamos ultimo valor
			}

	 		/*----------------
 			Actualizacion WebSocket
 			------------------*/
			#ifdef WebSocket
				if ( cSalida != String(' ') && lWebSocket )				//Si está habililtado WebSocket y hay cambio de estado en el dispositivo, se notifica a WebSocket
				{	
					EnviaMensajeWebSocket(cSalida);						//Actualizamos las páginas html conectadas
				}
	 		#endif

	 		/*----------------
 			Notificacion HomeKit
 			------------------*/
			#ifdef HomeKit
				if ( cSalida != String(' ') && oMensaje.Remitente != ( cDispositivo + String("_") ) && lHomeKit ) //Si está habililtadoHomeKit y hay cambio de estado en el dispositivo, se notifica a HomeKit
				{
					oMensaje.Destinatario = cDispositivo + String("_");
					oMensaje.Mensaje = cSalida;
					EnviaMensaje(oMensaje);
				}
			#endif		
			
			cSalida = String(' ');										//Limpiamos cSalida para iniciar un nuevo bucle

			if ( lEstadisticas )									 	//Si están habilitadas las estadisticas, actualizamos el numero de comandos recibidos
			{
				GrabaVariable ("comandos", 1 + LeeVariable("comandos") );
			}	

			nMiliSegundosTest = millis();								//Refrescamos nMiliSegundosTest para no hacer test de conexión por que si se ha recibido mensaje es que hay conexion	
	
		}	
	    if (  (nTipoEntrada == TipoEntrada1) && !lSirena ) 	    	
    	{	
    		
	   			#ifdef Debug
					Serial.println("Durmiendo");
				#endif
				Dormir();
		}	
	    if ( nTipoEntrada == TipoEntradaTemporizacion || nTipoEntrada == TipoEntradaNormal ) 	    	
    	{	
    		 	#ifdef Debug
					Serial.println("Durmiendo");
				#endif
				Dormir();
		}	
	    wdt_reset(); 								//Refrescamos WDT

}