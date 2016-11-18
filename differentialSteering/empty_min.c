
#define LEFT 0
#define RIGHT 1
/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

#include <math.h>
/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>
#include <stdint.h>
#include <stdbool.h>
/* Example/Board Header files */
#include "Board.h"
#define TASKSTACKSIZE   512
#define oneClickL		0.04887
#define oneClickR		0.04806
#define pi				3.141592
#define wheelDiameter		11.37
#define wheelRadius			5.685
Int resource = 0;
Int finishCount = 0;
UInt32 sleepTickCount;

PWM_Handle pwmL, pwmR;
PWM_Params params;
uint_fast16_t pwmPeriod;
uint_fast16_t duty;
Task_Struct task1Struct, task2Struct, task3Struct;
Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE], task3Stack[TASKSTACKSIZE];
Semaphore_Struct semStruct;
Semaphore_Handle semHandleSS, semHandleDS, semHandleTP;
volatile int enc_count[2];
int motor_direction[2];
int movingLeftCount[3];
int movingRightCount[3];
float velocityLeft[5];
float velocityRight[5];
int waypointsX[6];
int waypointsY[6];
float waypointsH[6];
volatile int count;
volatile int countTP;
volatile int countWayPoints;
Void differentialSteering(UArg arg0, UArg arg1);
Void sensorSuite(UArg arg0, UArg arg1);
Void trajectoryPlanner(UArg arg0, UArg arg1);
Void initializePWM();
Void initializeEncoder();
int convertToPWM(int desiredIntensity);
int read_enc_count(int enc);

int testCount;
int KpL;
int KpR;
int Ki;
int Ka;
int Kphi;
volatile float e3;
float desiredX;
float desiredY;
volatile int flagDS;
volatile int error_l;
volatile int error_r;
volatile int error_t;
volatile float currentHeading;
volatile float prevHeading;
volatile float desiredHeading;
volatile float desiredVel;
volatile int currentVelL;
volatile int currentVelR;
volatile int currentVelL1;
volatile int currentVelR1;
volatile int currentVelocityL;
volatile int currentVelocityR;
volatile float currentX;
volatile float currentY;
volatile double prevX;
volatile double prevY;
volatile int nextPWMDutyL;
volatile int nextPWMDutyR;
volatile int nextVelL;
volatile int nextVelR;
volatile float bias;
volatile int countL;
volatile int countR;
volatile int countPos;
volatile float global_errorX;
volatile float global_errorY;
volatile float global_errorHeading;
volatile double commFac;
volatile double commFacX;
volatile double commFacY;
volatile double alpha;
volatile int time[500];
volatile float desVel[500];
volatile float desHead[500];
volatile float curComL[500];
volatile float curComR[500];


int read_enc_count(int enc) {
	return enc_count[enc];
}


