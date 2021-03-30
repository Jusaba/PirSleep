/**
******************************************************
* @file IO.h
* @brief Documentacion de Modulo Outlet wifi
* @author FabLab Lleida
* @author Jose Martinez Molero
* @author Julian Salas Bartolome
* @version 1.0
* @date Abril 2018
*
*
*******************************************************/
#ifndef IO_H
	#define IO_H


	#ifdef ESP01
		#define Modelo "ESP01"
	#endif
	#ifdef ESP11
		#define Modelo "ESP11"
	#endif		
	#ifdef ESP12
		#define Modelo "ESP12"
		int PinReset = 4;	
		int PinLed = 2;											
		int PinSirena = 13;
		int Test_1 = 12; 
		int Test_2 = 14;
		int Enable_1 = 10;
		int Enable_2 = 5;
		int PinWake = 16;
	#endif	

#endif
/*
GPIO16: pin is high at BOOT
GPIO0: boot failure if pulled LOW
GPIO2: pin is high on BOOT, boot failure if pulled LOW
GPIO15: boot failure if pulled HIGH
GPIO3: pin is high at BOOT
GPIO1: pin is high at BOOT, boot failure if pulled LOW
GPIO10: pin is high at BOOT
GPIO9: pin is high at BOOT

Oye, los
GPIO 4 y 5 son los únicos que siempre son de alta impedancia. Todos los demás tienen pull-ups internos o incluso se conducen bajo / alto durante el arranque.

Los GPIO 3, 12, 13 y 14 tiraron ALTO durante el arranque. Su estado real no influye en el proceso de arranque.

Los GPIO 0, 1, 2 y 15 se colocan en ALTO durante el arranque y también en BAJO durante períodos cortos.
El dispositivo no arrancará si 0, 1 o 2 son BAJOS durante el arranque.

GPIO 16 se maneja ALTO durante el arranque, no cortocircuite a GND.

*/