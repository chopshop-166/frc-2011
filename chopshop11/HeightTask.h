/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: HeightTask.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task that gets the elevator height
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "Robot.h"
#include "AnalogChannel.h"
#include "AnalogModule.h"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
#define HEIGHTTASK_CYCLE_TIME (20) // 20ms
#define InchesPerVolt 0.02  // This value is given in Spec sheet for LX-PA-50

class HeightTask166 : public Team166Task
{
	
public:
	
	// task constructor
	HeightTask166(void);
	
	// task destructor
	virtual ~HeightTask166(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;             // Local handle
	
	float HowHigh;
	AnalogChannel Height;
	
};
