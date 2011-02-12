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
#include "GripperTask.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	int state;						// Open vs closed
};

//  Memory Log
// <<CHANGEME>>
class GripperLog : public MemoryLog
{
public:
	GripperLog() : MemoryLog(
			sizeof(struct abuf), TEMPLATE_CYCLE_TIME, "gripper",
			"Seconds,Nanoseconds,Elapsed Time,Gripper State\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~GripperLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(int);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int GripperLog::PutOne(int state)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->state = state;
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int GripperLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%d\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.))
			,ab->state
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
GripperTask::GripperTask(void):gripper(GRIPPER_OPEN,GRIPPER_CLOSE)
{
	Start((char *)"166GripperTask", TEMPLATE_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	return;
};
	
// task destructor
GripperTask::~GripperTask(void)
{
	return;
};
	
// Main function of the task
int GripperTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	GripperLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Gripper task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	// lHandle = Robot::getInstance() MUST go after this, otherwise code breaks
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
		
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		gripper.Set(proxy->get(GRIPPER_BUTTON));
		
		// Make this match the declaraction above
		sl.PutOne(gripper.Get());
		
		// Wait for our next lap
		WaitForNextLoop();
	}
	return (0);
	
};
