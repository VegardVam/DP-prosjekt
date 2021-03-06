#ifndef _My_PHIDGET_
#define _My_PHIDGET_

#include <phidget21.h>
/* 
	--------------------------------------------------------------------
	DP-Program    			Modul: Tilkobling      			No.:1
	--------------------------------------------------------------------

	Hensikt :
	Rutiner for initsiering av interfacekit og servo, oppretti
	Metode :


	Kall sekvens .......................................................

	K A L L :


	I N T E R N E   V A R I A B L E :
	*err        			Feilflagg

	F U K S J O N E R :
	AttachHandler		Kjører når servo eller sensor tilkobles 
	DetachHandler		Kjører når servo eller sensor tilkobles
	ErrorHandler			Printer feilmelding ved problemer ved initsiering
	initSensor			Initsierer posisjonssensoren
	initServo			Initsierer servoen 

	R E T U R N E R E R :
	initSensor			Returnerer 0 ved timeout ved tilkobling, ellers 1
	initServo			Returnerer 0 ved timeout ved tilkobling, ellers 1



	Programmert av 		Vegard Vamråk
	Dato			   		03/31/14 22:16:44 

	**********************************************************************
*/

/*
Kjører når servo og/eller sensor tilkobles
*/
int CCONV AttachHandler(CPhidgetHandle IFK, void *userptr);

/*
Kjører når servo og/eller sensor tilkobles
*/
int CCONV DetachHandler(CPhidgetHandle IFK, void *userptr);
 
/*
Kjører når servo eller sensor tilkobles
Printer feilmeilding ved feil
*/
int CCONV ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown);

/*
Blir kalt fra main.
Returnerer 0 hvis problem ved tilkobling, 1 ellers. 
*/
int initSensor(CPhidgetInterfaceKitHandle *ifKit);

/*
Blir kalt fra main.
Returnerer 0 hvis problem ved tilkobling, 1 ellers. 
*/
int initServo(CPhidgetServoHandle *servo);

#endif
