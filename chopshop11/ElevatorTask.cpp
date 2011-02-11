/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: ElevatorTask.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to run the elevator
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "ElevatorTask.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	int target_type;
	
};

//  Memory Log

class ElevatorLog : public MemoryLog
{
public:
	ElevatorLog() : MemoryLog(
			sizeof(struct abuf), ELEVATOR_CYCLE_TIME, "Elevator",
			"Seconds,Nanoseconds,Elapsed Time,Target Height\n"
			) {
		return;
	};
	~ElevatorLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	
	unsigned int PutOne(int);     // Log the values needed-add in arguments
};

// Write one buffer into memory

unsigned int ElevatorLog::PutOne(int height)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->target_type = height;
		
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int ElevatorLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f, %d\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->target_type
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
ElevatorTask::ElevatorTask(void): elevator(11), speed(0.25)
{
	Start((char *)"166ElevatorTask", ELEVATOR_CYCLE_TIME);	
	// Register the proxy
	proxy = Proxy::getInstance();
	
	return;
};
	
// task destructor
ElevatorTask::~ElevatorTask(void)
{
	return;
};
	
// Main function of the task
int ElevatorTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	//Create then register logger
	ElevatorLog sl;                   // log
	
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Elevator task\n");

	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register main robot
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	enum {hNone=-1, hFloor=0,
		hLowSide=1, hLowCenter=2,
		hMidSide=3, hMidCenter=4,
		hHighSide=5, hHighCenter=6} target_type = hNone;
	
	// Fix these heights once we can test
	// They currently don't take the arm height into account
	const double height_list[] = {0,30,37,67,74,104,111};
	
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
		} else {
			target_type = hNone;
		}
		if(target_type != hNone && fabs(proxy->get("ElevatorHeight")) < 0.1) {
			float target = height_list[target_type];
			float current = proxy->get("ElevatorHeight");
			elevator.Set((target < current)? speed : ((target > current)? -speed : 0));
		} else {
			elevator.Set(proxy->get(ELEVATOR_AXIS));
		}
		
        // Logging any values
		sl.PutOne(target_type);
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