Void trajectoryPlanner(UArg arg0, UArg arg1)
{
	while(1) {
		if(Semaphore_pend(semHandleTP, BIOS_WAIT_FOREVER)) {

			//Calculate global error position X & Y and heading
			global_errorX = desiredX - currentX;
			global_errorY = desiredY - currentY;
			global_errorHeading = desiredHeading - currentHeading;

			//Update and transition to next waypoints in certain condition
			if ((abs(global_errorX) < 0.5) && (abs(global_errorY) < 0.5)) {
				if (countWayPoints < 6) {
					desiredX = waypointsX[countWayPoints];
					desiredY = waypointsY[countWayPoints];
					desiredHeading = waypointsH[countWayPoints];
					countWayPoints = countWayPoints + 1;
				}
			}

			//If updated, new global errors will be calculated
			global_errorX = desiredX - currentX;
			global_errorY = desiredY - currentY;
			global_errorHeading = desiredHeading - currentHeading;

			//Calculate desired velocity
			desiredVel = sqrt(pow(global_errorX,2) + pow(global_errorY,2));


			//Keep the global heading error in the range specified
			if (global_errorHeading > (pi/2)) {
				global_errorHeading = global_errorHeading - pi;
			}

			//Calculate alpha term
			alpha = atan(global_errorX / global_errorY) - currentHeading;
			alpha = roundf(alpha * 100) / 100;

			//Calculate bias term
			bias = alpha * Ka + Kphi * global_errorHeading;
			bias = (float) (roundf(bias * 100) / 100);

			e3 = 0;
		}
	}
}
Void sensorSuite(UArg arg0, UArg arg1)
{
	while (1) {
		if(Semaphore_pend(semHandleSS, BIOS_WAIT_FOREVER)) {

			//Take encoder counts
			countL = read_enc_count(LEFT);
			countR = read_enc_count(RIGHT);

			//Initialize encoder array to 0
			enc_count[LEFT] = 0;
			enc_count[RIGHT] = 0;

			//Moving average filter
			if (count == 3) {
				count = 0;
			}
			movingLeftCount[count] = countL;
			movingRightCount[count] = countR;
			count = count + 1;
			currentVelL1 = (movingLeftCount[0] + movingLeftCount[1] + movingLeftCount[2]) * oneClickL / 0.15;
			currentVelR1 = (movingRightCount[0] + movingRightCount[1] + movingRightCount[2]) * oneClickR / 0.15;

			//Mean filter for current heading, position X & Y
			velocityLeft[countPos] = currentVelL1;
			velocityRight[countPos] = currentVelR1;
			countPos = countPos + 1;

			//Every 5 times, calculate and update current heading and position X & Y
			if (countPos == 5) {

				//Average for 5 times
				currentVelocityL = (velocityLeft[0] + velocityLeft[1] + velocityLeft[2] + velocityLeft[3] + velocityLeft[4]) / 5;
				currentVelocityR = (velocityRight[0] + velocityRight[1] + velocityRight[2] + velocityRight[3] + velocityRight[4]) / 5;

				//Calculate Heading
				currentHeading = prevHeading + ((currentVelocityR - currentVelocityL)*0.05 / wheelDiameter);

				//Keep the heading in the range specified
				if (currentHeading > (pi)) {
					currentHeading = currentHeading - 2 * pi;
				} else if (currentHeading < (-1 * pi)) {
					currentHeading = 2* pi + currentHeading;
				}

				//Update current position X & Y
				currentX = prevX + ((currentVelocityR + currentVelocityL)* 0.25/2) * cos(currentHeading);
				currentY = prevY + ((currentVelocityR + currentVelocityL)* 0.25/2) * sin(currentHeading);

				/* second Method
				 * commFac = (wheelRadius* (currentVelocityR + currentVelocityL) / (currentVelocityR - currentVelocityL));
				 * commFacX = commFac * (sin((currentVelocityR - currentVelocityL)*0.25 / wheelDiameter + prevHeading) - sin(prevHeading));
				 * commFacY = commFac * (cos((currentVelocityR - currentVelocityL)*0.25 / wheelDiameter + prevHeading) - cos(prevHeading));
				 * currentY = prevY - commFacY;
				 * currentX = prevX + commFacX;
				 */

				//For next calculation
				prevX = currentX;
				prevY = currentY;
				prevHeading = currentHeading;
				countPos = 0;
			}
		}
	}
}
/*
 *  ======== differentialSteering ========
 */

Void differentialSteering(UArg arg0, UArg arg1)
{

	while (1) {
		if(Semaphore_pend(semHandleDS, BIOS_WAIT_FOREVER)) {

			//Retrieve current velocity for each whell
			currentVelL = currentVelL1;
			currentVelR = currentVelR1;

			//e3 term
			e3 = e3 + currentVelL + bias - currentVelR;
			e3 = roundf(e3 * 100 * Ki) / 100;

			//Left Part
			error_l = desiredVel - currentVelL1  - e3;
			nextVelL = error_l;


			//Right Part
			error_r = desiredVel - currentVelR1 + e3;
			nextVelR = error_r;


			if (desiredVel == 0) {
				nextVelL = 0;
				nextVelR = 0;
			}

			//Convert speed to PWM signal
			nextPWMDutyL = convertToPWM(nextVelL);
			nextPWMDutyR = convertToPWM(nextVelR);
		}


		PWM_setDuty(pwmL, nextPWMDutyL);
		PWM_setDuty(pwmR, nextPWMDutyR);
	}
}

