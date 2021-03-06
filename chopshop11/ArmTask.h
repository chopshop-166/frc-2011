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

#define ARM_CYCLE_TIME (50) // 50ms
#define ANGLE_LIST_SIZE (10) // Number of angles to average as the current
#define MAXHEIGHT (4.15)	//Maximum angle of arm
#define ARM_USES_CAN (0)
#define ENGAGED (0.632)
#define DISENGAGED (.397)
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
#if ARM_USES_CAN
	CANJaguar armJag;			// Jaguar that controls arm's height
#else
	Jaguar armJag;				// Jaguar that controls arm's height
#endif
	float speed, deadband;		// Speed that the arm moves at
	DoubleSolenoid gripper;		// The solenoid that controls the gripper
	AnalogChannel potentiometer;	// Potentiometer
	float high_limit, low_limit;	// High and Low limits 
	Servo ArmLock;				// Servo to lock arm in place
};
