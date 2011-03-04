/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: MiniDeployTask.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to handle minibot deployment logic
*******************************************************************************/
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "Robot.h"

// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
#define MINIDEPLOY_CYCLE_TIME (20) // 10ms

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
	enum DeployState {kWait=0, kSwing = 1, kExtend = 2, kDeploy=3, kRepunch=4, kReRetract=5, kFinished=6} Deploy_State;
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	Solenoid DeployerExtender;	// Extend the minibot deployment mechanism
	DoubleSolenoid MiniDeployer;		//Close the minibot on the pole
	Solenoid MiniRelease;		//Release MiniBot Deployer
	DigitalInput Deploy_Limit;
	
};
