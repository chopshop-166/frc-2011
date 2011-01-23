/*******************************************************************************
*  Project   		: Framework
*  File Name  		: HeightTask.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 23, 2011
*  File Description	: Template header file for tasks, with template functions
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
// You should rename this when you copy it into a new file
// <<CHANGEME>>
#define HEIGHTTASK_CYCLE_TIME (10) // 10ms

// Rename this, too, or you'll run into collisions
// <<CHANGEME>>
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
	// Any variables that the task has as members go here
	// <<CHANGEME>>
};
