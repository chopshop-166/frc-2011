/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: SonarTask.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to get front and side sonar distances
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "Robot.h"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.

#define SONAR_CYCLE_TIME (50) // 50ms
#define AVERAGESIZE (10)	//Store 10 values
#define SONARINPERVOLT (81.0)	//Number of inches per 1 volt of change
class SonarTask : public Team166Task
{
	
public:
	
	// task constructor
	SonarTask(void);
		
	// task destructor
	virtual ~SonarTask(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	
	AnalogChannel SonarCenter, SonarLeft, SonarRight;
};
