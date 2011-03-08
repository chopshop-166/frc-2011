/*******************************************************************************
*  Project   		: Framework
*  File Name  		: TaskTemplate.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Template source file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/* Find & Replace "Template" with the name you would like to give this task     */
/* Find & Replace "Testing" with the name you would like to give this task      */
/* Find & Replace "TaskTemplate" with the name you would like to give this task */
/*------------------------------------------------------------------------------*/

#include "WPILib.h"
#include "ArmTask.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	float angle;
	int gripper;
	int faults;
};

//  Memory Log
class ArmLog : public MemoryLog
{
public:
	ArmLog() : MemoryLog(
			sizeof(struct abuf), ARM_CYCLE_TIME, "arm",
			"Elapsed Time,Arm Angle,Gripper,Faults\n"
			) {
		return;
	};
	~ArmLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(float, int, unsigned);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int ArmLog::PutOne(float a, int direction, unsigned faults)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->angle = a;
		ob->gripper = direction;
		ob->faults = faults;
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int ArmLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%4.5f,%1.5f,%d,%d\n",
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->angle, ab->gripper, ab->faults
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
ArmTask::ArmTask(void) :
	armJag(ARM_JAGUAR), speed(0.25), deadband(0.1),
	gripper(GRIPPER_OPEN,GRIPPER_CLOSE), potentiometer(ARM_POT),
	high_limit(3.6), low_limit(1.58)
{
	Start((char *)"166ArmTask", ARM_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
	return;
};
	
// task destructor
ArmTask::~ArmTask(void)
{
	return;
};
	
// Main function of the task
int ArmTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	ArmLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Arm task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	// lHandle = Robot::getInstance() MUST go after this, otherwise code breaks
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	proxy->add("ArmAngle");
	float currentAngle;
	float axis;
	float previousAngles[ANGLE_LIST_SIZE];
	for(unsigned i=0;i<ANGLE_LIST_SIZE;i++) {
		previousAngles[i] = potentiometer.GetVoltage();
	}
	int angleSizeCounter = 0;
	bool grip = false;
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		angleSizeCounter = (angleSizeCounter + 1) % ANGLE_LIST_SIZE;
		previousAngles[angleSizeCounter] = potentiometer.GetVoltage();
		currentAngle=0;
		for(unsigned i=0;i<ANGLE_LIST_SIZE;i++) {
			currentAngle+=previousAngles[i];
		}
		currentAngle /= ANGLE_LIST_SIZE;
		
		axis = -proxy->get(ELBOW_AXIS);
		if(fabs(axis) < deadband) {
			axis=0;
		}
		if(axis>0) {
			axis -= deadband;
		} else if(axis<0) {
			axis += deadband;
		}
		
		axis /= 3;
		if (currentAngle >= MAXHEIGHT) {
			if (axis >=0) {
				axis = 0;
			}
		}
		
		armJag.Set(axis);
		proxy->set("ArmAngle",currentAngle);
		
		if(proxy->get(GRIPPER_BUTTON)) {
			printf("Switched to: %s\n", (!grip)?"open":"closed");
			if(grip = !grip) {
				gripper.Set(DoubleSolenoid::kForward);
			} else {
				gripper.Set(DoubleSolenoid::kReverse);
			}
		}
		
		SmartDashboard::Log(currentAngle,"Current Angle");
//		SmartDashboard::Log(axis,"Elbow Axis");
		
        // Logging any values
		sl.PutOne(currentAngle, grip, armJag.GetFaults());
		
		// Wait for our next lap
		WaitForNextLoop();
	}
	return (0);
	
};
