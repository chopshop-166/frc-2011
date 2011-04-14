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
#include "Autonomous.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf
#define USING_AUTONOMOUS (1)

// Here are some speeds
const double Autonomous_Drive_Backward_Speed = sqrt(0.25); // Actual speed 0.25 after calculations
const double Autonomous_Arm_Down_Speed = 0.6;
const double Autonomous_Arm_Up_Speed = 0.75;
const double Autonomous_Drive_Forward_Speed = -sqrt(0.5);

AutonomousTask::AutonomousTask() {
	proxy = Proxy::getInstance();
	// We know lHandle really exists because it's the only thing that calls Autonomous
	lHandle = Robot::getInstance();
	main();
	// Stop EVERYTHING
	proxy->reset();
}

#define WaitUntil(t) { \
	while(proxy->get("matchtimer") > t) { \
		Wait(AUTONOMOUS_WAIT_TIME); \
		if(!lHandle->IsAutonomous()) { \
			return; \
		} \
	} \
}

void AutonomousTask::main() {
	// Create handles for proxy and robot
	Robot *lHandle;
#if USING_AUTONOMOUS
	Proxy *proxy = Proxy::getInstance();
#endif
	
	// Register robot handle
	while( !(lHandle = Robot::getInstance()) && !( lHandle->IsAutonomous() ) ) {
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	AnalogChannel lane_switch(AUTONOMOUS_DIAL_LANE);
	int lane_choice;
	// Each Volt is the right number, +- a couple millivolts
	lane_choice = (int)lane_switch.GetVoltage();
	
	if(lane_choice!=4) return;
	
#if USING_AUTONOMOUS
	lHandle->DriverStationDisplay("Find Sarah Connor");
	
	// Backing up...
	proxy->set(DRIVE_FORWARD_BACK, Autonomous_Drive_Backward_Speed); // Should move about 2 ft
	WaitUntil(14.5);
	
	// Stop moving back
	proxy->set(DRIVE_FORWARD_BACK,0);
	// Drop the arm
	proxy->set(ELBOW_AXIS, Autonomous_Arm_Down_Speed);
	WaitUntil(14.25);
	
	// Stop moving the arm
	proxy->set(ELBOW_AXIS, 0);
	// Open the gripper
	proxy->set(GRIPPER_BUTTON, true);
	Wait(AUTONOMOUS_WAIT_TIME);
	proxy->set(GRIPPER_BUTTON, false);
	// Drive Forward, same speed as the backwards one
	proxy->set(DRIVE_FORWARD_BACK, -Autonomous_Drive_Backward_Speed);
	WaitUntil(13.25);
	
	// Grab the tube
	proxy->set(GRIPPER_BUTTON, true);
	Wait(AUTONOMOUS_WAIT_TIME);
	proxy->set(GRIPPER_BUTTON, false);
	// Drive Forward, 6 ft/s
	proxy->set(DRIVE_FORWARD_BACK, Autonomous_Drive_Forward_Speed);
	// Go to the highest preset
	proxy->set(HIGH_PRESET_BUTTON, true);
	proxy->set(PRESET_TYPE_AXIS, 1);
	while((proxy->get("matchtimer") > 11.75) && lHandle->IsAutonomous()) {
		if(proxy->get("ArmAngle") < 1.9) {
			proxy->set(ELBOW_AXIS, 0);
		} else {
			proxy->set(ELBOW_AXIS, Autonomous_Arm_Down_Speed);
		}
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	// Release the tube
	proxy->set(GRIPPER_BUTTON, true);
	Wait(AUTONOMOUS_WAIT_TIME);
	proxy->set(GRIPPER_BUTTON, false);
	proxy->set(HIGH_PRESET_BUTTON, false);
	proxy->set(MID_PRESET_BUTTON, true);
	WaitUntil(10);
	proxy->reset();
	return;
#endif
}
