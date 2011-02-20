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
	// Any values that need to be logged go here
	// <<CHANGEME>>
};

//  Memory Log
// <<CHANGEME>>
class ArmLog : public MemoryLog
{
public:
	ArmLog() : MemoryLog(
			sizeof(struct abuf), ARM_CYCLE_TIME, "arm",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~ArmLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int ArmLog::PutOne(void)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		// Add any values to be logged here
		// <<CHANGEME>>
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
	fprintf(ofile, "%u,%u,%4.5f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.))
			// Add values here
			// <<CHANGEME>>
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
ArmTask::ArmTask(void) :
	armJag(ARM_JAGUAR, CANJaguar::kPosition), speed(0.25), deadband(0.1)
{
	Start((char *)"166ArmTask", ARM_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
	armJag.SetPositionReference(CANJaguar::kPosRef_Potentiometer);
	armJag.SetPID(PCOEFF,ICOEFF,DCOEFF);
	armJag.EnableControl();
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
	
	enum {hNone=-1, hFloor=0,
		hLowSide=1, hLowCenter=2,
		hMidSide=3, hMidCenter=4,
		hHighSide=5, hHighCenter=6} target_type = hNone;
	
	// Fix these heights once we can test
	// They currently don't take the arm height into account
	//
	const double angle_list[] = {0,30,37,67,74,104,111};
	proxy->add("ArmReadyPosition");
	float currentAngle;
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		if(proxy->get(FLOOR_PRESET_BUTTON)) {
			target_type = hFloor;
		} else {
			// This isn't set in Teleop...
			switch ((int)proxy->get("HeightLocation")) {
				default:
				case -1:
					target_type = hNone;
					break;
				case 0:
					target_type = hLowSide;
					break;
				case 1:
					target_type = hLowCenter;
					break;
				case 2:
					target_type = hMidSide;
					break;
				case 3:
					target_type = hMidCenter;
					break;
				case 4:
					target_type = hHighSide;
					break;
				case 5:
					target_type = hHighCenter;
					break;
				case 6:
					target_type = hFloor;
					break;
			}
		}
		
		if(fabs(proxy->get(ELBOW_AXIS)) >= 0.1) {
			target_type = hNone;
		}
		if((target_type != hNone && proxy->get(ARM_PRESET_BUTTON)) || proxy->get(FLOOR_PRESET_BUTTON)) {
			// Choose a "target" angle
			float target = angle_list[target_type];
			// Get the arm angle
			currentAngle = armJag.GetPosition();
			proxy->get("ElevatorHeight");
			// Set the speed to go in the proper direction
			//armJag.Set((target < currentAngle)? speed : ((target > currentAngle)? -speed : 0));
			armJag.Set(target);
			proxy->set("ArmReadyPosition", true);
		} else {
			float axis = proxy->get(ELBOW_AXIS);
			if(axis >= deadband || axis <= -deadband) {
				armJag.Set(proxy->get(ELBOW_AXIS));
			} else {
				armJag.Set(0);
			}
		}
		
		SmartDashboard::Log(currentAngle,"Potentiometer Value");
		
        // Logging any values
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();
	}
	return (0);
	
};
