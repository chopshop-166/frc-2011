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
#include "stdlib.h"

#define CAMERA_OFFSET 0.0
#define DO_BINARY_IMAGE_CLEAN_UP false

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(true)dprintf
//"Most of the vision functions return an integer value, 1 if the call was successful, and 0 if unsuccessful."



//Converts the camera image by RGB threshold into a binary image with targets highlighted
//Debugging info is reported where function is called
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
		if(!frcColorThreshold(ReflectingTape, srcimage, IMAQ_RGB, &R_Range, &G_Range, &B_Range)) {return 0;} 
		return 1;
	}

//Converts the camera image by HSL threshold into a binary image with targets highlighted
//Debugging info is reported where function is called
int IsolateLightTargetHSL(Image* ReflectingTape, Image* srcimage)
	{
	//Create binary image with threshold technique
		Range H_Range;
		Range S_Range;
		Range L_Range;
		//Define the HSL minimum values 
			H_Range.minValue = 0;
			H_Range.maxValue = 255;
			S_Range.minValue = 0;
			S_Range.maxValue = 45;
			L_Range.minValue = 200;	
			L_Range.maxValue = 255;
			/*H_Range.minValue = 0;
			H_Range.maxValue = 255;
			S_Range.minValue = 0;
			S_Range.maxValue = 255;
			L_Range.minValue = 0;	
			L_Range.maxValue = 255;*/
		if(!frcColorThreshold(ReflectingTape, srcimage, IMAQ_HSL, &H_Range, &S_Range, &L_Range)) {return 0;} 
		return 1;
	}

//Edit the binary image that has targets highlighted for readability
//Debugging info IS REPORTED IN THE FUNCTION
int CleanUpBinary(Image* ReflectingTape, bool AttemptCleanUp)
{
	if (AttemptCleanUp)
	{

		//Separate particles that are barely connected
			StructuringElement myStructuringElement;
			myStructuringElement.matrixCols = 3;
			myStructuringElement.matrixRows = 3;
			myStructuringElement.hexa = FALSE;
			myStructuringElement.kernel = (int*) malloc(9 * sizeof(int));
			myStructuringElement.kernel[0] = 1;
			myStructuringElement.kernel[1] = 0;
			myStructuringElement.kernel[2] = 1;
			myStructuringElement.kernel[3] = 1;
			myStructuringElement.kernel[4] = 0;
			myStructuringElement.kernel[5] = 1;
			myStructuringElement.kernel[6] = 1;
			myStructuringElement.kernel[7] = 0;
			myStructuringElement.kernel[8] = 1;
			if (!imaqSeparation(ReflectingTape, ReflectingTape, 1, &myStructuringElement))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "Separation of barely connected particles in binary image failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				return 0;
			}

			
		// Keeps large particles only
			if(!imaqSizeFilter(ReflectingTape, ReflectingTape, 1, 0, IMAQ_KEEP_LARGE, NULL)) 
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "Elimination of small particles in binary image failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				return 0;
			}
	
		//Fill holes in the particles - MAY BE UNNECESSARY
			if(!imaqFillHoles(ReflectingTape, ReflectingTape, true)) 
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "Filling of holes in binary image failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				return 0;
			}
/*
		
		if(!imaqSizeFilter(ReflectingTape, ReflectingTape, 1, 0, IMAQ_KEEP_LARGE, NULL)) 
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Elimination of small particles in binary image failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			return 0;
		}
*/
	}
	
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
		if (numParticles == 0)  {  printf("No widest particle found...\n"); } 
		
		// find the widest particle
		double widestParticleWidth = 0;
		double particleWidth;
		for (int i = 0; i < numParticles; ++i) {		
			success = imaqMeasureParticle(binaryImage, i, 0, IMAQ_MT_BOUNDING_RECT_WIDTH, &particleWidth);
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
		
	//Isolate the targets in the image
		//Transform the camera image into a binary image (Reflecting Tape) with only the targets as "on"
		//WantedType determines which method of thresholding is called
		if(WantedType == IMAQ_IMAGE_RGB)
		{
			if(!IsolateLightTargetRGB(ReflectingTape, srcimage))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "RGB Isolation of targets to binary failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				return 0;
			} else {DPRINTF(LOG_INFO, "Isolated targets by RGB.");}
		}
		else if(WantedType == IMAQ_IMAGE_HSL)
		{
			if(!IsolateLightTargetHSL(ReflectingTape, srcimage))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "HSL Isolation of targets to binary failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				return 0;
			} else {DPRINTF(LOG_INFO, "Isolated targets by HSL.");}
		}
		else {return 0; DPRINTF(LOG_INFO, "ProcessTheImage() does not support analysis of the wanted type.");}
	
	//Make the binary image more readable for analysis
		//Debugging info is REPORTED IN THE FUNCTION BY ITSELF.
		if(!CleanUpBinary(ReflectingTape, DO_BINARY_IMAGE_CLEAN_UP))
		{
			DPRINTF(LOG_INFO, "Binary Image enhancement failed - see above.");
			return 0;
		} 
		else if (!DO_BINARY_IMAGE_CLEAN_UP)
		{
			DPRINTF(LOG_INFO, "Did not attempt cleanup of targets.");
		} else {DPRINTF(LOG_INFO, "Cleaned up targets.");}
		
		
	//Using usable image to return 
		//Determine the wanted target: the central circle/the central circle and its strip-friend
		//If it worked, analyze the particle
		if(!GetWidestParticle(ReflectingTape, &widestParticleIndex)) 
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Widest particle identification failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			return 0;
		} else {DPRINTF(LOG_INFO, "Got widest target.");}
		if(!frcParticleAnalysis(ReflectingTape, widestParticleIndex, &Biggest))
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Widest particle analysis failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			return 0;
		} else {DPRINTF(LOG_INFO, "Analyzed center target.");}
		
		//Return the normalized center - points out which way to turn, accounting for camera's offset (if any)
		*targetCenterNormalized = Biggest.center_mass_x_normalized - CAMERA_OFFSET;
		DPRINTF(LOG_INFO, "Found target center.");
		return 1;
	};