int convertToPWM(int desiredIntensity)
{

	//Saturate the speed to still run with pwm signal lower than 4000 (4000 is the threshold)
	int dutyVal = (16000 * desiredIntensity / 25) + 4000;

	//When the robot needs to stop
	if (desiredIntensity == 0) {
		dutyVal = 0;
	}

	//Saturation for maximum speed and minimum speed
	if (dutyVal > 20000) {
		dutyVal = 20000;
	} else if (dutyVal < 0) {
		dutyVal = 0;
	}

	return dutyVal;
}


void l_enc_handler1(unsigned int index)
{
	enc_count[LEFT] = enc_count[LEFT] + 1;
}



void l_enc_handler2(unsigned int index)
{
	enc_count[LEFT] = enc_count[LEFT] + 1;
}



void r_enc_handler1(unsigned int index)
{
	enc_count[RIGHT] = enc_count[RIGHT] + 1;
}



void r_enc_handler2(unsigned int index)
{
	enc_count[RIGHT] = enc_count[RIGHT] + 1;
}



Void clockHandlerSS(UArg arg)
{
	Semaphore_post(semHandleSS);
}



Void clockHandlerDS(UArg arg)
{
	Semaphore_post(semHandleDS);
}



Void clockHanlderTP(UArg arg)
{
	Semaphore_post(semHandleTP);
}



Void initializePWM()
{

	//Construct and configure PWM
	pwmPeriod = 32000;
	duty = 0;
	PWM_Params_init(&params);
	params.dutyUnits = PWM_DUTY_US;
	params.dutyValue = 0;
	params.periodUnits = PWM_PERIOD_US;
	params.periodValue = pwmPeriod;

	//Construct PWM handlers for left motor
	pwmL = PWM_open(Board_L_MOTOR, &params);
	if(pwmL == NULL) {
		System_abort("Motor L did not open");
	}

	//Start left PWM handler
	PWM_start(pwmL);

	//Construct PWM handlers for right motor
	pwmR = PWM_open(Board_R_MOTOR, &params);
	if(pwmR == NULL) {
		System_abort("Motor R did not open");
	}

	//Start right PWM handler
	PWM_start(pwmR);


}



Void initializeWayPoints()
{
	waypointsX[0] = 0;
	waypointsX[1] = 20;
	waypointsX[2] = 40;
	waypointsX[3] = 60;
	waypointsX[4] = 80;
	waypointsX[5] = 100;

	waypointsY[0] = 0;
	waypointsY[1] = 4;
	waypointsY[2] = 13;
	waypointsY[3] = 25;
	waypointsY[4] = 45;
	waypointsY[5] = 100;

	waypointsH[0] = 0;
	waypointsH[1] = 0.2027;
	waypointsH[2] = 0.3369;
	waypointsH[3] = 0.4426;
	waypointsH[4] = 0.6304;
	waypointsH[5] = 1.5574;


 /* Another type of waypoints
	waypointsY[0] = 0;
	waypointsY[1] = 0;
	waypointsY[2] = 2;
	waypointsY[3] = 5;
	waypointsY[4] = 8;
	waypointsY[5] = 13;
	waypointsY[6] = 18;
	waypointsY[7] = 25;
	waypointsY[8] = 32;
	waypointsY[9] = 40;
	waypointsY[10] = 100;

	waypointsH[0] = 0;
	waypointsH[1] = 0;
	waypointsH[2] = 0.1003;
	waypointsH[3] = 0.1682;
	waypointsH[4] = 0.2027;
	waypointsH[5] = 0.2660;
	waypointsH[6] = 0.3093;
	waypointsH[7] = 0.3731;
	waypointsH[8] = 0.4228;
	waypointsH[9] = 0.4762;
	waypointsH[10] = 1.5574; */
}



Void initializeDS()
{
	count = 0;
	countTP = 0;
	currentHeading = 0;
	prevHeading = 0;
	desiredX = waypointsX[0];
	desiredY = waypointsY[0];
	desiredHeading = waypointsH[0];
	KpL = 1;
	KpR = 4;
	Ki = 4;
	Ka = 2;
	Kphi = 1;
	error_l = 0;
	error_r = 0;
	error_t = 0;
	desiredVel = 0;
	currentVelL = 0;
	currentVelR = 0;
	currentX = 0;
	currentY = 0;
	prevX = 0;
	prevY = 0;
	nextVelL = 0;
	nextVelR = 0;
	bias = 0;
	global_errorX = desiredX;
	global_errorX = desiredY;
	global_errorHeading = desiredHeading;
	countPos = 0;
	countWayPoints = 1;
	e3 = 0;
	testCount = 0;
}



