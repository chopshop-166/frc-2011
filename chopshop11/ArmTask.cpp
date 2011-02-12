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
//	armJag(ARM_JAGUAR, CANJaguar::kPosition),
	armChan(4), speed(0.25)
{
	Start((char *)"166ArmTask", ARM_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
//	armJag.SetPositionReference(CANJaguar::kPosRef_Potentiometer);
//	armJag.EnableControl();
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
	/* Since the pot has a range of 0-270 degrees and 0-5 volts,
	 * We know that the degrees per volt has to be 270/5, or 54
	 */ 
	const double degrees_per_volt = 54.;
	
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		if(proxy->get(TOP_CENTER_PRESET_BUTTON)) {
			target_type = hHighCenter;
		} else if(proxy->get(MIDDLE_CENTER_PRESET_BUTTON)) {
			target_type = hMidCenter;
		} else if(proxy->get(LOW_CENTER_PRESET_BUTTON)) {
			target_type = hLowCenter;
		} else if(proxy->get(TOP_SIDE_PRESET_BUTTON)) {
			target_type = hHighSide;
		} else if(proxy->get(MIDDLE_SIDE_PRESET_BUTTON)) {
			target_type = hMidSide;
		} else if(proxy->get(LOW_SIDE_PRESET_BUTTON)) {
			target_type = hLowSide;
		} else if(proxy->get(FLOOR_PRESET_BUTTON)) {
			target_type = hFloor;
		} else if(fabs(proxy->get(ELEVATOR_AXIS)) >= 0.1) {
			target_type = hNone;
		}
		if(target_type != hNone) {
			// Choose a "target" angle
			float target = angle_list[target_type];
			// Get the arm angle
			float current =
				//armCan.GetPosition();
				proxy->get("ElevatorHeight");
			// Just get rid of annoying compiler warnings
			(void)target,(void)current;
			// Set the speed to go in the proper direction
//			armJag.Set((target < current)? speed : ((target > current)? -speed : 0));
		} else {
//			armJag.Set(proxy->get(ELEVATOR_AXIS));
		}
		
		double volts = armChan.GetVoltage();
		SmartDashboard::Log(degrees_per_volt*volts,"Potentiometer Value");
		SmartDashboard::Log(volts,"Potentiometer Voltage");
		
        // Logging any values
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
