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
const double Autonomous_Arm_Up_Speed = 0.75;

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	float angle;
	int gripper;
	float brake;
#if ARM_USES_CAN
	int faults;
#endif
};

//  Memory Log
class ArmLog : public MemoryLog
{
public:
	ArmLog() : MemoryLog(
			sizeof(struct abuf), ARM_CYCLE_TIME, "arm",
#if ARM_USES_CAN
			"Elapsed Time,Arm Angle,Gripper,Brake,Faults\n"
#else
			"Elapsed Time,Arm Angle,Gripper,Brake\n"
#endif
			) {
		return;
	};
	~ArmLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
#if ARM_USES_CAN
	unsigned int PutOne(float, int, float, unsigned);     // Log the values needed-add in arguments
#else
	unsigned int PutOne(float, int, float);     // Log the values needed-add in arguments
#endif
};

// Write one buffer into memory
unsigned int ArmLog::PutOne(float a, int direction, float brake
#if ARM_USES_CAN
		, unsigned faults
#endif
		)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->angle = a;
		ob->gripper = direction;
		ob->brake = brake;
#if ARM_USES_CAN
		ob->faults = faults;
#endif
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
#if ARM_USES_CAN
	fprintf(ofile, "%4.5f,%1.5f,%d,%f,%d\n",
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->angle, ab->gripper, ab->brake, ab->faults
	);
#else
	fprintf(ofile, "%4.5f,%1.5f,%d,%f\n",
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->angle, ab->gripper, ab->brake
	);
#endif
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
ArmTask::ArmTask(void) :
#if ARM_USES_CAN
	armJag(ARM_JAGUAR),
#else
	armJag(ARM_JAGUAR_PWM),
#endif
	speed(0.25), deadband(0.15),
	gripper(GRIPPER_OPEN,GRIPPER_CLOSE), potentiometer(ARM_POT),
	high_limit(3.6), low_limit(1.58),
	ArmLock(ARM_LOCK_PWM)
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
	proxy->add("ArmOverride");
	float currentAngle;
	float axis;
	float previousAngles[ANGLE_LIST_SIZE];
	for(unsigned i=0;i<ANGLE_LIST_SIZE;i++) {
		previousAngles[i] = potentiometer.GetVoltage();
	}
	int angleSizeCounter = 0;
	int throttle = 0;
	bool grip = (gripper.Get() == DoubleSolenoid::kForward);
	proxy->TrackNewpress("Joy3B5");
	proxy->add("ArmReadyPosition");
	
    //General main loop (while in Autonomous or Tele mode)
	while (true) {
		angleSizeCounter = (angleSizeCounter + 1) % ANGLE_LIST_SIZE;
		previousAngles[angleSizeCounter] = potentiometer.GetVoltage();
		currentAngle=0;
		for(unsigned i=0;i<ANGLE_LIST_SIZE;i++) {
			currentAngle+=previousAngles[i];
		}
		currentAngle /= ANGLE_LIST_SIZE;
		if(proxy->get("ArmOverride")) {
			if(currentAngle >= 2.7) {
				axis = 0;
				proxy->set("ArmReadyPosition", true);
			} else {
				axis = Autonomous_Arm_Up_Speed;
				proxy->set("ArmReadyPosition", false);
			}
		} else {
			axis = -proxy->get(ELBOW_AXIS);
			proxy->set("ArmReadyPosition", false);
		}
		if(fabs(axis) < deadband) {
			axis=0;
		}
		if(axis>0) {
			axis -= deadband;
		} else if(axis<0) {
			axis += deadband;
		}
		
		axis /= 2;
		if (currentAngle >= MAXHEIGHT) {
			if (axis >=0) {
				axis = 0;
			}
		}
		
		if(axis != 0) {
			ArmLock.Set(DISENGAGED);
			throttle = 1;
		} else {
			ArmLock.Set(ENGAGED);
		}
		
		armJag.Set(axis);
		proxy->set("ArmAngle",currentAngle);
		if(proxy->get(GRIPPER_BUTTON)) {
			grip=!grip;
		}
		if(grip==1) {
			gripper.Set(DoubleSolenoid::kForward);
		} else {
			gripper.Set(DoubleSolenoid::kReverse);
		}
		SmartDashboard::Log(currentAngle,"Current Angle");
		
		if(lHandle->IsEnabled()) {
			// Logging any values
			sl.PutOne(currentAngle, grip, ArmLock.Get()
#if ARM_USES_CAN
					, armJag.GetFaults()
#endif
					);
        }
		
		// Wait for our next lap
		WaitForNextLoop();
	}
	return (0);
	
};
