/*******************************************************************************
*  Project   		: Framework
*  File Name  		: CameraTask.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Template header file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "Robot.h"
#include "Vision/AxisCamera.h"
#include "TrackAPI.h"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
#define CAMERA_CYCLE_TIME (20) // ms

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
	void ProcessImage();
	
private:
	// Any variables that the task has as members go here
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	AxisCamera &camera;

	Image* ReflectingTape;
	Image* srcimage;
	int numParticles;
	int largestParticleIndex;
	int widestParticleIndex;
	ParticleAnalysisReport Biggest;
	int CameraTask::GetWidestParticle(Image* binaryImage, int* widestParticleIndex);
	double targetCenterNormalized;
	int CameraTask::ProcessImage(double* targetCenterNormalized);
	/*ddduuurrrrr...?
	 */
	int imageProcessResult;
	
	Range R_Range;
	Range B_Range;
	Range G_Range;
};
