/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: ElevatorTask.h
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to run the elevator
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
#define ELEVATOR_CYCLE_TIME (25) // 25ms
#define ELEVATOR_USES_CAN (1)

#define ClicksPerInch 162.977  //1024/circumference of drum


class ElevatorTask : public Team166Task
{
	
public:
	
	// task constructor
	ElevatorTask(void);

	// task destructor
	virtual ~ElevatorTask(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	
#if ELEVATOR_USES_CAN
	CANJaguar elevator;
#else
	Jaguar elevator;
#endif
	const float speed, deadband, height_deadband;
	Solenoid brakeSolenoid;
	Encoder Height;
};
