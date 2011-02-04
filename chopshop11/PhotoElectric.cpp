/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: PhotoElectric.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to get photoelectric sensor values
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "PhotoElectric.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf166
{
	struct timespec tp;               // Time of snapshot
	int left_result;
	int center_result;
	int right_result;
	LineStateType result_result;
};

//  Memory Log
class PhotoElectricLog : public MemoryLog
{
public:
	PhotoElectricLog() : MemoryLog(
			sizeof(struct abuf166), PHOTOELECTRIC_CYCLE_TIME, "photoelectric",
			"Seconds,Nanoseconds,Elapsed Time,Left Result,Right Result,Center Result,Result\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~PhotoElectricLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(int ,int ,int ,LineStateType);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int PhotoElectricLog::PutOne(int left,int right,int center,LineStateType result)
{
	struct abuf166 *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf166 *)GetNextBuffer(sizeof(struct abuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->left_result= left;
		ob->right_result= right;
		ob->center_result= center;
		ob->result_result=result;
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
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)), 
			ab->left_result,
			ab->right_result,
			ab->center_result,
			ab->result_result
	);
	
	// Done
	return (sizeof(struct abuf166));
}


// task constructor
PhotoElectricTask::PhotoElectricTask(void):left(LEFTPHOTOSENSE),center(CENTERPHOTOSENSE),right(RIGHTPHOTOSENSE)
{
	Start((char *)"166PhotoElectricTask", PHOTOELECTRIC_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
	LineState = No_Line;
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
	// Register our logger
	PhotoElectricLog sl;                   // log
	
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Photoelectric task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();	
	
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	// Set up the proxy value
	proxy->add("LineDirection");
		bool left_result;
		bool center_result;
		bool right_result;
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		// Use .Get to get the value of the sensor
		left_result = !left.Get();
	    center_result = !center.Get();
	    right_result = !right.Get();
		/* 
		 * 2 means it hit a T or fork
		 * 1 means to the right
		 * 0 means dead on
		 * -1 means to the left
		 * -2 means it's not on the line at all
		*/
		// Figure out if 1 is "on the line" or "off the line"
		if(left_result&&center_result&&right_result) {
			LineState=T;
		} else if(left_result&&right_result) {
			LineState=Fork;
		} else if(left_result) {
			LineState=Left;
		} else if(right_result) {
			LineState=Right;
		} else if(center_result) {
			LineState=Center;
		} else {
			LineState=No_Line;
		} 
		// Figure out whether the robot is to the left of a line, to the right of a line, on the line, or off the line
		// Store that result in proxy
		proxy->set("LineDirection",(float) LineState);
		SmartDashboard::Log((int) LineState, "Line Result");
		
        // Logging any values
		// <<CHANGEME>>
		// Make this match the declaration above
		sl.PutOne(left_result, center_result, right_result, LineState);
		
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