Void initializeEncoder()
{
	//Initialize Encoder Counts
	enc_count[LEFT] = 0;
	enc_count[RIGHT] = 0;

	//Clear Interrupts
	GPIO_clearInt(Board_ENC1L);
	GPIO_clearInt(Board_ENC2L);
	GPIO_clearInt(Board_ENC1R);
	GPIO_clearInt(Board_ENC2R);


	//Set Callback Functions for left motor
	GPIO_setCallback(Board_ENC1L, l_enc_handler1);
	GPIO_enableInt(Board_ENC1L);
	GPIO_setCallback(Board_ENC2L, l_enc_handler2);
	GPIO_enableInt(Board_ENC2L);


	//Set Callback functions for right motor
	GPIO_setCallback(Board_ENC1R, r_enc_handler1);
	GPIO_enableInt(Board_ENC1R);
	GPIO_setCallback(Board_ENC2R, r_enc_handler2);
	GPIO_enableInt(Board_ENC2R);

	//Success print statment
	System_printf("initialized done\n");
	System_flush();
}



void initializeTasks()
{
    Task_Params taskParams;
	Task_Params_init(&taskParams);

	//Construct sensor suite task
	taskParams.stackSize = TASKSTACKSIZE;
	taskParams.stack = &task1Stack;
	taskParams.priority = 1;
	Task_construct(&task1Struct, (Task_FuncPtr)sensorSuite, &taskParams, NULL);

	//Construct differential steering task
	taskParams.stack = &task2Stack;
	taskParams.priority = 1;
	Task_construct(&task2Struct, (Task_FuncPtr)differentialSteering, &taskParams, NULL);

	//Construct trajectory planner task
	taskParams.stack = &task3Stack;
	taskParams.priority = 1;
	Task_construct(&task3Struct, (Task_FuncPtr)trajectoryPlanner, &taskParams, NULL);
}



void initializeClocks()
{
    Clock_Params clockParams;
    Clock_Handle clockSS, clockDS, clockTP;
    Error_Block eb;
    Clock_Params_init(&clockParams);

    //Construct sensor suite clock timer & handler
    clockParams.period = 100;
    clockParams.startFlag = TRUE;
    clockSS = Clock_create((Clock_FuncPtr) clockHandlerSS, 100, &clockParams, &eb);
    if (clockSS == NULL) {
    	System_abort("Clock0 create failed\n");
    }

    //Construct differential steering clock timer & handler
    clockParams.period = 50;
    clockParams.startFlag = TRUE;
    clockDS = Clock_create((Clock_FuncPtr) clockHandlerDS, 50, &clockParams, &eb);
    if (clockDS == NULL) {
    	System_abort("Clock1 create failed\n");
    }

    //Construct trajectory planner clock timer & handler
    clockParams.period = 1000;
    clockParams.startFlag = TRUE;
    clockTP = Clock_create((Clock_FuncPtr) clockHanlderTP, 1000, &clockParams, &eb);
    if (clockTP == NULL) {
        System_abort("Clock1 create failed\n");
    }
}

void initializeSemaphores()
{
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&semStruct, 0, &semParams);


    //Binary semaphore (0 or 1 only)
    semParams.mode = Semaphore_Mode_BINARY;


    //Construct semaphores for all tasks
    semHandleSS = Semaphore_handle(&semStruct);
    semHandleDS = Semaphore_handle(&semStruct);
    semHandleTP = Semaphore_handle(&semStruct);
}



int main(void)
{

	//BIOS objects initialization
    Board_initGeneral();
    Board_initGPIO();
    Board_initPWM();

    //Initialize waypoints
    initializeWayPoints();

    //Initialize Tasks
    initializeTasks();

    //Initialize Clock timers & handlers
    initializeClocks();

    //Initialize Semaphores
    initializeSemaphores();

    //Initialize PWM configurations
    initializePWM();

    //Initialize Encoders and interrupt handlers
    initializeEncoder();

    //Initialize all variables for differential steering & trajectory planner
    initializeDS();


    //Start the BIOS
    BIOS_start();    /* Does not return */

    return(0);
}




