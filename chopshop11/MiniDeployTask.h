/*******************************************************************************
*  Project   		: Framework
*  File Name  		: MiniDeployTask.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 21, 2011
*  File Description	: Header file for MiniDeploy task
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
#define MINIDEPLOY_CYCLE_TIME (10) // 10ms


class MiniDeploy166 : public Team166Task
{
	
public:
	
	// task constructor
	MiniDeploy166(void);

	// task destructor
	virtual ~MiniDeploy166(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	// Any variables that the task has as members go here
	// <<CHANGEME>>
};
