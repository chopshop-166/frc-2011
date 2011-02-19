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
	Proxy *proxy = Proxy::getInstance();
	
	// Register robot handle
	while( !(lHandle = Robot::getInstance()) && !( lHandle->IsAutonomous() ) ) {
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	AnalogChannel lane_switch(AUTONOMOUS_DIAL_LANE);
	int lane_choice;
	lane_choice = (int)lane_switch.GetVoltage();
	proxy->add("Autonomous Lane");
	proxy->set("Autonomous Lane", lane_choice);
	
	AnalogChannel height_switch(AUTONOMOUS_DIAL_HEIGHT);
	int height_choice;
	height_choice = (int)height_switch.GetVoltage();
	if(height_choice & 4) {
		height_choice = 0;
	}
	height_choice = 2 * height_choice;
	if(lane_choice != 3 && height_choice != 0) {
		// We're going for the slightly lowered ones
		height_choice = height_choice - 1;
	}
	
	string copilot_button_name = "";
	switch(height_choice) {
		case 1:
			copilot_button_name = LOW_SIDE_PRESET_BUTTON;
			break;
		case 2:
			copilot_button_name = LOW_CENTER_PRESET_BUTTON;
			break;
		case 3:
			copilot_button_name = MIDDLE_SIDE_PRESET_BUTTON;
			break;
		case 4:
			copilot_button_name = MIDDLE_CENTER_PRESET_BUTTON;
			break;
		case 5:
			copilot_button_name = TOP_SIDE_PRESET_BUTTON;
			break;
		case 6:
			copilot_button_name = TOP_CENTER_PRESET_BUTTON;
			break;
	}
	
	string lane_string = "";
	if(lane_choice==2 || lane_choice==1) {
		lane_string = LINE_STRAFE_LEFT_BUTTON;
	} else if(lane_choice==4 || lane_choice==5) {
		lane_string = LINE_STRAFE_RIGHT_BUTTON;
	}
	
	while( lHandle->IsAutonomous() ) {
		proxy->set(DRIVER_AUTOASSIST,true);
		if(copilot_button_name.size()) {
			proxy->set(copilot_button_name,true);
		}
		if(lane_string.size()) {
			proxy->set(lane_string,true);
		}
		
		if(proxy->get("matchtimer") < 1.5) {
			proxy->set(GRIPPER_BUTTON,false);
		} else {
			proxy->set(GRIPPER_BUTTON,true);
		}
		
		// This wait is required, it makes sure no task uses too much memory
		Wait(AUTONOMOUS_WAIT_TIME);
	}
}
