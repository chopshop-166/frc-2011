/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: HeightTask.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task that gets the elevator height
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
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
	float height;
};

//  Memory Log
class HeightTaskLog : public MemoryLog
{
public:
	HeightTaskLog() : MemoryLog(
			sizeof(struct abuf), HEIGHTTASK_CYCLE_TIME, "Height Task",
			"Seconds,Nanoseconds,Elapsed Time, Height of Lift\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~HeightTaskLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(float);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int HeightTaskLog::PutOne(float LiftHeight)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->height = LiftHeight;
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
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->height
	);
	
	// Done
	return (sizeof(struct abuf));
}

// task constructor
HeightTask166::HeightTask166(void): Height(HEIGHT_INPUT)
{
	Start((char *)"166HeightTask", HEIGHTTASK_CYCLE_TIME);
	
	// Register the proxy
	proxy = Proxy::getInstance();
	
	
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
	HeightTaskLog sl;           // log
	
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Height task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	// add a proxy variable
	proxy->add("ElevatorHeight");
	
	// Set variable values
	HowHigh = 0;
		
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		
		// set howhigh to the height of the elevator
		HowHigh = InchesPerVolt*Height.GetVoltage();
		// tell proxy how high we are
		proxy->set("ElevatorHeight",HowHigh);
		SmartDashboard::Log(HowHigh, "Height");
		sl.PutOne(HowHigh);
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
