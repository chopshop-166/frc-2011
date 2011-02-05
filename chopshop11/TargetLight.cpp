/*******************************************************************************
*  Project   		: Image Processing
*  File Name  		: TargetLight.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: February 2, 2011
*  File Description	: Returns a normalized number that says to go left or right
* 					  based on the image given.
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
//"Most of the vision functions return an integer value, 1 if the call was successful, and 0 if unsuccessful."



//Converts the camera image into a binary image with targets highlighted
int IsolateLightTargetRGB(Image* ReflectingTape, Image* srcimage)
	{
	//Create binary image with threshold technique
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
		if(!frcColorThreshold(ReflectingTape, srcimage, IMAQ_HSL, &R_Range, &G_Range, &B_Range)) {return 0;} 
		return 1;
	}

//Converts the camera image into a binary image with targets highlighted
int IsolateLightTargetHSL(Image* ReflectingTape, Image* srcimage)
	{
	//Create binary image with threshold technique
		Range R_Range;
		Range B_Range;
		Range G_Range;
		//Define the RGB minimum values
			R_Range.minValue = 0;
			G_Range.minValue = 0;
			B_Range.minValue = 0;
			R_Range.maxValue = 255;
			B_Range.maxValue = 255;	
			G_Range.maxValue = 255;
		if(!frcColorThreshold(ReflectingTape, srcimage, IMAQ_RGB, &R_Range, &G_Range, &B_Range)) {return 0;} 
		return 1;
	}

int CleanUpBinary(Image* ReflectingTape)
{
	//Edit the binary image that has targets highlighted for readability
		// Keeps large particles only
		//if(!imaqSizeFilter(ReflectingTape, ReflectingTape, 1, 0, IMAQ_KEEP_LARGE, NULL)) {return 0;} else {return 1;}
	return 1;
}

//This function is WPI's, but it instead searches for the widest particle
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

//Processes the image
int ProcessTheImage(Image* srcimage, double* targetCenterNormalized, ImageType WantedType)  
	{
	//Defining the needed variables
		Image* ReflectingTape = frcCreateImage(IMAQ_IMAGE_U8);
		int widestParticleIndex = 0;
		ParticleAnalysisReport Biggest;
		
	//Making the image usable
		//Transform the camera image into a binary image (Reflecting Tape) with only the targets as "on"
		//WantedType determines which method of thresholding is called
		if(WantedType = IMAQ_IMAGE_RGB)
		{
			if(!IsolateLightTargetRGB(ReflectingTape, srcimage))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "RGB Isolation of targets to binary failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				return 0;
			}
		}
		else if(WantedType = IMAQ_IMAGE_HSL)
		{
			if(!IsolateLightTargetHSL(ReflectingTape, srcimage))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "HSL Isolation of targets to binary failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				return 0;
			}
		}
		else {return 0; DPRINTF(LOG_INFO, "ProcessTheImage() does not support analysis of the wanted type.");}
		//Make the binary image more readable for analysis
		if(!CleanUpBinary(ReflectingTape))
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Binary Image enhancement failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			return 0;
		}
		
		
	//Using usable image to return 
		//Determine the wanted target: the central circle/the central circle and its strip-friend
		//If it worked, analyze the particle
		if(!GetWidestParticle(ReflectingTape, &widestParticleIndex)) {
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Widest particle identification failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			return 0;
		} 
		else{frcParticleAnalysis(ReflectingTape, widestParticleIndex, &Biggest);}  
		//Return the normalized center - points out which way to turn, accounting for camera's offset (if any)
		*targetCenterNormalized = Biggest.center_mass_x_normalized - CAMERA_OFFSET;
		return 1;
	};

//Processes the image, but also returns a picture for a snapshot
int ProcessTheImage(Image* srcimage, double* targetCenterNormalized, ImageType WantedType, Image* ColoredBinaryImage, ImageType type)  
	{
		return 0;
	};


/*
if(!imaqCast(ColoredBinaryImage, ReflectingTape, type, NULL, -1))
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Returning a colored binary image failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			return 0;
		}
*/

