/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: ColorLights.h   
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to run the LEDs to tell the feeders the needed colors
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
#define COLORLIGHT_CYCLE_TIME (100) // 100ms

class ColorLightTask : public Team166Task
{
	
public:
	
	// task constructor
	ColorLightTask(void);

	// task destructor
	virtual ~ColorLightTask(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	
	Relay RedWhite;
	Relay Blue;
//	Relay red;
//	Relay white;
//	Relay blue;
};
