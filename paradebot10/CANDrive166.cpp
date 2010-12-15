/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: CANDrive166.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 23, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Code which monitors jaguar return info
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "CANDrive166.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf166
{
	struct timespec tp;               // Time of snapshot
	float l_current;              // left motor current value
	float r_current;		      // right motor current value	
};

//  Memory Log
class CANDriveLog : public MemoryLog166
{
public:
	CANDriveLog() : MemoryLog166(
			sizeof(struct abuf166), CAN_CYCLE_TIME, "candrive",
			"Seconds,Nanoseconds,Elapsed Time,Left Current,Right Current\n"
			) {
		return;
	};
	~CANDriveLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(float l_current, float r_current);     // Log the x and y values
};

// Write one buffer into memory
unsigned int CANDriveLog::PutOne(float l_current, float r_current)
{
	struct abuf166 *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf166 *)GetNextBuffer(sizeof(struct abuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->l_current = l_current;
		ob->r_current = r_current;
		return (sizeof(struct abuf166));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int CANDriveLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf166 *ab = (struct abuf166 *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%f,%f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->l_current, ab->r_current);
	
	// Done
	return (sizeof(struct abuf166));
}

// Initializes the ProxyHandle. 
Team166CANDrive *Team166CANDrive::CANDriveHandle = 0;

/**
 * @brief Gets the singleton instance of Proxy166.
 * @return The instance of Proxy166
 */
Team166CANDrive *Team166CANDrive::getInstance(void)
{
	return CANDriveHandle;
}


// task constructor
Team166CANDrive::Team166CANDrive(void):
	leftJag(T166_LEFT_MOTOR_CAN),
	rightJag(T166_RIGHT_MOTOR_CAN)
{
	CANDriveHandle=this;
	Start((char *)"166CANDriveTask", CAN_CYCLE_TIME);
	return;

};
	
// task destructor
Team166CANDrive::~Team166CANDrive(void)
{
	return;
};

float Team166CANDrive::Limit(float num)
{
	if (num > 1.0)
	{
		return 1.0;
	}
	if (num < -1.0)
	{
		return -1.0;
	}
	return num;
}

void Team166CANDrive::SquareInputs(float &leftValue,float &rightValue)
{
	// square the inputs (while preserving the sign) to increase fine control while permitting full power
	leftValue = Limit(leftValue);
	rightValue = Limit(rightValue);
	if (leftValue >= 0.0) {
		leftValue = (leftValue * leftValue);
	} else {
		leftValue = -(leftValue * leftValue);
	}
	if (rightValue >= 0.0) {
		rightValue = (rightValue * rightValue);
	} else {
		rightValue = -(rightValue * rightValue);
	}
}

// Main function of the task
int Team166CANDrive::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
		
	Robot *lHandle;                  // Local handle
	CANDriveLog sl;                    // log
	Proxy *proxy;				  //pointer to proxy	
	
	int valuethrottle=0;
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 CANDrive task\n");

	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();

	proxy = Proxy::getInstance();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	printf("CANDrive is ready.\n");
	float leftCurrent=0, rightCurrent=0;
	float leftMotorSpeed = 0;
	float rightMotorSpeed = 0;
	bool output_limit = 1;
	typedef enum {TANK_DRIVE = 1, ARCADE_DRIVE = 2} Drive_States;
	Drive_States Drive_State;
	Drive_States Prev_State;
	
    // General main loop (while in Autonomous or Tele mode)
	while ((lHandle->RobotMode == T166_AUTONOMOUS) || 
			(lHandle->RobotMode == T166_OPERATOR)) {
		if(proxy->get("Joy2T")>=0) {
			Drive_State = TANK_DRIVE;
			output_limit = 0;
		} else {
			Drive_State = ARCADE_DRIVE;
			output_limit = 0;
		}
		if (Drive_State != Prev_State) {
			lHandle->DriverStationDisplay("State Changed");
		}
		switch (Drive_State) {
			case 1:
				if (output_limit) {
					lHandle->DriverStationDisplay("In Tank Drive");
					output_limit = 0;
				}
				//Tank Drive
				leftMotorSpeed = proxy->get("Joy1Y");
				rightMotorSpeed = -proxy->get("Joy2Y");
				Prev_State = TANK_DRIVE;
				break;
			case 2:
				if (output_limit) {
					lHandle->DriverStationDisplay("In Arcade Drive");
					output_limit = 0;
				}
				//Arcade Drive
				if(proxy->get("Joy1B3")) {
					leftMotorSpeed = ARCADE_AUTO_SPEED;
					rightMotorSpeed = -ARCADE_AUTO_SPEED;
				} else if(proxy->get("Joy1B2")) {
					leftMotorSpeed = -ARCADE_AUTO_SPEED;
					rightMotorSpeed = ARCADE_AUTO_SPEED;
				} else {
					float moveValue = -proxy->get("Joy1X");
					float rotateValue = -proxy->get("Joy1Y");
			
					if (moveValue > 0.0) {
						if (rotateValue > 0.0) {
							leftMotorSpeed = moveValue - rotateValue;
							rightMotorSpeed = max(moveValue, rotateValue);
						} else {
							leftMotorSpeed = max(moveValue, -rotateValue);
							rightMotorSpeed = moveValue + rotateValue;
						}
					} else {
						if (rotateValue > 0.0) {
							leftMotorSpeed = - max(-moveValue, rotateValue);
							rightMotorSpeed = moveValue + rotateValue;
						} else {
							leftMotorSpeed = moveValue - rotateValue;
							rightMotorSpeed = - max(-moveValue, -rotateValue);
						}
					}
					//Make sure values aren't out of bounds
					SquareInputs(leftMotorSpeed, rightMotorSpeed);		
				}
				Prev_State = ARCADE_DRIVE;
				break;
		}
		//Set Speed of motor to correct value depending on drive mode
		leftJag.Set(leftMotorSpeed);
		rightJag.Set(rightMotorSpeed);
		
		//LOGGING STUFF
		if ((++valuethrottle) % (1000/CAN_CYCLE_TIME) ==0)
		{
			// Get Current from each jaguar 
			leftCurrent = leftJag.GetOutputCurrent();
			rightCurrent = rightJag.GetOutputCurrent();
			// Print debug to console
			DPRINTF(LOG_DEBUG, "Left Jag Current: %f", leftCurrent);
			DPRINTF(LOG_DEBUG, "Right Jag Current: %f", rightCurrent );
			//lHandle->DriverStationDisplay("DRV: %f %f", leftCurrent, rightCurrent);
		}
		
		// do stuff
		sl.PutOne(leftCurrent, rightCurrent);
		
		// Wait for our next lap
		WaitForNextLoop();
	}
	return (0);
	
};
