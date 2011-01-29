/*******************************************************************************
*  Project   		: Framework
*  File Name  		: HeightTask.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 23, 2011
*  File Description	: Template source file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "HeightTask.h"
#include "AnalogChannel.h"
#include "AnalogModule.h"

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
class HeightTaskLog : public MemoryLog
{
public:
	HeightTaskLog() : MemoryLog(
			sizeof(struct abuf), HEIGHTTASK_CYCLE_TIME, "Height Task",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~HeightTaskLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int HeightTaskLog::PutOne(void)
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
unsigned int HeightTaskLog::DumpBuffer(char *nptr, FILE *ofile)
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
HeightTask166::HeightTask166(void): Height(2)
{
	Start((char *)"166HeightTask", HEIGHTTASK_CYCLE_TIME);
	return;
};
	
// task destructor
HeightTask166::~HeightTask166(void)
{
	return;
};
	
// Main function of the task
int HeightTask166::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;             // Local handle
	HeightTaskLog sl;           // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Height task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	// Register the proxy
	proxy = Proxy::getInstance();
	
	// add a proxy variable
	proxy->add("ElevatorHeight");
	
	// Set variable values
	HowHigh = 0;
	InchesPerVolt = 0.02;  // This value is given in Spec sheet for LX-PA-50
		
    // General main loop (while in Autonomous or Tele mode)
	while (1) {
		
		// set howhigh to the height of the elevator
		HowHigh = InchesPerVolt*Height.GetVoltage();
		// tell proxy how high we are
		proxy->set("ElevatorHeight",HowHigh);
		
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
