/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: PneumaticsTask.h
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task for running compressor and measuring pressure
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "Robot.h"

//
// This constant defines how often we want this task to run in the form
// of milliseconds. Max allowed time is 999 miliseconds.
#define PNEUMATICS_CYCLE_TIME (100) // 100ms
#define PSI_PER_VOLT (62.5)
class PneumaticsTask : public Team166Task
{
	
public:
	
	// task constructor
	PneumaticsTask(void);
	
	// task destructor
	virtual ~PneumaticsTask(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	
	AnalogChannel PSITransducer;
	Compressor AirCompresser;
};
