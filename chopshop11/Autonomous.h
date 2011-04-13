/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: Autonomous.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Set outputs to automate scoring
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/
#pragma once

#include "Robot.h"
#include "MemoryLog.h"
#include "WPILib.h"
#include "BaeUtilities.h"
#include <cmath>

// 13 ft/second at 500 RPM
#define AUTONOMOUS_WAIT_TIME (0.050)

class AutonomousTask
{
	Proxy *proxy;
	Robot *lHandle;
public:
	AutonomousTask(void);
//	void WaitUntil(double t); // t is the number of seconds left in Autonomous
	void main(void); // the main autonomous loop
};
