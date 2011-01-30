/*******************************************************************************
*  Project   		: Framework
*  File Name  		: ColorLights.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to run the LEDs to tell the feeders the needed colors
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
// You should rename this when you copy it into a new file
// <<CHANGEME>>
#define DRIVE_TASK_CYCLE_TIME (20) // 20ms

// Rename this, too, or you'll run into collisions
// <<CHANGEME>>
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
	void Normalize(double*);
	float m_maxOutput;
	int syncGroup;
	CANJaguar fl, fr, bl, br;
	double x,y,r;
	double wheelSpeeds[4];
};
