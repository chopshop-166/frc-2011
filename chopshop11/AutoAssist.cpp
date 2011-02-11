/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: TaskTemplate.cpp
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
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
#include "AutoAssist.h"
#include "PhotoElectric.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
};

//  Memory Log
class AutoAssistLog : public MemoryLog
{
public:
	AutoAssistLog() : MemoryLog(
			sizeof(struct abuf), AUTOASSIST_CYCLE_TIME, "AutoAssist",
			"Seconds,Nanoseconds,Elapsed Time\n"
			) {
		return;
	};
	~AutoAssistLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int AutoAssistLog::PutOne(void)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		// Add any values to be logged here
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int AutoAssistLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.))
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
AutonomousAssistTask::AutonomousAssistTask(void)
{
	Start((char *)"166TemplateTask", AUTOASSIST_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
	
	return;
};
	
// task destructor
AutonomousAssistTask::~AutonomousAssistTask(void)
{
	return;
};
	
// Main function of the task
int AutonomousAssistTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	// Register our logger
	AutoAssistLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 AutoAssist task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	// Register main robot task
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	int curr_value;
	x=y=r=lane=height=0;
	
	// "Actual" values to map to
	double height_list[] = {0,30,37,67,74,104,111};
	(void) height_list;
	
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		if(proxy->exists("Autonomous Lane")) {
			lane = (int)proxy->get("Autonomous Lane");
		}
		if(proxy->exists("Autonomous Height")) {
			height = (int)proxy->get("Autonomous Height");
		}
		wpi_assert(height >= 0 && height <= 6);
		
		if(proxy->get(DRIVER_AUTOASSIST)) {
			// The driver activated their autoassist
			proxy->UseUserJoystick(1,false);
			proxy->set(DRIVER_AUTOASSIST, Joystick(1).GetRawButton(6));
			if(proxy->exists("LineDirection")) {
				curr_value = (int)proxy->get("LineDirection");
				x=0;
				switch(curr_value) {
					case lRight:
						r = -AUTOASSIST_SPEED_TURN;
						break;
					case lCenter:
						r = 0;
						break;
					case lLeft:
						r = AUTOASSIST_SPEED_TURN;
						break;
					case lNo_Line:
						break;
					case lFork:
						if(proxy->exists("Autonomous Lane")) {
							if(proxy->get("Autonomous Lane")==2) {
								x = -AUTOASSIST_SPEED_STRAFE;
							}
							else if(proxy->get("Autonomous Lane")==4) {
								x = AUTOASSIST_SPEED_STRAFE;
							}
							else {
								x = 0;
							}
						}
						r = 0;
						break;
					default:
						r=0;
						break;
				}
			}
			if(proxy->exists("FrontSonar")) {
				if(proxy->get("FrontSonar") < AUTOASSIST_SONAR_FRONT_DISTANCE) {
					y=AUTOASSIST_SPEED_FORWARD;
				}
			}
			if(lane == 1 && proxy->exists("LeftSonar")) {
				// We want it to go to the left
				if(proxy->get("LeftSonar") > AUTOASSIST_SONAR_SIDE_DISTANCE) {
					x = -AUTOASSIST_SPEED_STRAFE;
				} else {
					x = 0;
				}
			} else if(lane == 5 && proxy->exists("RightSonar")) {
				// We want it to go to the right
				if(proxy->get("RightSonar") > AUTOASSIST_SONAR_SIDE_DISTANCE) {
					x = AUTOASSIST_SPEED_STRAFE;
				} else {
					x = 0;
				}
			}
			
			proxy->set("joy1x",x);
			proxy->set("joy1y",y);
			proxy->set("joy1r",r);
		} else {
			// Driver autoassist disabled
			proxy->UseUserJoystick(1,true);
		}
		
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
