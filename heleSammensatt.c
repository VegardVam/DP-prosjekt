#include <stdio.h>
#include <phidget21.h>
#include <math.h>
#include <unistd.h>
#include <time.h>


int CCONV AttachHandler(CPhidgetHandle IFK, void *userptr)  
{	//
      int serialNo;
      const char *name;
      CPhidget_getDeviceName(IFK, &name);
      CPhidget_getSerialNumber(IFK, &serialNo);
      printf("%s %10d attached!\n", name, serialNo);
      return 0;
  }

  int CCONV DetachHandler(CPhidgetHandle IFK, void *userptr)
  {
      int serialNo;
      const char *name;
      CPhidget_getDeviceName (IFK, &name);
      CPhidget_getSerialNumber(IFK, &serialNo);
      printf("%s %10d detached!\n", name, serialNo);
      return 0;
  }
  
  int CCONV ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown)
  {
      printf("Error handled. %d - %s", ErrorCode, unknown);
      return 0;
  }

int initSensor(CPhidgetInterfaceKitHandle *ifKit){	//Initializes the position sensor. Creates handle for the interface, attachHandler and errorHandle
	int result;
	const char *err;
   	CPhidgetInterfaceKit_create(ifKit);
   	CPhidget_set_OnAttach_Handler((CPhidgetHandle)*ifKit, AttachHandler, NULL);
 	CPhidget_set_OnError_Handler((CPhidgetHandle)*ifKit, ErrorHandler, NULL);
	CPhidget_open((CPhidgetHandle)*ifKit, -1);

	printf("Waiting for interface kit to be attached....");
  	if((result = CPhidget_waitForAttachment((CPhidgetHandle)*ifKit, 3000))) {	//Waits for 5 sec for attachment then timeout
         CPhidget_getErrorDescription(result, &err);
         printf("Problem waiting for attachment: %s\n", err);
         return 0; }
	return 1;	}

int initServo(CPhidgetServoHandle *servo){			//Initializes the position sensor. Creates handle for the interface, attachHandler and errorHandle
	int result;
	const char *err;
	CPhidgetServo_create(servo);
    CPhidget_set_OnAttach_Handler((CPhidgetHandle)*servo, AttachHandler, NULL);
    CPhidget_set_OnDetach_Handler((CPhidgetHandle)*servo, DetachHandler, NULL);
    CPhidget_set_OnError_Handler((CPhidgetHandle)*servo, ErrorHandler, NULL);
    CPhidget_open((CPhidgetHandle)*servo, -1);
    printf("Waiting for Servo controller to be attached....");
    if((result = CPhidget_waitForAttachment((CPhidgetHandle)*servo, 3000))){	//Waits for 5 sec for attachment then timeout
        CPhidget_getErrorDescription(result, &err);
        printf("Problem waiting for attachment: %s\n", err);
        return 0;	}
	return 1;	}

double getDistance(CPhidgetInterfaceKitHandle ifKit, double *diff){	//gets distance from the potMeter and checks if the potMeter goes a hole round
	double currentPosition, radius, pi;
	int temporaryCount, sensorValue;
//	diff = 0;
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
 	return currentPosition; }

double getThrust(double currentPosition,double desiredPostition, double *integral,  double dt){	//The PI-controller 
	double ki, kp, error;	
	kp = 1.5;
	ki = 0.75;
	error = desiredPostition - currentPosition;
	if ((*integral + error*dt)*ki + error*kp > 50)	//The servo gives forward thrust between 100 and 160. 
		return 50;
	else if ((*integral + error*dt)*ki + error*kp < 0)
		return 0;
	else
		*integral = *integral + error*dt;
	return kp*error + ki*(*integral);	}

void giveThrust(CPhidgetServoHandle servo, double thrust)	{	//sends thrust to the servo
	thrust = 100 + thrust;
	 CPhidgetServo_setPosition(servo, 0, thrust);}


// returns time since tstart was executed using the command :
// clock_gettime(CLOCK_REALTIME, &tstart);
// to use this function -lrt must be included in as gcc command
double getTime(struct timespec tstart){		//timer for the dt and runTime
	struct timespec	tend; 
	double timeDiff;
  	clock_gettime(CLOCK_REALTIME, &tend);
	timeDiff=(double)(tend.tv_sec + (double)tend.tv_nsec/1000000000  - (tstart.tv_sec + (double)tstart.tv_nsec/1000000000) )  ;
	return timeDiff;
}

int main(int argc, char* argv[])
  {
	int temporaryCount, result, sensorValue, errorFlag;
	double thrust, currentPosition, desiredPostition, dt, runTime, dpTime;
	double integral=0;
	double diff = 0;
	struct timespec dtStart, runTimeStart;
	FILE *f = fopen("dataTable.dat", "w");
	if (f == NULL)	{
		printf("Error opening file\n");
		return(1);
	}
	printf("Where do you want the boat to be positioned? Specify in centimeteres from the right side: ");
	scanf("%lf", &desiredPostition);	
	printf("For how long do you want the program to run? Specify time in seconds: ");
	scanf("%lf", &dpTime);
	CPhidgetServoHandle servo = 0;
	CPhidgetInterfaceKitHandle ifKit = 0;
	errorFlag = initSensor(&ifKit) + initServo(&servo);
	if( errorFlag < 2 ){
		return 0;
	}
	giveThrust(servo, 1);
	sleep(3);
  	clock_gettime(CLOCK_REALTIME, &dtStart);
	clock_gettime(CLOCK_REALTIME, &runTimeStart);
	while ( dpTime>runTime ) {
		dt = getTime(dtStart);
  		clock_gettime(CLOCK_REALTIME, &dtStart);
		currentPosition = getDistance(ifKit, &diff);
		thrust = getThrust(currentPosition, desiredPostition, &integral, dt);	
		giveThrust(servo, thrust);
		runTime = getTime(runTimeStart);
		fprintf(f, "%f	%f	%f	%f %f\n", runTime, desiredPostition, currentPosition, thrust, integral);
	}
	

	//avslutter
	FILE *gnuPlotPipe = popen("gnuplot -persisent", "w");

	gnuPlotPipe("plot '%s' using 1:2 title 'Desired position' with lines,  '%s' using 1:3 title 'Position' with lines, '%s' using 1:4 title 'Thrust' with lines,  '%s' using 1:5 title 'Integral' with lines","dataTable.dat","dataTable.dat", "dataTable.dat", "dataTable.dat");
	pclose(gnuPlotPipe);
	


	printf("Disengage. Press any key to Continue\n");
	getchar();
	CPhidgetServo_setEngaged (servo, 0, 0);
	printf("Press any key to end\n");
	getchar();
	printf("Closing...\n");
	CPhidget_close((CPhidgetHandle)servo);
	CPhidget_delete((CPhidgetHandle)servo);
	fclose(f);
	return 0;	}


