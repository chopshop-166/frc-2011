/*******************************************************************************
*  Project   		: Framework
*  File Name  		: TaskTemplate.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Template header file for tasks, with template functions
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

#define ARM_CYCLE_TIME (10) // 10ms


class ArmTask : public Team166Task
{
	
public:
	
	// task constructor
	ArmTask(void);	

	// task destructor
	virtual ~ArmTask(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	//Declare Proxy and Robot handles
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;				// Local handle
//	CANJaguar armJag;			// Jaguar that controls arm's height
	AnalogChannel armChan;		// Temporary analog channel for potentiometer
	float speed;				// Speed that the arm moves at
};