//Processes the image, but also returns a picture for a snapshot
int ProcessTheImage(Image* srcimage, double* targetCenterNormalized, ImageType WantedType, Image* ColoredBinaryImage, ImageType type)  
{
	//Defining the needed variables
		Image* ReflectingTape = frcCreateImage(IMAQ_IMAGE_U8);
		int widestParticleIndex = 0;
		ParticleAnalysisReport Biggest;
		
	//Isolate the targets in the image
		//Transform the camera image into a binary image (Reflecting Tape) with only the targets as "on"
		//WantedType determines which method of thresholding is called
		if(WantedType == IMAQ_IMAGE_RGB)
		{
			if(!IsolateLightTargetRGB(ReflectingTape, srcimage))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "RGB Isolation of targets to binary failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				return 0;
			} else {DPRINTF(LOG_INFO, "Isolated targets by RGB.");}
		}
		else if(WantedType == IMAQ_IMAGE_HSL)
		{
			if(!IsolateLightTargetHSL(ReflectingTape, srcimage))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "HSL Isolation of targets to binary failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				return 0;
			} else {DPRINTF(LOG_INFO, "Isolated targets by HSL.");}
		}
		else {return 0; DPRINTF(LOG_INFO, "ProcessTheImage() does not support analysis of the wanted type.");}
	
	//Make the binary image more readable for analysis
		//Debugging info is REPORTED IN THE FUNCTION BY ITSELF.
		if(!CleanUpBinary(ReflectingTape, DO_BINARY_IMAGE_CLEAN_UP))
		{
			DPRINTF(LOG_INFO, "Binary Image enhancement failed - see above.");
			return 0;
		} 
		else if (!DO_BINARY_IMAGE_CLEAN_UP)
		{
			DPRINTF(LOG_INFO, "Did not attempt cleanup of targets.");
		} else {DPRINTF(LOG_INFO, "Cleaned up targets.");}
		
	//Using usable image to return 
		//Determine the wanted target: the central circle/the central circle and its strip-friend
		//If it worked, analyze the particle
		if(!GetWidestParticle(ReflectingTape, &widestParticleIndex)) 
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Widest particle identification failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			return 0;
		} else {DPRINTF(LOG_INFO, "Got widest target.");}
		if(!frcParticleAnalysis(ReflectingTape, widestParticleIndex, &Biggest))
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Widest particle analysis failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			return 0;
		} else {DPRINTF(LOG_INFO, "Analyzed center target.");}
		
	//Return the normalized center - points out which way to turn, accounting for camera's offset (if any)
		*targetCenterNormalized = Biggest.center_mass_x_normalized - CAMERA_OFFSET;
		DPRINTF(LOG_INFO, "Found target center.");
		/*
	//Returning colored binary image to user
		Rect R;
		R.top = 50;
		R.left = 50;
		R.height = 50;
		R.width = 50;
		imaqDrawShapeOnImage(ColoredBinaryImage, ReflectingTape, R, IMAQ_PAINT_INVERT, IMAQ_SHAPE_RECT, 0.0);
		if(!imaqCast(ColoredBinaryImage, ReflectingTape, type, NULL, -1))
				{
					int errCode = GetLastVisionError();
					DPRINTF(LOG_INFO, "Returning a colored binary image failed - errorcode %i", errCode);
					char *errString = GetVisionErrorText(errCode);
					DPRINTF(LOG_INFO, "errString= %s", errString);
					return 0;
				} else {DPRINTF(LOG_INFO, "Changed Image Type");}*/
		Point P;
		P.x = 1;
		P.y = 1;
		imaqCopyRect(ColoredBinaryImage, srcimage, IMAQ_NO_RECT, P); 
			
		return 1;
};

int ProcessImageForCircles(Image* srcimage, double* targetCenterNormalized)
{
	//Extract Luminance plane
		Image* ProcessedImage = frcCreateImage(IMAQ_IMAGE_U8);
		if(!imaqExtractColorPlanes(srcimage, IMAQ_HSL, NULL, NULL, ProcessedImage))
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Luminace Plane extraction failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			return 0;
		} else {DPRINTF(LOG_INFO, "Extracted Color Plane.");}
		
	//Transform Image for readability
		if (!imaqMathTransform(ProcessedImage, ProcessedImage, IMAQ_TRANSFORM_POWX, 0, 100000, 2, NULL))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "Transformation failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				return 0;
			} else {DPRINTF(LOG_INFO, "Transformed Image");} 
		
	//Identify circles, return center of largest
		CircleDescriptor CD;
		CD.minRadius = 1;
		CD.maxRadius = 200;
		int numCircles;
		double maxRadius = 0;
		int maxRadiusIndex;
		CircleMatch* CM = imaqDetectCircles(ProcessedImage, &CD, NULL, NULL, NULL, &numCircles);
		if(CM=NULL)
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Transformation failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			return 0;
		} else {DPRINTF(LOG_INFO, "Transformed Image");} 
		for (int i = 0; i < numCircles; ++i) 
			{
				if(CM[i].radius>maxRadius)
				{
					maxRadius = CM[i].radius;
					maxRadiusIndex = i;
				}
			}
		*targetCenterNormalized = (double) CM[maxRadiusIndex].position.x;
			
			
		return 1;
}

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

