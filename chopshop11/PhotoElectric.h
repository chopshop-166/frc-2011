/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: PhotoElectric.h
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to get photoelectric sensor values
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
#define PHOTOELECTRIC_CYCLE_TIME (20) // 20ms
enum LineStateType { No_Line = -2, Left = -1, Center = 0, Right = 1, Fork = 2, T = 3 };

class PhotoElectricTask : public Team166Task
{
public:
	
	// task constructor
	PhotoElectricTask(void);

	// task destructor
	virtual ~PhotoElectricTask(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	
	LineStateType LineState;
	DigitalInput left;
	DigitalInput center;
	DigitalInput right;
};
