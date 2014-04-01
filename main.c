#include <stdio.h>
#include <stdlib.h>
#include <phidget21.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#include "phidget.h"
#include "tid.h"
#include "posisjon.h"
#include "thrust.h"
#include "skrivVtf.h"


int main(int argc, char* argv[])
{
	int temporaryCount, result, sensorValue, errorFlag, i;
	double thrust, currentPosition, desiredPostition, dt, dpTime,  runTime, firstPostition, nextPosition;
	int inPositionCount = 0;
	double integral=0;
	double diff = 0;
	double ys[]={0,0,0};    // < output from reference system
	double timeInPosition = 0;
	int secondPosition = 0;
	int count = 0;	// counts number of iteration for while-loop in main to know the lenght for the vtfFile
	FILE *f = fopen("dataTable.dat", "w");
	char * commandsForGnuplot[] = {"set title \"DP-oversikt\"", "plot 'dataTable.dat' using 1:2 title 'Desired position' with lines, 'dataTable.dat' using 1:3 title 'Position' with lines"}; //, 'dataTable.dat' using 1:4 title 'Thrust' with lines, 'dataTable.dat' using 1:5 title 'Integral' with lines"};
	if (f == NULL)	
	{
		printf("Error opening file\n");
		return(1);
	}
	printf("Posisjon måles i centimeter fra tankens høyre kant. Vær snill å angi båtens første posisjon, tall mellom 0-60:  \n");
	scanf("%lf", &firstPostition);	
	printf("Vær snill å angi båtens andre posisjon, tall mellom 0-60: \n");
	scanf("%lf", &nextPosition);
	printf("Kjøretid angis i sekunder. Klokken begynner å telle etter denne dialogen er gjennomført og USBer er tilkoblet. Båtens posisjon vil alternere mellom første og andre oppgitte posisjon. En klokke vil begynne å telle når båten har stabilisert seg på ønsket posisjon og holde seg der før den går til neste posisjon. Slik forsetter den til kjøretiden er ute. Hvor lenge ønsker de at programmet skal kjøre? \n\n");
	scanf("%lf", &dpTime);
  	desiredPostition = firstPostition;
	double us[]={desiredPostition,desiredPostition,desiredPostition}; // < input from reference system
	int  arraySize = (int) (dpTime*100);
	double *currentPositionTable;
	double *desiredPositionTable;
 	currentPositionTable = (double*) malloc(arraySize*sizeof(double));
	desiredPositionTable = (double*) malloc(arraySize*sizeof(double));
	struct timespec dtStart, runTimeStart;
	CPhidgetServoHandle servo = 0;
	CPhidgetInterfaceKitHandle ifKit = 0;
	errorFlag = initSensor(&ifKit) + initServo(&servo);
	if( errorFlag < 2 )
		return 0;
	giveThrust(servo, 1);
	sleep(3);
  	clock_gettime(CLOCK_REALTIME, &dtStart);
	clock_gettime(CLOCK_REALTIME, &runTimeStart);
	while ( 1)//dpTime>runTime ) //&& count < arraySize)	 
	{
		++count;
		dt = getTime( dtStart );
  		clock_gettime( CLOCK_REALTIME, &dtStart );
		currentPosition = getDistance( ifKit, &diff );
		desiredPostition = getReference(ys,us);
		thrust = getThrust( currentPosition, desiredPostition, &integral, dt );	
		giveThrust( servo, thrust );
		runTime = getTime( runTimeStart );
		timeInPosition = inPosition( &inPositionCount, currentPosition, desiredPostition, dt, &timeInPosition );	
		if ( timeInPosition > 5 && secondPosition == 0 )
		{
			us[0] = nextPosition;
			us[1] = nextPosition;
			us[2] = nextPosition;
			timeInPosition = 0;
			secondPosition = 1;
		}
		else if ( timeInPosition > 5 && secondPosition == 1 )
		{ 
			us[0] = firstPostition; 
			us[1] = firstPostition; 
			us[2] = firstPostition;
			timeInPosition = 0;
			secondPosition = 0;
		}	 
		printf("ønsket pos%f current %f diff%f second%d thrust%f integral%f runTime%f \n",desiredPostition, currentPosition, desiredPostition - currentPosition, secondPosition, thrust, integral, runTime);
		fprintf(f, "%f	%f	%f	%f %f \n", runTime, desiredPostition, currentPosition, thrust, integral);
	}
	
	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
	
	for ( i=0; i < 2; i++)
	{
   	fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
	}

	printf("DP-program ferdig. Total kjøretid: %.2f.\n Trykk en tast for å skrive .vtf filen som GLView trenger for visualisering. \n", runTime);
	getchar();
	produceVtfFile( currentPositionTable, desiredPositionTable, arraySize );
	CPhidgetServo_setEngaged (servo, 0, 0);
	printf("Press any key to end\n");
	getchar();
	printf("Closing...\n");
	CPhidget_close((CPhidgetHandle)servo);
	CPhidget_delete((CPhidgetHandle)servo);
	fclose(f);
	pclose(gnuplotPipe);
	return 0;	
}









