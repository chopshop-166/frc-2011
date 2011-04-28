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
#define actual_drive_speed(speed) sqrt(speed)
const double Autonomous_Drive_Backward_Speed = actual_drive_speed(0.25);
const double Autonomous_Arm_Down_Speed = 0.6;
const double Autonomous_Arm_Up_Speed = 0.75;
const double Autonomous_Drive_Forward_Speed = -actual_drive_speed(0.5);

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
			lHandle->DriverStationDisplay("CRAP"); \
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
	float timeleft;
	if(lane_choice!=4) return;
	
#if USING_AUTONOMOUS
	
	while(proxy->get("matchtimer") < 0.5 && lHandle->IsAutonomous()) {}
	lHandle->DriverStationDisplay("Find Sarah Connor");
	
	lHandle->DriverStationDisplay("Backing up");
	// Backing up...
	proxy->set(DRIVE_FORWARD_BACK, Autonomous_Drive_Backward_Speed); // Should move about 2 ft
	WaitUntil(14.5);
	
	lHandle->DriverStationDisplay("Lowering Arm");
	// Stop moving back
	proxy->set(DRIVE_FORWARD_BACK,0);
	// Drop the arm
	proxy->set(ELBOW_AXIS, Autonomous_Arm_Down_Speed);
	WaitUntil(13.5);
	
	lHandle->DriverStationDisplay("Opening Claw");
	// Stop moving the arm
	proxy->set(ELBOW_AXIS, 0);
	// Open the gripper
	proxy->set("JOY3B5", true);
	Wait(AUTONOMOUS_WAIT_TIME); 
	proxy->set("JOY3B5", false);
	WaitUntil(13);
	
	
	lHandle->DriverStationDisplay("Approaching Tube");
	// Drive Forward, same speed as the backwards one
	proxy->set(DRIVE_FORWARD_BACK, -Autonomous_Drive_Backward_Speed);
	WaitUntil(12);
	
	lHandle->DriverStationDisplay("Gripping Tube");
	proxy->set(DRIVE_FORWARD_BACK, 0);
	// Grab the tube
	proxy->set("JOY3B5", true);
	Wait(AUTONOMOUS_WAIT_TIME); 
	proxy->set("JOY3B5", false);
	WaitUntil(11.5);

	
	lHandle->DriverStationDisplay("Driving Forward");
	// Drive Forward, 6 ft/s
	proxy->set(DRIVE_FORWARD_BACK, Autonomous_Drive_Forward_Speed);
	
	// Go to the highest preset
	/*
	proxy->set(HIGH_PRESET_BUTTON, true);
	proxy->set(PRESET_TYPE_AXIS, 1);
	*/
	while((proxy->get("matchtimer") > 9.5) && lHandle->IsAutonomous()) {
		if(proxy->get("ArmAngle") < 1.9) {
			proxy->set(ELBOW_AXIS, 0);
		} else {
			proxy->set(ELBOW_AXIS, Autonomous_Arm_Down_Speed);
		}
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	WaitUntil(9.5);
	timeleft = proxy->get("matchtimer");
	lHandle->DriverStationDisplay("Time: %f", timeleft);
	proxy->set(DRIVE_FORWARD_BACK, Autonomous_Drive_Forward_Speed);
	WaitUntil(timeleft-0.5);
	lHandle->DriverStationDisplay("Releasing Tube");
	// Release the tube
	proxy->set(GRIPPER_BUTTON_RAW, true);
	Wait(AUTONOMOUS_WAIT_TIME);
//	proxy->set(GRIPPER_BUTTON_RAW, false);
//	proxy->set(HIGH_PRESET_BUTTON, false);
//	proxy->set(MID_PRESET_BUTTON, true);
	WaitUntil(timeleft-1.0);
	
	proxy->reset();
	lHandle->DriverStationDisplay("Subject terminated");
	return;
#endif
}
