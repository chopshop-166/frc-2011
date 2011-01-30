/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: CameraTask.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to run the camera and tracking
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "Robot.h"
#include "Proxy.h"
#include "Vision/AxisCamera.h"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
#define CAMERA_CYCLE_TIME (20) // ms

/** Private NI function needed to write to the VxWorks target */
IMAQ_FUNC int Priv_SetWriteFileAllowed(UINT32 enable); 

// Store an image on the cRIO
void SaveImage(char* imageName, Image* image);

class CameraTask : public Team166Task 
{
	
public:
	
	// task constructor
	CameraTask(void);

	// task destructor
	virtual ~CameraTask(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
	// Take a picture and store to cRIO
	void TakeSnapshot(char* imageName);
	// Search for Target
	void FindTargets();
	
private:
	// Any variables that the task has as members go here
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	AxisCamera &camera;
};
