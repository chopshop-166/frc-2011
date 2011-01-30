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

#include "WPILib.h"
#include "ColorLights.h"

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
// <<CHANGEME>>
class ColorLightLog : public MemoryLog
{
public:
	ColorLightLog() : MemoryLog(
			sizeof(struct abuf166), COLORLIGHT_CYCLE_TIME, "template",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~ColorLightLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int ColorLightLog::PutOne(void)
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
unsigned int ColorLightLog::DumpBuffer(char *nptr, FILE *ofile)
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
//Above me is just logging


// task constructor
ColorLightTask::ColorLightTask(void):red(4,Relay::kForwardOnly), white(5,Relay::kForwardOnly), blue(6,Relay::kForwardOnly)
{
	Start((char *)"166ColorLightsTask", COLORLIGHT_CYCLE_TIME);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	return;
};
	
// task destructor
ColorLightTask::~ColorLightTask(void)
{
	return;
};
	
// Main function of the task
int ColorLightTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	ColorLightLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Template task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	// Register the proxy
	proxy = Proxy::getInstance();
	
	//Before don't generally touch^^
	//To run once write bellow me
    // General main loop (while in Autonomous or Tele mode)
	while (1) 
	{
		if(proxy->get("Joy3B4N")) //red
		{
			red.Set(Relay::kOn);
			white.Set(Relay::kOff);
			blue.Set(Relay::kOff);
		}
		if(proxy->get("Joy3B3N")) //white
		{
			white.Set(Relay::kOn);
			red.Set(Relay::kOff);
			blue.Set(Relay::kOff);
		}
		if(proxy->get("Joy3B5N")) //blue
		{
			blue.Set(Relay::kOn);
			red.Set(Relay::kOff);
			white.Set(Relay::kOff);
		}
		if(proxy->get("Joy3B2N")) //clear all
		{
			red.Set(Relay::kOff);
			white.Set(Relay::kOff);
			blue.Set(Relay::kOff);
		}
        // Logging any values
		// <<CHANGEME>>
		// Make this match the declaraction above
		sl.PutOne();//part of logging
		
		// Wait for our next lap
		WaitForNextLoop();//'donate' spare processing power
						  //When this task is done, stop accessing the CPU
	}
	return (0);
	
};
