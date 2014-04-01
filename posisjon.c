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

double getDistance(CPhidgetInterfaceKitHandle ifKit, double *diff)	//gets distance from the potMeter and checks if the potMeter goes a hole round
{
	double currentPosition, radius, pi;
	int temporaryCount, sensorValue;
	pi = acos(-1);
	radius = 4.2;

    CPhidgetInterfaceKit_getSensorValue(ifKit, 0, &sensorValue);
  	temporaryCount = sensorValue;
	usleep(10000); 
    CPhidgetInterfaceKit_getSensorValue(ifKit, 0, &sensorValue);
    if ((sensorValue - temporaryCount) < (-800))
		*diff = (*diff + 1000 + sensorValue - temporaryCount); 
 	else if ((sensorValue - temporaryCount) > 800) 
 		*diff = (*diff - 1000 + sensorValue - temporaryCount); 
 	else
		*diff = (*diff + sensorValue - temporaryCount);   
 	currentPosition = 2*pi*radius*(*diff)/1000;
 	return currentPosition; 
}

 /* 
 This function filters the constant reference to obtain a smooth reference to the PI controller 
 */
 
double getReference( double ys[3], double us[3]  )
{
	double num[3] ={ 0.0000, 0.0000498, 0.0000495 };
	double denum[3] ={ 1.0000, -1.9859982, 0.9860975 };
	ys[2]=ys[1];
    ys[1]=ys[0];
    ys[0]= -(denum[1]) * (ys[1]) -(denum[2]) * (ys[2]) + num[1]*us[1] + num[2]*us[2];
    return ys[0];
}

double inPosition( int *inPositionCount, double currentPosition, double desiredPostition, double dt, double *timeInPosition )
{
	double error;
	error = (currentPosition - desiredPostition);
	printf("count %d\n", *inPositionCount);
	if ( fabs(error) < 1.5 ) {
		if ( *inPositionCount > 150 )
			*timeInPosition = (*timeInPosition + dt);
		*inPositionCount =*inPositionCount + 1; }
	else 
		*inPositionCount = 0;
	return *timeInPosition;
}



