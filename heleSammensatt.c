#include <stdio.h>
#include <phidget21.h>
#include <math.h>
#include <unistd.h>
#include <time.h>


int CCONV AttachHandler(CPhidgetHandle IFK, void *userptr)  
{
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

int initSensor(CPhidgetInterfaceKitHandle *ifKit ){
	int result;
	const char *err;
   	CPhidgetInterfaceKit_create(ifKit);
   	CPhidget_set_OnAttach_Handler((CPhidgetHandle)*ifKit, AttachHandler, NULL);
 	CPhidget_set_OnError_Handler((CPhidgetHandle)*ifKit, ErrorHandler, NULL);
	CPhidget_open((CPhidgetHandle)*ifKit, -1);

	printf("Waiting for interface kit to be attached....");
  	if((result = CPhidget_waitForAttachment((CPhidgetHandle)*ifKit, 5000))) {
         CPhidget_getErrorDescription(result, &err);
         printf("Problem waiting for attachment: %s\n", err);
         return 0; }
	return 1;	}

int initServo(CPhidgetServoHandle *servo){
	int result;
	const char *err;
	CPhidgetServo_create(servo);
    CPhidget_set_OnAttach_Handler((CPhidgetHandle)*servo, AttachHandler, NULL);
    CPhidget_set_OnDetach_Handler((CPhidgetHandle)*servo, DetachHandler, NULL);
    CPhidget_set_OnError_Handler((CPhidgetHandle)*servo, ErrorHandler, NULL);
    CPhidget_open((CPhidgetHandle)*servo, -1);
    printf("Waiting for Servo controller to be attached....");
    if((result = CPhidget_waitForAttachment((CPhidgetHandle)*servo, 5000))){
        CPhidget_getErrorDescription(result, &err);
        printf("Problem waiting for attachment: %s\n", err);
        return 0;	}
	return 1;	}

double getDistance(CPhidgetInterfaceKitHandle ifKit, double *diff){
	double currentPosition, radius, pi;
	int temporaryCount, sensorValue;
//	diff = 0;
	pi = acos(-1);
	radius = 4.2;

    CPhidgetInterfaceKit_getSensorValue(ifKit, 0, &sensorValue);
  	temporaryCount = sensorValue;
 	sleep(1);
    CPhidgetInterfaceKit_getSensorValue(ifKit, 0, &sensorValue);
    if ((sensorValue - temporaryCount) < (-800))
		*diff = (*diff + 1000 + sensorValue - temporaryCount); 
 	else if ((sensorValue - temporaryCount) > 800) 
 		*diff = (*diff - 1000 + sensorValue - temporaryCount); 
 	else
		*diff = (*diff + sensorValue - temporaryCount);   
 	currentPosition = 2*pi*radius**diff/1000;
 	return currentPosition; }

double getThrust(double currentPosition,double desiredPostition, double integral,  double dt){
	double ki, kp, error;	
	//her *integral
	kp = 1;
	ki = 1;
	error = desiredPostition - currentPosition;
	integral = integral + error*dt;
	return kp*error + ki*integral;	}

void giveThust(CPhidgetServoHandle servo, double thrust, double currentPosition)	{
	thrust = 100 + ((60/ *currentPosition)* *thrust);
	 CPhidgetServo_setPosition(servo, 0, thrust);}


// returns time since tstart was executed using the command :
// clock_gettime(CLOCK_REALTIME, &tstart);
// to use this function -lrt must be included in as gcc command
double getTime(struct timespec tstart){
	struct timespec	tend; 
	double timeDiff;
  	clock_gettime(CLOCK_REALTIME, &tend);
	timeDiff=(double)(tend.tv_sec + (double)tend.tv_nsec/1000000000  - (tstart.tv_sec + (double)tstart.tv_nsec/1000000000) )  ;
	return timeDiff;
}

int main(int argc, char* argv[])
  {
	int temporaryCount, result, sensorValue, errorFlag;
	double thrust, currentPosition, desiredPostition, dt;
	double integral=0;
	double diff = 0;
	struct timespec tstart; 
	
	CPhidgetServoHandle servo = 0;
	CPhidgetInterfaceKitHandle ifKit = 0;
//	printf("The boat is now %f from the right side", currentPosition);
	errorFlag = initSensor(&ifKit) + initServo(&servo);
	if( errorFlag < 2 ){
		return 0;
	}
	
	desiredPostition = 30; //her kommer en scanf etterhvert
  	clock_gettime(CLOCK_REALTIME, &tstart);
//	currentPosition = getDistance(ifKit);
	printf("The boat is now %f from the right side", currentPosition);
	while (1){
		dt = getTime(tstart);
  		clock_gettime(CLOCK_REALTIME, &tstart);
		currentPosition = getDistance(ifKit, &diff);
		thrust = getThrust(currentPosition, desiredPostition, integral, dt);	
		printf("The boat is now %f from the right side\n", currentPosition);
		printf("The thrust is now %f\n", thrust);
		giveThust(servo, &thrust, &currentPosition);
		usleep(100); // sleep 100 microseconds
	}
	

	//avslutter
	printf("Disengage. Press any key to Continue\n");
	getchar();

	CPhidgetServo_setEngaged (servo, 0, 0);

	printf("Press any key to end\n");
	getchar();


	printf("Closing...\n");
	CPhidget_close((CPhidgetHandle)servo);
	CPhidget_delete((CPhidgetHandle)servo);
	return 0;	}


