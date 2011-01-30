/*******************************************************************************
*  Project   		: Framework
*  File Name  		: PhotoElectric.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to get photoelectric sensor values
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "PhotoElectric.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf166
{
	struct timespec tp;               // Time of snapshot
	// Any values that need to be logged go here
	// <<CHANGEME>>
};

//  Memory Log
class PhotoElectricLog : public MemoryLog
{
public:
	PhotoElectricLog() : MemoryLog(
			sizeof(struct abuf166), PHOTOELECTRIC_CYCLE_TIME, "photoelectric",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~PhotoElectricLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int PhotoElectricLog::PutOne(void)
{
	struct abuf166 *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf166 *)GetNextBuffer(sizeof(struct abuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		// Add any values to be logged here
		// <<CHANGEME>>
		return (sizeof(struct abuf166));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int PhotoElectricLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf166 *ab = (struct abuf166 *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.))
			// Add values here
			// <<CHANGEME>>
	);
	
	// Done
	return (sizeof(struct abuf166));
}


// task constructor
PhotoElectricTask::PhotoElectricTask(void):left(LEFTPHOTOSENSE),center(CENTERPHOTOSENSE),right(RIGHTPHOTOSENSE)
{
	Start((char *)"166PhotoElectricTask", PHOTOELECTRIC_CYCLE_TIME);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	return;
};
	
// task destructor
PhotoElectricTask::~PhotoElectricTask(void)
{
	return;
};
	
// Main function of the task
int PhotoElectricTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	PhotoElectricLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Photoelectric task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	// Register the proxy
	proxy = Proxy::getInstance();
	
	// Set up the proxy value
	proxy->add("LineDirection");
		
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		// Use .Get to get the value of the sensor
		bool l = !left.Get();
		bool c = !center.Get();
		bool r = !right.Get();
		int result=0;
		/* 
		 * 2 means it hit a T or fork
		 * 1 means to the right
		 * 0 means dead on
		 * -1 means to the left
		 * -2 means it's not on the line at all
		*/
		// Figure out if 1 is "on the line" or "off the line"
		if(l&&r) {
			result=2;
		} else if(l) {
			result=1;
		} else if(r) {
			result=-1;
		} else if(c) {
			result=0;
		} else {
			result=-2;
		} 
		// Figure out whether the robot is to the left of a line, to the right of a line, on the line, or off the line
		// Store that result in proxy
		proxy->set("LineDirection",result);
		SmartDashboard::Log(result, "Line Result");
		
        // Logging any values
		// <<CHANGEME>>
		// Make this match the declaration above
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
