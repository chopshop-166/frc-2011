/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: cameraServo.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Control a camera by moving the servos its mounted on
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#if !defined(_CAMERA_SERVO_H)
#define _CAMERA_SERVO_H
#include "WPILib.h"
#include "Robot.h"
#include "Vision/AxisCamera.h"
#include "TrackAPI.h"
#include "vector"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
// You should rename this when you copy it into a new file 
#define CAMERA_SERVO_CYCLE_TIME (10) // 10ms

// Rename this, too, or you'll run into collisions
class CameraServo : public Team166Task
{
	
public:
	
	// task constructor
	CameraServo(void);

	// task destructor
	virtual ~CameraServo(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	// Any variables that the task has as members go here
	Servo cameraX;
	Servo cameraY;
	AxisCamera &camera;
	Image* destimage;
	Image* srcimage;
	ParticleAnalysisReport Particle_Report;
	Range Hue_Range;
	Range Sat_Range;
	Range Lum_Range;
};

#endif // !defined(_TASK_TEMPLATE_H)
