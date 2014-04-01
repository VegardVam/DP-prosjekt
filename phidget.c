#include <phidget21.h>
#include <stdio.h>
#include <stdlib.h>
#include <phidget21.h>
#include <unistd.h>
#include <time.h>

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
int CCONV AttachHandler(CPhidgetHandle IFK, void *userptr)	
{	
    int serialNo;
    const char *name;
    CPhidget_getDeviceName(IFK, &name);
    CPhidget_getSerialNumber(IFK, &serialNo);
    printf("%s %10d attached!\n", name, serialNo);
	return 0;
}

/*
Kjører når servo og/eller sensor tilkobles
*/
int CCONV DetachHandler(CPhidgetHandle IFK, void *userptr)	
{
	int serialNo;
    const char *name;
    CPhidget_getDeviceName (IFK, &name);
    CPhidget_getSerialNumber(IFK, &serialNo);
    printf("%s %10d detached!\n", name, serialNo);
    return 0;
}
 
/*
Kjører når servo eller sensor tilkobles
Printer feilmeilding ved feil
*/
int CCONV ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown)
{
      printf("Error handled. %d - %s", ErrorCode, unknown);
      return 0;
}

/*
Blir kalt fra main.
Returnerer 0 hvis problem ved tilkobling, 1 ellers. 
*/
int initSensor(CPhidgetInterfaceKitHandle *ifKit)	//Initializes the position sensor. Creates handle for the interface, attachHandler and errorHandle
{
	int result;
	const char *err;
   	CPhidgetInterfaceKit_create(ifKit);
   	CPhidget_set_OnAttach_Handler((CPhidgetHandle)*ifKit, AttachHandler, NULL);
 	CPhidget_set_OnError_Handler((CPhidgetHandle)*ifKit, ErrorHandler, NULL);
	CPhidget_open((CPhidgetHandle)*ifKit, -1);
	printf("Waiting for interface kit to be attached....");
  	if((result = CPhidget_waitForAttachment((CPhidgetHandle)*ifKit, 3000))) 	//Waits for 5 sec for attachment then timeout
	{
		CPhidget_getErrorDescription(result, &err);
        printf("Problem waiting for attachment: %s\n", err);
        return 0; 
	}
	return 1;	
}

/*
Blir kalt fra main.
Returnerer 0 hvis problem ved tilkobling, 1 ellers. 
*/
int initServo(CPhidgetServoHandle *servo)		//Initializes the position sensor. Creates handle for the interface, attachHandler and errorHandle
{
	int result;
	const char *err;
	CPhidgetServo_create(servo);

    CPhidget_set_OnAttach_Handler((CPhidgetHandle)*servo, AttachHandler, NULL);
    CPhidget_set_OnDetach_Handler((CPhidgetHandle)*servo, DetachHandler, NULL);
    CPhidget_set_OnError_Handler((CPhidgetHandle)*servo, ErrorHandler, NULL);
    CPhidget_open((CPhidgetHandle)*servo, -1);
    printf("Waiting for Servo controller to be attached....");
    if((result = CPhidget_waitForAttachment((CPhidgetHandle)*servo, 3000)))	//Waits for 3 sec for attachment then timeout
	{
	 	CPhidget_getErrorDescription(result, &err);
        printf("Problem waiting for attachment: %s\n", err);
        return 0;	
	}
	return 1;	
}

