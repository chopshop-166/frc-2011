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
#define USING_AUTONOMOUS (1)

AutonomousTask::AutonomousTask() {
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
	
	AnalogChannel height_switch(AUTONOMOUS_DIAL_HEIGHT);
	int height_choice;
	height_choice = (int)height_switch.GetVoltage();
	
	if(lane_choice==height_choice && lane_choice==5) return;
	
#if USING_AUTONOMOUS
	unsigned timer=0;
	enum {sDriving, sRising, sHanging, sDelay, sReverse, sWait} state = sDriving;
	lHandle->DriverStationDisplay("State: Forward");
#endif
	
	float angle;
	
	while( lHandle->IsAutonomous() ) {
#if USING_AUTONOMOUS
		switch (state) {
			case sDriving:
				proxy->set(DRIVE_FOWARD_BACK, AUTONOMOUS_FORWARD_SPEED);
				
				angle = proxy->get("ArmAngle");
				if(angle < AUTONOMOUS_ANGLE) {
					proxy->set(ELBOW_AXIS, AUTONOMOUS_ARM_SPEED);
				} else {
					proxy->set(ELBOW_AXIS, 0);
				}
				
				if(!proxy->get("ElevatorZeroed")) {
					proxy->set(ELEVATOR_AXIS,0.5);
				}
				
				++timer;
				if(timer > (500*AUTONOMOUS_WAIT_TIME*AUTONOMOUS_MOVE_SECONDS)) {
					timer = 0;
					state = sRising;
					lHandle->DriverStationDisplay("State: Rising");
				}
				break;
			case sRising:
				proxy->set(DRIVE_FOWARD_BACK, 0);
				proxy->set(PRESET_TYPE_AXIS, 1);
				proxy->set(HIGH_PRESET_BUTTON, 1);
				
				angle = proxy->get("ArmAngle");
				if(angle < AUTONOMOUS_ANGLE) {
					proxy->set(ELBOW_AXIS, AUTONOMOUS_ARM_SPEED);
				} else {
					proxy->set(ELBOW_AXIS, 0);
				}
				if (proxy->exists("ElevatorReadyPosition")) {
					if(proxy->get("ElevatorReadyPosition")) {	
						state = sHanging;
						lHandle->DriverStationDisplay("State: Hanging");
					}
				}
				break;
			case sHanging:
				proxy->set(DRIVE_FOWARD_BACK, AUTONOMOUS_FORWARD_SPEED*(3/4));
				
				++timer;
				if(timer > (500*AUTONOMOUS_WAIT_TIME*RELEASE_SECONDS)) {
					proxy->set(GRIPPER_BUTTON, true);
					timer = 0;
					state = sDelay;
					lHandle->DriverStationDisplay("State: Delay");
				}
				break;
			case sDelay:
				proxy->set(PRESET_TYPE_AXIS, 0);
				proxy->set(HIGH_PRESET_BUTTON, 0);
				
				++timer;
				if(timer > (50*AUTONOMOUS_WAIT_TIME*RELEASE_SECONDS)) {
					timer = 0;
					state = sReverse;
					lHandle->DriverStationDisplay("State: Reverse");
				}
				break;
			case sReverse:
				proxy->set(GRIPPER_BUTTON, false);
				proxy->set(ELBOW_AXIS, 0);
				proxy->set(PRESET_TYPE_AXIS, 0);
				proxy->set(HIGH_PRESET_BUTTON, 0);
				proxy->set(DRIVE_FOWARD_BACK,AUTONOMOUS_BACKWARD_SPEED);
				
				++timer;
				if(timer > (500*AUTONOMOUS_WAIT_TIME*AUTONOMOUS_MOVE_SECONDS)) {
					state = sWait;
					lHandle->DriverStationDisplay("State: Wait");
				}
				break;
			case sWait:
			default:
				proxy->set(DRIVE_STRAFE,0);
				proxy->set(DRIVE_FOWARD_BACK,0);
				proxy->set(DRIVE_ROTATION,0);
				proxy->set(PRESET_TYPE_AXIS, 0);
				proxy->set(HIGH_PRESET_BUTTON, 0);
				break;
		}
#endif
		// This wait is required, it makes sure no task uses too much memory
		Wait(AUTONOMOUS_WAIT_TIME);
	}
}
