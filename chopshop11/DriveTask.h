/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: DriveTask.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Takes X, Y, Z inputs from joystick generates outputs for mecanum wheels
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
#define DRIVE_TASK_CYCLE_TIME (20) // 20ms

class DriveTask : public Team166Task
{
	
public:
	
	// task constructor
	DriveTask(void);
	
	// task destructor
	virtual ~DriveTask(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	
	void Normalize(float*);
	double SignPreservingSquare(double);
	double TruncateDouble(double);
	int LostComms(CANJaguar&, int);
	float m_maxOutput;
	int syncGroup;
	CANJaguar fl, fr, bl, br;
	float x,y,r;
	float wheelSpeeds[4];
	float actualSpeed[4];
	int encoderBad[4];
};
