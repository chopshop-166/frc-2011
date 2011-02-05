/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: CameraTask.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to run the camera and tracking
*  File Description	: Definition of CameraTask class and associated data
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "Robot.h"
#include "Proxy.h"
#include "Vision/AxisCamera.h"
#include "TrackAPI.h"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
#define CAMERA_CYCLE_TIME (20) // ms

/** Private NI function needed to write to the VxWorks target */
IMAQ_FUNC int Priv_SetWriteFileAllowed(UINT32 enable); 

/**
 * Store an Image to the cRIO in the specified path
 */
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
	// Any task should be able to call this to take and save a snapshot
	static void TakeSnapshot(char* imageName);
	
	// Search for Target
	bool FindCircleTargets();
	bool FindLightTargets();
	
private:
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	AxisCamera &camera;
	static CameraTask *myHandle;      // handle to myself for static function
	
	//values to log
	double targetHAngle;
	double targetVAngle;
	double targetSize;
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
