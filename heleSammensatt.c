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

int initSensor(CPhidgetInterfaceKitHandle ifKit ){
	int result;
	const char *err;
   	CPhidgetInterfaceKit_create(&ifKit);
   	CPhidget_set_OnAttach_Handler((CPhidgetHandle)ifKit, AttachHandler, NULL);
 	CPhidget_set_OnError_Handler((CPhidgetHandle)ifKit, ErrorHandler, NULL);
	CPhidget_open((CPhidgetHandle)ifKit, -1);

	printf("Waiting for interface kit to be attached....");
  	if((result = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 10000))) {
         CPhidget_getErrorDescription(result, &err);
         printf("Problem waiting for attachment: %s\n", err);
         return 0; }
	return 1;	}

int initServo(CPhidgetServoHandle servo){
	int result;
	const char *err;
	CPhidgetServo_create(&servo);
    CPhidget_set_OnAttach_Handler((CPhidgetHandle)servo, AttachHandler, NULL);
    CPhidget_set_OnDetach_Handler((CPhidgetHandle)servo, DetachHandler, NULL);
    CPhidget_set_OnError_Handler((CPhidgetHandle)servo, ErrorHandler, NULL);
    CPhidget_open((CPhidgetHandle)servo, -1);
    printf("Waiting for Servo controller to be attached....");
    if((result = CPhidget_waitForAttachment((CPhidgetHandle)servo, 10000))){
        CPhidget_getErrorDescription(result, &err);
        printf("Problem waiting for attachment: %s\n", err);
        return 0;	}
	return 1;	}

double getDistance(CPhidgetInterfaceKitHandle ifKit){
	double currentPosition, radius, distance, pi;
	int temporaryCount, sensorValue, diff;
	diff = 0;
	pi = acos(-1);
	distance = 0;
	radius = 4.5;

    CPhidgetInterfaceKit_getSensorValue(ifKit, 0, &sensorValue);
  	temporaryCount = sensorValue;
 	sleep(1);
    CPhidgetInterfaceKit_getSensorValue(ifKit, 0, &sensorValue);
    if ((sensorValue - temporaryCount) < (-800))
		diff = (diff + 1000 + sensorValue - temporaryCount); 
 	else if ((sensorValue - temporaryCount) > 800) 
 		diff = (diff - 1000 + sensorValue - temporaryCount); 
 	else
		diff = (diff + sensorValue - temporaryCount);   
 	currentPosition = 2*pi*radius*diff/1000;
 	return currentPosition; }

double getThrust(double currentPosition,double desiredPostition);
	//int previousError = 0;
	double ki, kp, error, integral, dt;	
	//her *integral
	error = desiredPostition - currentPosition;
	integral = integral + error*dt;
	return kp*error + ki*(*integral);

int main(int argc, char* argv[])
  {
	int  temporaryCount, result, sensorValue, errorFlag;
	double a, b;
	double integral=0;
	
	CPhidgetServoHandle servo = 0;
	CPhidgetInterfaceKitHandle ifKit = 0;
	errorFlag = initSensor(ifKit) + initServo(servo);
	if( errorFlag < 2 ){
		return 0;
	}
	
	a = getDistance(ifKit);
	b = getThrust(currentPosition, desiredPostition);
/*		diff = diff + sensorValue - temporaryCount;
		distance = 2*pi*radius*diff/1000;
		printf("The boat is now %fcm from the right end.\n",distance);
		speed = speed + 10;
		CPhidgetServo_setPosition (servo, 0, speed);
		dt = clock() - start      }


	printf("Disengage. Press any key to Continue\n");
	getchar();

	CPhidgetServo_setEngaged (servo, 0, 0);

	printf("Press any key to end\n");
	getchar();


	printf("Closing...\n");
	CPhidget_close((CPhidgetHandle)servo);
	CPhidget_delete((CPhidgetHandle)servo);
*/	return 0;	}


