/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: Autonomous.cpp
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Robot code to execute in autonomous mode
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "Robot.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

AutonomousTask::AutonomousTask() {
	// Create handles for proxy and robot
	Robot *lHandle;
	Proxy *proxy;
	
	// Register robot handle
	while( !(lHandle = Robot::getInstance()) && !( lHandle->IsAutonomous() ) ) {
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	// Register proxy handle
	while( (proxy = Proxy::getInstance()) == NULL ) {
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	AnalogChannel lane_switch(5);
	int lane_choice;
	lane_choice = (int)lane_switch.GetVoltage();
	proxy->add("Autonomous Lane");
	proxy->set("Autonomous Lane", lane_choice);
	
	AnalogChannel height_switch(6);
	int height_choice;
	height_choice = (int)height_switch.GetVoltage();
	if(height_choice & 4) {
		height_choice = 0;
	}
	proxy->add("Autonomous Height");
	proxy->set("Autonomous Height", height_choice);
	
	while( lHandle->IsAutonomous() ) {
		proxy->set(DRIVER_AUTOASSIST,true);
		
		// This wait is required, it makes sure no task uses too much memory
		Wait(AUTONOMOUS_WAIT_TIME);
	}
}
