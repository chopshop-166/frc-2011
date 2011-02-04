/*******************************************************************************
*  Project   		: Framework
*  File Name  		: CameraTask.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Template source file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/* Find & Replace "Template" with the name you would like to give this task     */
/* Find & Replace "Testing" with the name you would like to give this task      */
/* Find & Replace "TaskTemplate" with the name you would like to give this task */
/*------------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Robot.h"
#include "CameraTask.h"
#include "TargetCircle.h"
#include "nivision.h"
#include "VisionAPI.h"
#include "TrackAPI.h"

#define CAMERA_OFFSET 0.0

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

int GetWidestParticle(Image* binaryImage, int* widestParticleIndex)
{
	*widestParticleIndex = 0; 
	int numParticles;
	int success = frcCountParticles(binaryImage, &numParticles);	
	
	if ( !success )	{  printf("Error: %d\n", imaqGetLastError()); return success;}			
	
	// if no particles found we quit here
	if (numParticles == 0)  {  printf("No widest particle found...\n"); 	} 
	
	// find the widest particle
	double widestParticleWidth = 0;
	double particleWidth;
	for (int i = 0; i < numParticles; ++i) {		
		success = imaqMeasureParticle(binaryImage, i, 0, IMAQ_MT_BOUNDING_RECT_WIDTH, &widestParticleWidth);
		if ( !success )	{ return success; }		
		if (particleWidth > widestParticleWidth) 	{
			// see if is in the right area
			if ( InArea(binaryImage, i, IMAQ_NO_RECT) ) {
				widestParticleWidth = particleWidth;
				*widestParticleIndex = i;  // return index to caller
			}
		}
	}
	
	return success;
};


int ProcessTheImage(Image* srcimage, double* targetCenterNormalized)  
{
	
//Defining the needed variables
	//Image holders:
		Image* ReflectingTape = frcCreateImage(IMAQ_IMAGE_U8);
	//Integers:
		int widestParticleIndex = 0;
	//Other variables:
		ParticleAnalysisReport Biggest;
		Range R_Range;
		Range B_Range;
		Range G_Range;
		//Define the RGB minimum values
			R_Range.minValue = 100;
			G_Range.minValue = 100;
			B_Range.minValue = 100;
			R_Range.maxValue = 255;
			G_Range.maxValue = 255;
			B_Range.maxValue = 255;
	
//The algorithm:
	//These transform the source image (srcimage) into a binary image that has only the targets as particles
			frcColorThreshold(ReflectingTape, srcimage, IMAQ_RGB, &R_Range, &G_Range, &B_Range);
	
	//logic
	
	int success = GetWidestParticle(ReflectingTape, &widestParticleIndex);
	
	if(!success) 
	{
		frcParticleAnalysis(ReflectingTape, widestParticleIndex, &Biggest);
		return 1;
	} 
	else
	{
		return 0;
	}
		
	*targetCenterNormalized = Biggest.center_mass_x_normalized - CAMERA_OFFSET;
};

int ProcessTheImage(Image* srcimage, double* targetCenterNormalized, Image* ColoredBinaryImage, ImageType* type)  
{
	
//Defining the needed variables
	//Image holders:
		Image* ReflectingTape = frcCreateImage(IMAQ_IMAGE_U8);
	//Integers:
		int widestParticleIndex = 0;
	//Other variables:
		ParticleAnalysisReport Biggest;
		Range R_Range;
		Range B_Range;
		Range G_Range;
		//Define the RGB minimum values
			R_Range.minValue = 100;
			G_Range.minValue = 100;
			B_Range.minValue = 100;
			R_Range.maxValue = 255;
			G_Range.maxValue = 255;
			B_Range.maxValue = 255;
	
//The algorithm:
	//These transform the source image (srcimage) into a binary image that has only the targets as particles
			frcColorThreshold(ReflectingTape, srcimage, IMAQ_RGB, &R_Range, &G_Range, &B_Range);
			imaqCast(ColoredBinaryImage, ReflectingTape, *type, NULL, -1); 

	
	//logic
	
	int success = GetWidestParticle(ReflectingTape, &widestParticleIndex);
	
	if(!success) 
	{
		frcParticleAnalysis(ReflectingTape, widestParticleIndex, &Biggest);
		return 1;
	} 
	else
	{
		return 0;
	}
		
	*targetCenterNormalized = Biggest.center_mass_x_normalized - CAMERA_OFFSET;
};


//This code was copied from WPI's library, but all instances of "large" were replaced with "wide"
