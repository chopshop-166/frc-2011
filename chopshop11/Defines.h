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

// Driver and copilot buttons
#define DRIVE_STRAFE ("Joy1X")
#define DRIVE_FOWARD_BACK ("Joy1Y")
#define DRIVE_ROTATION ("Joy1R")
#define DRIVER_AUTOASSIST ("Joy1B2")
#define DRIVER_AUTOASSIST_REAL (2)
#define LINE_STRAFE_LEFT_BUTTON ("Joy1B3")
#define LINE_STRAFE_RIGHT_BUTTON ("Joy1B4")
#define DEPLOY_MINIBOT ("joy1b6")

#define WRIST_AXIS ("Joy3X")
#define ELEVATOR_AXIS ("Joy3Y")
#define FLOOR_PRESET_BUTTON ("Joy3B2")
#define TURNOFF_LIGHTS_BUTTON ("Joy3B2N")
#define WHITE_LIGHT_BUTTON ("Joy3B3N")
#define RED_LIGHT_BUTTON ("Joy3B4N")
#define BLUE_LIGHT_BUTTON ("Joy3B5N")
#define GRIPPER_BUTTON ("Joy3B5N")

// Jaguars
#define FRONT_RIGHT_JAGUAR (3)
#define FRONT_LEFT_JAGUAR (4)
#define BACK_RIGHT_JAGUAR (5)
#define BACK_LEFT_JAGUAR (6)
#define ELEVATOR_JAGUAR (13)
#define ARM_JAGUAR (16)

// Relays
#define COMPRESSORRELAY (1)
#define RED_WHITE_COLOR_LIGHT (4)
#define BLUE_COLOR_LIGHT (5)

// Digital I/O
#define CENTERPHOTOSENSE (1)
#define LEFTPHOTOSENSE (2)
#define RIGHTPHOTOSENSE (3)
#define HEIGHT_INPUT_A (5)
#define HEIGHT_INPUT_B (6)
#define PRESSURESWITCH (7)

// Analog input
#define SONAR_CENTER (1)
#define SONAR_RIGHT (2)
#define SONAR_LEFT (3)
#define AUTONOMOUS_DIAL_LANE (5)
#define AUTONOMOUS_DIAL_HEIGHT (6)
#define PRESSURETRANSDUCER (7)

// Pneumatics bumper
#define MINIBOT_DEPLOYER_EXTENDER (1)
#define MINIBOT_DEPLOYER (2)
#define GRIPPER_OPEN (3)
#define GRIPPER_CLOSE (4)
#define ELEVATOR_BRAKE_EXTEND (5)
