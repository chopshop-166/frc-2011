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
#include "Proxy.h"
#include "Vision/AxisCamera.h"
#include "nivision.h"
#include "VisionAPI.h"
#include "TrackAPI.h"

#define CAMERA_OFFSET 0.00625 //This needs to be changed in the .CPP file too

// A return of 0 is a fail.
// A return of non-zero-ness is a win. 


// THESE ARE THE FUNCTIONS THAT MAY BE CALLED:
/*

 */
int ProcessTheImage(Image* srcimage, double* targetCenterNormalized, Image* ColoredBinaryImage, bool* CanSeeTargets, bool* ImageReturned);
/*
 *  This returns a value between -1 and 1. 
 	The ProcessTheImage function will account for the distance and return targetCenterNormalized.
 		The closer to 1, the closer the target is to the right of the camera view.
 		The closer to -1, the closer the target is to the left of the camera view.
 	
 */


int GetWidestParticle(Image* binaryImage, int* widestParticleIndex);
int IsolateLightTarget(Image* ReflectingTape, Image* srcimage);
int CleanUpBinary(Image* ReflectingTape, bool AttemptCleanUp);
/*DO NOT CALL THESE FUNCTIONS
 This function is called in ProcessTheImage.
 */
