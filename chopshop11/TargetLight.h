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

// A return of 0 is a fail.
// A return of non-zero-ness is a win. 

int ProcessTheImage(Image* srcimage, double* targetCenterNormalized, Image* ColoredBinaryImage, bool* CanSeeTargets, bool* ImageReturned, bool SaveImages);
/*
 VARIABLE:					RETURNED/FED:		IS:
 srcimage 					fed					source image
 targetCenterNormalized		returned			return value of location of target
												-1 is left, 0 is center, 1 is right
 ColoredBinaryImage			returned			processed image (is returned)
 ImageReturned 				returned			if true, a processed image was returned
 SaveImages 				fed					if true, tries to return ColoredBinaryImage		
 	
 */

