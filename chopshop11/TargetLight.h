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

#define CAMERA_OFFSET 0.0 //This needs to be changed in the .CPP file too

// A return of 0 is a fail.
// A return of non-zero-ness is a win. Success. 


// THESE ARE THE FUNCTIONS THAT MAY BE CALLED:
int ProcessTheImage(Image* ImageSource, double* targetCenterNormalized, ImageType WantedType);
/*
 This returns a value between -1 and 1. 
 	Set the CAMERA_OFFSET to perceived center (at distance wanted)
 	The ProcessTheImage function will account for the distance and return targetCenterNormalized.
 		The closer to 1, the closer the target is to the right of the camera view.
 		The closer to -1, the closer the target is to the left of the camera view.
 */
int ProcessTheImage(Image* srcimage, double* targetCenterNormalized, ImageType WantedType, Image* ColoredBinaryImage, ImageType type);
/*
 This has the exact same script as above, but it returns a "colored binary image" as well. 
 In other words, the input image "ColoredBinaryImage" is transformed into a color image with binary properties. 
 Input one of the following for a specific image type: 
 	    IMAQ_IMAGE_U8              = 0,           //The image type is 8-bit unsigned integer grayscale.
    	IMAQ_IMAGE_U16             = 7,           //The image type is 16-bit unsigned integer grayscale.
   		IMAQ_IMAGE_I16             = 1,           //The image type is 16-bit signed integer grayscale.
   		IMAQ_IMAGE_SGL             = 2,           //The image type is 32-bit floating-point grayscale.
    	IMAQ_IMAGE_COMPLEX         = 3,           //The image type is complex.
    	IMAQ_IMAGE_RGB             = 4,           //The image type is RGB color.
    	IMAQ_IMAGE_HSL             = 5,           //The image type is HSL color.
    	IMAQ_IMAGE_RGB_U64         = 6,           //The image type is 64-bit unsigned RGB color.
  
 */

int GetWidestParticle(Image* binaryImage, int* widestParticleIndex);
int IsolateLightTarget(Image* ReflectingTape, Image* srcimage);
/*DO NOT CALL THESE FUNCTIONS
 This function is called in ProcessTheImage.
 */
