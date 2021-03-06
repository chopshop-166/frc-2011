/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: MiniDeployTask.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to handle minibot deployment logic
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/


#include "WPILib.h"
#include "MiniDeployTask.h"
#include "Solenoid.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	float axis;
	int state;
};

//  Memory Log
class MiniDeployLog : public MemoryLog
{
public:
	MiniDeployLog() : MemoryLog(
			sizeof(struct abuf), MINIDEPLOY_CYCLE_TIME, "MiniDeploy",
			"Elapsed Time,Axis,State\n"
			) {
		return;
	};
	~MiniDeployLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(float, int);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int MiniDeployLog::PutOne(float a, int s)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->axis = a;
		ob->state = s;
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int MiniDeployLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%4.5f,%0.5f,%d\n",
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->axis, ab->state
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
MiniDeploy166::MiniDeploy166(void): MiniRelease(MINIBOT_ARM_RELEASE)
{
	Start((char *)"166MiniDeployTask", MINIDEPLOY_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
	Deploy_State = false;
	return;
};
	
// task destructor
MiniDeploy166::~MiniDeploy166(void)
{
	return;
};
	
// Main function of the task
int MiniDeploy166::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	// Register our logger
	MiniDeployLog sl;                   // log
		
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 MiniDeploy task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register main robot task
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	MiniRelease.Set(0);
	Solenoid(MINIBOT_ARM_LOCK).Set(1);
	Solenoid(MINIBOT_ARM_LOCK).Set(0);
	
    // General main loop (while in Autonomous or Tele mode) 
	while (true){
		Deploy_State = ((proxy->get("matchtimer") <= 10.0) && (proxy->get(DEPLOY_MINIBOT_COPILOT) <= -0.5));
		MiniRelease.Set(Deploy_State);
        // Logging any values
		if(lHandle->IsEnabled()) {
			sl.PutOne(proxy->get(DEPLOY_MINIBOT_COPILOT), Deploy_State);
		}
		
		// Wait for our next loop
		WaitForNextLoop();		
	}
	return (0);
	
};
