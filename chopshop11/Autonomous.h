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

#define AUTONOMOUS_WAIT_TIME (0.050)
#define AUTONOMOUS_MOVE_SECONDS (4)
#define RELEASE_SECONDS (5)

#define AUTONOMOUS_FORWARD_SPEED (-0.4)
#define AUTONOMOUS_BACKWARD_SPEED (0.3)
#define AUTONOMOUS_ARM_SPEED (-0.7)
#define AUTONOMOUS_ANGLE (2.4)

class AutonomousTask
{
public:
	AutonomousTask(void);
	// <<CHANGEME>>
	// Add any extra functions your autonomous needs
};
