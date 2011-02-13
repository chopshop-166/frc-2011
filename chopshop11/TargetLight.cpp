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

#include "WPILib.h"
#include "Robot.h"
#include "CameraTask.h"
#include "TargetCircle.h"
#include "nivision.h"
#include "VisionAPI.h"
#include "TrackAPI.h"
#include "stdlib.h"

#define CAMERA_OFFSET (0.00625)
#define DO_BINARY_IMAGE_CLEAN_UP (true)
#define ISOLATE_NUM (2)
// ISOLATE_NUM determines which method is loaded onto the processor:
// 0 = use HSL threshold
// 1 = extract luminance plane, threshold
// 2 = extract blue plane, threshold

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf
// to enable testing printing
#define TPRINTF if(false)dprintf
//"Most of the vision functions return an integer value, 1 if the call was successful, and 0 if unsuccessful."


//Converts the camera image by HSL threshold into a binary image with targets highlighted
//Debugging info is reported where function is called
#if (ISOLATE_NUM == 0)
int IsolateLightTargetHSL(Image* ReflectingTape, Image* srcimage)
	{
	//Create binary image with threshold technique
		Range H_Range;
		Range S_Range;
		Range L_Range;
		//Define the HSL minimum values 
			H_Range.minValue = 0;
			H_Range.maxValue = 255;
			S_Range.minValue = 24;
			S_Range.maxValue = 255;
			L_Range.minValue = 183;	
			L_Range.maxValue = 255;
		if(!frcColorThreshold(ReflectingTape, srcimage, 255, IMAQ_HSL, &H_Range, &S_Range, &L_Range)) {return 0;} 
		return 1;
	}
#endif

#if (ISOLATE_NUM == 1)
int IsolationAlternative(Image* ReflectingTape, Image* srcimage)
{
	//Get the luminance plane - targets should be light
		if(!imaqExtractColorPlanes(srcimage, IMAQ_HSL, NULL, NULL, ReflectingTape)) {return 0;}
		
	//exaggerate the dark/light contrast
		/*short int table[256];
		for (int i = 0; i<256; ++i){table[i] = i*i;} table[0]=0;
		if(!imaqLookup(ReflectingTape, ReflectingTape, &table[0], NULL)){DPRINTF(LOG_INFO, "AltIso Lookup failed"); return 0;} 
		*/
		
	//threshold light/dark
		if(!frcSimpleThreshold(ReflectingTape, ReflectingTape, 200, 255)){return 0;}
		
		return 1;
}
#endif
#if (ISOLATE_NUM == 2)
int IsolationAlternative2(Image* ReflectingTape, Image* srcimage)
{
	//Get the luminance plane - targets should be light
		if(!imaqExtractColorPlanes(srcimage, IMAQ_RGB, NULL, NULL, ReflectingTape)) {return 0;}

	//Smooth out the image while eliminating small bits of light
		//create a 5x5 structuring element
			StructuringElement SE;
			SE.matrixCols = 3;
			SE.matrixRows = 3;
			SE.hexa = FALSE;
			SE.kernel = (int*) malloc(9 * sizeof(int));
			for(int i; i<9; ++i){SE.kernel[i] = 1;}
		if(!imaqGrayMorphology(ReflectingTape, ReflectingTape, IMAQ_OPEN, &SE)){return 0;}
		
	//exaggerate the dark/light contrast
		if(!imaqMathTransform(ReflectingTape, ReflectingTape, IMAQ_TRANSFORM_EXP, 0, 1000, 2, NULL)){return 0;}
		/*short int table[256];
		for (int i = 0; i<256; ++i)
		{
			table[i]=((1/255)*(i*i));
		} 
		table[0]=0;
		if(!imaqLookup(ReflectingTape, ReflectingTape, &table[0], NULL)){DPRINTF(LOG_INFO, "AltIso Lookup failed"); return 0;} 
		*/
	//threshold light/dark
		if(!frcSimpleThreshold(ReflectingTape, ReflectingTape, 220, 255)){return 0;}
		
		return 1;
}
#endif


//Edit the binary image that has targets highlighted for readability
//Debugging info IS REPORTED IN THE FUNCTION
#if DO_BINARY_IMAGE_CLEAN_UP
int CleanUpBinary(Image* ReflectingTape, bool AttemptCleanUp)
{
	if (AttemptCleanUp)
	{

		//Set up a structuring element for functions to use
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
		//perform a series of manipulations to (hopefully) enhance the returned binary image
			//if(!imaqGrayMorphology(ReflectingTape, ReflectingTape, IMAQ_OPEN, &myStructuringElement)){DPRINTF(LOG_INFO, "Unopened");return 0;}
			//if(!imaqGrayMorphology(ReflectingTape, ReflectingTape, IMAQ_ERODE, &myStructuringElement)){DPRINTF(LOG_INFO, "Uneroded");return 0;}
			if(!imaqGrayMorphology(ReflectingTape, ReflectingTape, IMAQ_CLOSE, &myStructuringElement)){DPRINTF(LOG_INFO, "Unclosed");return 0;}
			//if(!imaqGrayMorphology(ReflectingTape, ReflectingTape, IMAQ_AUTOM, &myStructuringElement)){return 0;}
			/*
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
			}*/
	
		/*//Fill holes in the particles - MAY BE UNNECESSARY
			if(!imaqFillHoles(ReflectingTape, ReflectingTape, true)) 
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "Filling of holes in binary image failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				return 0;
			}*/
/*
		//Filter out particles by size
			if(!imaqSizeFilter(ReflectingTape, ReflectingTape, 1, 0, IMAQ_KEEP_LARGE, NULL)) 
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "Elimination of small particles in binary image failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				return 0;
			}
*/
		/*
		//Convex the hull
			imaqLabel2(ReflectingTape, ReflectingTape, true, NULL);
			imaqConvexHull(ReflectingTape, ReflectingTape, true); 
*/
		//set up particle filter criteria for size (eliminate small particles)
			ParticleFilterCriteria2 PFC[1];
			PFC[0].parameter = IMAQ_MT_AREA;
			PFC[0].lower = 50;
			PFC[0].upper = 1000;
			PFC[0].calibrated = false;
			PFC[0].exclude = false;
			ParticleFilterOptions PFO;
			PFO.rejectMatches = false;
			PFO.rejectBorder = false;
			PFO.connectivity8 = false;
		
		//Do particle filtering by above criteria
			if(!imaqParticleFilter3(ReflectingTape, ReflectingTape, &PFC[0], 1, &PFO, NULL, NULL)) {return 0;}
		
		//change any non-zero pixel values to 255, which is white in Python script's returned images
			short int table[256];
			for (int i = 0; i<256; ++i){table[i] = 255;} table[0]=0;
			imaqLookup(ReflectingTape, ReflectingTape, &table[0], NULL); 


	}
	
	return 1;
}
#endif

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

