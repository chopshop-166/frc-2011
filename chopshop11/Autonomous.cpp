/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: Autonomous.cpp
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Set outputs to automate scoring
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
	// Each Volt is the right number, +- a couple millivolts
	lane_choice = (int)lane_switch.GetVoltage();
	
	AnalogChannel height_switch(AUTONOMOUS_DIAL_HEIGHT);
	int height_choice;
	height_choice = (int)height_switch.GetVoltage();
	if(height_choice & 4) {
		// If it's 4 or 5, it's no good and disable
		height_choice = 0;
	}
	// Turns [0 1 2 3] into [0 2 4 6]
	// Heights are numbered 1-6 going up from the floor
	height_choice = 2 * height_choice;
	if(lane_choice != 3 && height_choice != 0) {
		// We're going for the slightly lowered ones
		height_choice = height_choice - 1;
	}
	
	string copilot_button_name = "";
	
	string lane_string = "";
	if(lane_choice==2 || lane_choice==1) {
		lane_string = LINE_STRAFE_LEFT_BUTTON;
	} else if(lane_choice==4 || lane_choice==5) {
		lane_string = LINE_STRAFE_RIGHT_BUTTON;
	}
	bool gripper_state=true;
	
	while( lHandle->IsAutonomous() ) {
		proxy->set(DRIVER_AUTOASSIST,true);
		if(copilot_button_name.size()) {
			proxy->set("HeightLocation",height_choice);
		}
		if(lane_string.size()) {
			proxy->set(lane_string,true);
		}
		
		if(proxy->get("matchtimer") < 1.5) {
			// Time's running out, so release
			gripper_state = false;
		} else {
			// Keep holding it
			gripper_state = true;
		}
		
		int ready_values = 0;
		int possible_ready_values = 0;
		if(proxy->exists("AutoassistReadyPosition")) {
			possible_ready_values++;
			if(proxy->get("AutoassistReadyPosition")) {
				ready_values++;
			}
		}
		if(proxy->exists("ArmReadyPosition")) {
			possible_ready_values++;
			if(proxy->get("ArmReadyPosition")) {
				ready_values++;
			}
		}
		if(proxy->exists("ElevatorReadyPosition")) {
			possible_ready_values++;
			if(proxy->get("ElevatorReadyPosition")) {
				ready_values++;
			}
		} 
		if(possible_ready_values == ready_values) {
			gripper_state = false;
		}
		
		proxy->set(GRIPPER_BUTTON,gripper_state);
		
		// This wait is required, it makes sure no task uses too much memory
		Wait(AUTONOMOUS_WAIT_TIME);
	}
}
