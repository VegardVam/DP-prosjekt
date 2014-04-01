#include <stdio.h>
#include <stdlib.h>
#include <phidget21.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

/* 
	--------------------------------------------------------------------
	DP-Program    			Modul: Posisjon      			No.:1
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

double getThrust(double currentPosition,double desiredPostition, double *integral,  double dt)	//The PI-controller 
{
	double ki, kp, error;	
	kp = 3;
	ki = 1;
	error = desiredPostition - currentPosition;
	if ((*integral + error*dt)*ki + error*kp > 60)	//The servo gives forward thrust between 100 and 160. 
		return 60;
	else if ((*integral + error*dt)*ki + error*kp < 0)
		return 0;
	else
		*integral = *integral + error*dt;
	return kp*error + ki*(*integral);	
}

void giveThrust(CPhidgetServoHandle servo, double thrust)	//sends thrust to the servo
{
	thrust = 100 + thrust;
	CPhidgetServo_setPosition(servo, 0, thrust);
}