//Processes the image, but also returns a picture for a snapshot
int ProcessTheImage(Image* srcimage, double* targetCenterNormalized, Image* ColoredBinaryImage, bool* CanSeeTargets, bool* ImageReturned)  
{
	//Defining the needed variables
		Image* ReflectingTape = frcCreateImage(IMAQ_IMAGE_U8);
		int widestParticleIndex = 0;
		ParticleAnalysisReport Biggest;
		
	//Test image to make sure it has targets: histogram mean is lower when there are very bright points in image/the image is dark
		if(!imaqExtractColorPlanes(srcimage, IMAQ_HSL, NULL, NULL, ReflectingTape)) {return 0;}
		HistogramReport* HR = imaqHistogram(ReflectingTape, 1, 0, 999999, NULL);
		SmartDashboard::Log(HR->mean, "Histogram Mean");
		if(!imaqExtractColorPlanes(srcimage, IMAQ_RGB, NULL, NULL, ReflectingTape)) {return 0;}
		HistogramReport* HR2 = imaqHistogram(ReflectingTape, 1, 0, 999999, NULL);
		TPRINTF(LOG_INFO, "Histomean = %f, Histomean2 = %f", HR->mean, HR2->mean);
		if(HR->mean > 93){*CanSeeTargets = false; *ImageReturned = false; TPRINTF(LOG_INFO, "Image not Suitable"); return 1;}
		imaqDispose(HR);
		
	//Isolate target with threshold
#if (ISOLATE_NUM == 0)
		if(!IsolateLightTargetHSL(ReflectingTape, srcimage))
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "HSL Isolation of targets to binary failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			*CanSeeTargets = false;
			return 0;
		} else {DPRINTF(LOG_INFO, "Isolated targets by HSL.");}
#endif
		
	//Secondary Isolation Method
#if (ISOLATE_NUM == 1)
		if(!IsolationAlternative(ReflectingTape, srcimage))
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Alternative Isolation of targets to binary failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			*CanSeeTargets = false;
			return 0;
		} else {DPRINTF(LOG_INFO, "Isolated targets by alternate.");}
#endif
		
#if (ISOLATE_NUM == 2)
		if(!IsolationAlternative2(ReflectingTape, srcimage))
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Alternative Isolation of targets to binary failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			*CanSeeTargets = false;
			return 0;
		} else {DPRINTF(LOG_INFO, "Isolated targets by alternate.");}
#endif

#if DO_BINARY_IMAGE_CLEAN_UP
	//Make the binary image more readable for analysis
		//Debugging info is REPORTED IN THE FUNCTION BY ITSELF.
		if(!CleanUpBinary(ReflectingTape, DO_BINARY_IMAGE_CLEAN_UP))
		{
			DPRINTF(LOG_INFO, "Binary Image enhancement failed - see above.");
			*CanSeeTargets = false;
			return 0;
		} 
		else if (!DO_BINARY_IMAGE_CLEAN_UP)
		{
			DPRINTF(LOG_INFO, "Did not attempt cleanup of targets.");
		} else {DPRINTF(LOG_INFO, "Cleaned up targets.");}
#endif
		
	//Duplicate the image for testing
		if(!imaqDuplicate(ColoredBinaryImage, ReflectingTape))
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Duplication failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			*ImageReturned = false;
		} else { DPRINTF(LOG_INFO, "Duplicated Image"); *ImageReturned = true;}
		
	//Using usable image to return wide particle (target) info
		//Determine the wanted target: the central circle/the central circle and its strip-friend
		//If it worked, analyze the particle
		if(!GetWidestParticle(ReflectingTape, &widestParticleIndex)) 
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Widest particle identification failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			*CanSeeTargets = false;
			return 0;
		} else {DPRINTF(LOG_INFO, "Got widest target.");}
		if(!frcParticleAnalysis(ReflectingTape, widestParticleIndex, &Biggest))
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Widest particle analysis failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			*CanSeeTargets = false;
			return 0;
		} else {DPRINTF(LOG_INFO, "Analyzed center target.");}
		
	//Return the normalized center - points out which way to turn, accounting for camera's offset (if any)
		*targetCenterNormalized = Biggest.center_mass_x_normalized - CAMERA_OFFSET;
		DPRINTF(LOG_INFO, "Widest Particle width = %d", Biggest.boundingRect.width);
		DPRINTF(LOG_INFO, "Found target center.");
		
		*CanSeeTargets = true;
		return 1;
};
