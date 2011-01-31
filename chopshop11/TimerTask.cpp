/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: TimerTask.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to send the remaining match time to the dashboard
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "TimerTask.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// task constructor
TimerTask::TimerTask(void)
{
	Start((char *)"166TimerTask", TIMER_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
	// Register main robot task
	lHandle = Robot::getInstance();
	return;
};
	
// task destructor
TimerTask::~TimerTask(void)
{
	return;
};
	
// Main function of the task
int TimerTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Template task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
		
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		SmartDashboard::Log(proxy->get("matchtimer"), "Remaining Time");

		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
