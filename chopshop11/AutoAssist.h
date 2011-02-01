/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: AutoAssist.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Autonomous Assist to aid the drivers
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "Robot.h"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
#define AUTOASSIST_CYCLE_TIME (20) // 10ms

class AutonomousAssistTask : public Team166Task
{
	
public:
	
	// task constructor
	AutonomousAssistTask(void);

	// task destructor
	virtual ~AutonomousAssistTask(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
};