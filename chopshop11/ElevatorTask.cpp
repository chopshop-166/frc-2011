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
	
	enum {hNone=-1, hFloor=0, hLow=1, hMid=2, hHigh=3} target_type = hNone;
	
	// Fix these heights once we can test
	float target_heights[4] = {0,10,15,20};
	
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		if(proxy->get("Joy1B6")) {
			target_type = hHigh;
		} else if(proxy->get("Joy1B7")) {
			target_type = hMid;
		} else if(proxy->get("Joy1B8")) {
			target_type = hLow;
		} else if(proxy->get("Joy1B9")) {
			target_type = hFloor;
		} else {
			target_type = hNone;
		}
		if(target_type != hNone) {
			float target = target_heights[target_type];
			float current = proxy->get("ElevatorHeight");
			elevator.Set((target < current)? speed : ((target > current)? -speed : 0));
		} else {
			elevator.Set(proxy->get("Joy3Y"));
		}
		
        // Logging any values
		sl.PutOne(target_type);
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
