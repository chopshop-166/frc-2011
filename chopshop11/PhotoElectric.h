/*******************************************************************************
*  Project   		: Framework
*  File Name  		: Retroreflective.h     
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
// You should rename this when you copy it into a new file
// <<CHANGEME>>
#define PHOTOELECTRIC_CYCLE_TIME (20) // 20ms

// Rename this, too, or you'll run into collisions
// <<CHANGEME>>
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
	// Any variables that the task has as members go here
	// <<CHANGEME>>
};
