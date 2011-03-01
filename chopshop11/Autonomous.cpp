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
	// Chooses a height and height axis
	string copilot_button_name = "";
	if(height_choice == 1) {
		copilot_button_name = LOW_PRESET_BUTTON;
	} else if(height_choice == 2) {
		copilot_button_name = MID_PRESET_BUTTON;
	} else if(height_choice == 3) {
		copilot_button_name = HIGH_PRESET_BUTTON;
	}
	
	string lane_string = "";
	float preset_choice;
	if(lane_choice==2 || lane_choice==1) {
		// We want to head to the left
		lane_string = LINE_STRAFE_LEFT_BUTTON;
		preset_choice = -1;
	} else if(lane_choice==4 || lane_choice==5) {
		// We want to head to the right
		lane_string = LINE_STRAFE_RIGHT_BUTTON;
		preset_choice = -1;
	} else if(lane_choice == 3) {
		// We're going to the center
		preset_choice = 1;
	} else {
		// We shouldn't be moving...
		preset_choice = 0;
	}
	
	bool gripper_state=true;
	unsigned timer=0;
	
	while( lHandle->IsAutonomous() ) {
#if 0
		proxy->set(DRIVER_AUTOASSIST,true);
		
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
		
		if(proxy->get("matchtimer") < 1.5) {
			// Time's running out, so release
			gripper_state = false;
		} else {
			// Keep holding it
			gripper_state = true;
		}
		
		proxy->set(GRIPPER_BUTTON,gripper_state);
#else
		if(++timer <= (1000*AUTONOMOUS_WAIT_TIME*AUTONOMOUS_SECONDS)) {
			proxy->set(DRIVE_FOWARD_BACK, 0.25);
		} else {
			proxy->set(DRIVE_FOWARD_BACK, 0);
			if(gripper_state == true) {
				gripper_state = false;
				proxy->set(GRIPPER_BUTTON, true);
			} else {
				proxy->set(GRIPPER_BUTTON, false);
			}
		}
#endif
		if(copilot_button_name.size()) {
			proxy->set(copilot_button_name,true);
		}
		if(lane_string.size()) {
			proxy->set(lane_string,true);
		}
		proxy->set(PRESET_TYPE_AXIS,preset_choice);
		if(proxy->get("ArmAngle") < 2.90) {
			proxy->set(ELBOW_AXIS, 0.2);
		}
		
		// This wait is required, it makes sure no task uses too much memory
		Wait(AUTONOMOUS_WAIT_TIME);
	}
}
