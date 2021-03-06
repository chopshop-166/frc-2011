/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: Defines.h    
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Robot code header which contains defines for global usage
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

// Driver axes and buttons
#define DRIVE_STRAFE ("Joy1X")
#define DRIVE_FORWARD_BACK ("Joy1Y")
// This is Joy1Z for Nick's joystick,
// but Joy1R for the other one
#define DRIVE_ROTATION ("Joy1Z")
#define DEPLOY_MINIBOT_PILOT ("joy1b6")
// Make sure this doesn't interfere with the logic
// Autoassist buttons
#define DRIVER_AUTOASSIST ("Joy1B2")
#define DRIVER_AUTOASSIST_REAL (2)
// Buttons for drive maintenence
#define DRIVER_SWITCH_JOYSTICKS ("Joy1B5")
#define DRIVER_SLOW_DRIVE ("Joy1B1")
// Copilot axes
#define ELEVATOR_AXIS ("Joy3Y")
#define ELBOW_AXIS ("Joy3A5")
// Color light buttons
#define TURNOFF_LIGHTS_BUTTON ("Joy3B1")
#define RED_LIGHT_BUTTON ("Joy3B2")
#define WHITE_LIGHT_BUTTON ("Joy3B3")
#define BLUE_LIGHT_BUTTON ("Joy3B4")
#define GRIPPER_BUTTON_RAW ("Joy3B5")
#define GRIPPER_BUTTON ((string)GRIPPER_BUTTON_RAW + "N")
#define DEPLOY_MINIBOT_COPILOT ("joy3A3")
// Preset buttons (xbox controller)
#define LOW_PRESET_BUTTON ("Joy3B1")
#define FLOOR_PRESET_BUTTON ("Joy3B2")
#define MID_PRESET_BUTTON ("Joy3B3")
#define HIGH_PRESET_BUTTON ("Joy3B4")
#define DUMP_LOGS_BUTTON ("Joy3B7")
#define PRESET_TYPE_AXIS ("Joy3A6")

// Jaguars
#define FRONT_RIGHT_JAGUAR (3)
#define FRONT_LEFT_JAGUAR (4)
#define BACK_RIGHT_JAGUAR (5)
#define BACK_LEFT_JAGUAR (6)
#define ELEVATOR_JAGUAR (13)

// Relays
#define COMPRESSORRELAY (1)

// Digital I/O
#define CENTERPHOTOSENSE (1)
#define LEFTPHOTOSENSE (2)
#define RIGHTPHOTOSENSE (3)
#define DEPLOYLIMIT (4)
#define HEIGHT_INPUT_A (5)
#define HEIGHT_INPUT_B (6)
#define PRESSURESWITCH (7)
#define ELEVATOR_BOTTOM_LIMIT_SWITCH (8)

// Analog input
#define SONAR_CENTER (1)
#define SONAR_RIGHT (2)
#define SONAR_LEFT (3)
#define ARM_POT (4)
#define AUTONOMOUS_DIAL_LANE (5)
#define AUTONOMOUS_DIAL_HEIGHT (6)
#define PRESSURETRANSDUCER (7)

/*
 *  Pneumatics bumper
 */
#define GRIPPER_OPEN (3)
#define GRIPPER_CLOSE (4)
#define ELEVATOR_BRAKE_RETRACT (5)
#define MINIBOT_ARM_RELEASE (6)
#define MINIBOT_ARM_LOCK (8)

/*
 *  PWM Outputs
 */
#define ARM_JAGUAR_PWM (1)
#define ARM_LOCK_PWM (2)
#define DRIVE_FL_PWM (3)
#define DRIVE_FR_PWM (4)
#define DRIVE_BL_PWM (5)
#define DRIVE_BR_PWM (6)
#define ELEVATOR_PWM (7)
