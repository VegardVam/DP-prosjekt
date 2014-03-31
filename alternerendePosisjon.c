#include <stdio.h>
#include <phidget21.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

/**Kobler til handler 
*
*
*
*
*Her kommer funk*/
int CCONV AttachHandler(CPhidgetHandle IFK, void *userptr)	
{	
    int serialNo;
    const char *name;
    CPhidget_getDeviceName(IFK, &name);
    CPhidget_getSerialNumber(IFK, &serialNo);
    printf("%s %10d attached!\n", name, serialNo);
	return 0;
}

/**Kobler fra  handler 
*
*
*
*
*Her kommer funk*/
int CCONV DetachHandler(CPhidgetHandle IFK, void *userptr)	
{
	int serialNo;
    const char *name;
    CPhidget_getDeviceName (IFK, &name);
    CPhidget_getSerialNumber(IFK, &serialNo);
    printf("%s %10d detached!\n", name, serialNo);
    return 0;
}
 
/** Denne rutinene håndterer feil ved tilkobling 
*
*
*
*
*Her kommer funk*/ 
int CCONV ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown)
{
      printf("Error handled. %d - %s", ErrorCode, unknown);
      return 0;
}

/**Initsierer potsiometer på løpehjulpet 
*kobler til potensiometer som er koble til interfacekitet
*
*venter i 3 sekunder for vellykket tilkobling før den timer ut og returner 0
*Ved vellykket tilkobling returnerer den 1
*Her kommer rutinene*/
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

// returns time since tstart was executed using the command :
// clock_gettime(CLOCK_REALTIME, &tstart);
// to use this function -lrt must be included in as gcc command
double getTime(struct timespec tstart)		//timer for the dt and runTime
{
	struct timespec	tend; 
	double timeDiff;
  	clock_gettime(CLOCK_REALTIME, &tend);
	timeDiff=(double)(tend.tv_sec + (double)tend.tv_nsec/1000000000  - (tstart.tv_sec + (double)tstart.tv_nsec/1000000000) )  ;
	return timeDiff;
}

 /* This function filters the constant reference to obtain a smooth reference to the PI controller */
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

int main(int argc, char* argv[])
{
	int temporaryCount, result, sensorValue, errorFlag, i, dpTime;
	double thrust, currentPosition, desiredPostition, dt, runTime, firstPostition, nextPosition;
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
	printf("Where do you want the boat to be positioned? Specify in centimeteres from the right side: ");
	scanf("%lf", &firstPostition);	
	printf("Please state the next position you would like the boat to be positioned in: ");
	scanf("%lf", &nextPosition);
	printf("For how long do you want the program to run? The DP-program will alternate to place the boat to the desired position until the time is over. Specify time in seconds: ");
	scanf("%d", &dpTime);
  	desiredPostition = firstPostition;
	double us[]={desiredPostition,desiredPostition,desiredPostition}; // < input from reference system
	double currentPositionTable[dpTime*100];
	double desiredPositionTable[dpTime*100];
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
	while ( dpTime>runTime ) 
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
		printf("%f %f %f %d \n",desiredPostition, currentPosition, desiredPostition - currentPosition, secondPosition);
		fprintf(f, "%f	%f	%f	%f %f \n", runTime, desiredPostition, currentPosition, thrust, integral);
	}
	
	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
	
	for ( i=0; i < 2; i++)
	{
   	fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
	}

	printf("DP-program ferdig. Total kjøretid: %.2f.\n Trykk en tast for å skrive .vtf filen som GLView trenger for visualisering. \n", runTime);
	getchar();
	produceVtfFile( currentPositionTable, desiredPositionTable, count );
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

void produceVtfFile(double currentPositionTable[], double desiredPositionTable[], int count)
{
 
	FILE *vtfFile = fopen("/home/lokal/Ceetron/GLvInova_9.1-3_x86_64/tutorials/simulering.vtf","w");
  
	int line, step;
	fprintf(vtfFile,"*VTF-1.00\n\n\n");
	fprintf(vtfFile,"*NODES 1\n");
	fprintf(vtfFile,"0 14.5 0\n0 36.5 0\n77 14.5 0\n77 36.5 0\n0 14.5 10\n0 36.5 10\n77 14.5 10\n77 36.5 10\n\n\n");
	fprintf(vtfFile,"*NODES 2\n");
	fprintf(vtfFile,"0 0 0\n0 51 0\n149 51 0\n149 0 0\n\n\n");
	fprintf(vtfFile,"*ELEMENTS 1\n%%NODES #1\n%%QUADS\n1 2 4 3\n5 6 8 7\n1 2 6 5\n3 4 8 7\n1 3 7 5\n2 4 8 6\n\n\n"); 
	fprintf(vtfFile,"*ELEMENTS 2\n%%NODES #2\n%%QUADS\n1 2 3 4\n\n");
	fprintf(vtfFile,"*GLVIEWGEOMETRY 1\n%%ELEMENTS\n1 2\n\n");
 
	for ( line=0 ; line<count ; line++ )
	{
		fprintf( vtfFile,"*RESULTS %d\n", line+1 );
      	fprintf( vtfFile,"%%DIMENSION 3\n" );
 	    fprintf( vtfFile,"%%PER_NODE #1\n" );
 	    for ( step = 0 ; step < 8 ; step++ )
 	    {
	        fprintf( vtfFile,"%f 0 0\n", currentPositionTable[line] );
        }
	    fprintf(vtfFile,"\n");
	}

	fprintf(vtfFile,"*GLVIEWVECTOR 1\n");
    fprintf(vtfFile,"%%NAME \n");
  
    for ( line=0 ; line < count ; line++ )
    {
        fprintf(vtfFile,"%%STEP %d\n %d\n",line,line);
    }
    return;
}

