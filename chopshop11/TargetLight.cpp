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

//vision libraries to include
	#include "WPILib.h"
	#include "Robot.h"
	#include "CameraTask.h"
	#include "TargetCircle.h"
	#include "nivision.h"
	#include "VisionAPI.h"
	#include "TrackAPI.h"
	#include "stdlib.h"
#define CAMERA_OFFSET (0.00625)			//how offset the camera is when normalized
#define REGULAR_METHOD (1)
	#define CHECK_IMAGE (1) 
		#define HISTOGRAM_MAX_LUM (75.0)
	#define ISOLATE_NUM (3)	
		#define ISO1_LOWER_THRESH (200) //extract luminance plane, exp lookup, threshold
		#define ISO2_LOWER_THRESH (200) //extract blue plane, smooth, threshold
		#define ISO3_LOWER_THRESH (50)  //extract blue plane, smooth, exp lookup, threshold
		#define ISO4_LOWER_THRESH (50)  //extract luminance plane, smooth, exp lookup, threshold
	#define DO_BINARY_IMAGE_CLEAN_UP (2)
	#define IDENTIFY_NUM (0)
	#define DPRINTF if(false)dprintf 		//debugging info
	#define TPRINTF if(false)dprintf 		//testing info
#if REGULAR_METHOD
 /*******************************************************************************
PRECURSORY SETUP: ProcessTheImage() can do the following things:
	1. Test the image to see if a target is visible. 
	2. Examine image and find points of interest.
	3. Clean up the points of interest, leaving the target.
	4. Identify the target from the remain points of image, and return the
	   location of the target to the proxy.
	5. Report debugging/testing information.

There is more than one way to do each of those steps. The #defines choose which
methods are used. 

"Most of the vision functions return an integer value, 1 if the call was 
successful, and 0 if unsuccessful." - FIRST Robotics Competition (FRC)
Application Programming Interface (API) Specification For Machine Vision
*******************************************************************************/ 
// Step 1: Test the image to see if a target is visible.
		// CHECK_IMAGE: if 1, checks image with histogram report
/*******************************************************************************/ 
// Step 2: Examine image and find points of interest.
		//This step takes a source image and returns a binary image (an image with only
		//some regions 'on') with the points of interest 'on'.
		// ISOLATE_NUM determines how light color is found. 
			// 0 = HSL threshold
			// 1 = extract luminance plane, exp lookup, threshold
			// 2 = extract blue plane, smooth, threshold
			// 3 = 2, extract blue plane, smooth, exp lookup, threshold
			// 4 = 1, extract luminance plane, smooth, exp lookup, threshold

/*******************************************************************************/ 
//Step 3: Clean up the points of interest, leaving the target.
		//DO_BINARY_IMAGE_CLEAN_UP: if true (=1), image with points of interest is cleaned up

/*******************************************************************************/ 
//Step 4: Identify the target from the remain points of image, and return the
//location of the target to the proxy.
		//IDENTIFY_NUM determines how the computer picks the target
			// 0 = widest particle ('on' spot in image) is the target FUNCTIONAL
			// 1 = the particles that best match the pattern of the template are the target NONFUNCTIONAL
				//ISOLATE_NUM 3 should work best with IDENTIFY_NUM 1
				//IDENTIFY_NUM 1 requires a template image on the CRIO
				#if (IDENTIFY_NUM == 1)
				#define TEMPLATE_FILE_NAME "Template.png"
				#endif

/*******************************************************************************/ 
//Step 5: Report debugging/testing information.
/*******************************************************************************/ 
		// DPRINTF/TPRINTF: To locally enable console printing, set the variable true; to disable, false

/*******************************************************************************
METHODS OF ISOLATION: The intention of these functions is to take a color image
from the camera and to return it as a binary image with the target among the 
'on' pixels (value = 1 or more). 

The method used is determined by the value of ISOLATE_NUM. The numbers for 
ISOLATE_NUM are in chronological order, though the higher numbers tend to need
less manipulation of the threshold values to get the function to work.
*******************************************************************************/ 
#if (ISOLATE_NUM == 0)
int IsolateLightTarget(Image* ReflectingTape, Image* srcimage)
//return binary image from camera image with HSL threshold technique
{
	//Define the HSL acceptable range values 
		Range H_Range;
		Range S_Range;
		Range L_Range;
			H_Range.minValue = 0;
			H_Range.maxValue = 255;
			S_Range.minValue = 24;
			S_Range.maxValue = 255;
			L_Range.minValue = 183;	
			L_Range.maxValue = 255;
			
	//threshold source image to return binary image
		if(!frcColorThreshold(ReflectingTape, srcimage, 255, IMAQ_HSL, &H_Range, &S_Range, &L_Range)) {return 0;} 
	
	//let higher function know this worked
		return 1;
}
#endif
/*******************************************************************************/ 
#if (ISOLATE_NUM == 1)
int IsolateLightTarget(Image* ReflectingTape, Image* srcimage)
//return binary image from source image using the edited luminance plane
{
	//Get the luminance plane - targets should be light
		if(!imaqExtractColorPlanes(srcimage, IMAQ_HSL, NULL, NULL, ReflectingTape)) {return 0;}

	//exaggerate the dark/light contrast
		if(!imaqMathTransform(ReflectingTape, ReflectingTape, IMAQ_TRANSFORM_EXP, 0, 1000, 2, NULL)){return 0;}
			
	//threshold light/dark to return binary image
		if(!frcSimpleThreshold(ReflectingTape, ReflectingTape, ISO1_LOWER_THRESH, 255)){return 0;}
		
	//let higher function know this worked
		return 1;
}
#endif
/*******************************************************************************/ 
#if (ISOLATE_NUM == 2)
int IsolateLightTarget(Image* ReflectingTape, Image* srcimage)
//return binary image from source by thresholding the edited blue plane
{
	//Get the luminance plane from source image
		if(!imaqExtractColorPlanes(srcimage, IMAQ_RGB, NULL, NULL, ReflectingTape)) {return 0;}

	//Smooth out the image while eliminating small bits of light
		//create a 5x5 structuring element for smoothing function
			StructuringElement SE;
			SE.matrixCols = 3;
			SE.matrixRows = 3;
			SE.hexa = FALSE;
			SE.kernel = (int*) malloc(9 * sizeof(int));
			for(int i; i<9; ++i){SE.kernel[i] = 1;}
		//smooth image
			if(!imaqGrayMorphology(ReflectingTape, ReflectingTape, IMAQ_OPEN, &SE)){return 0;}
		
	//threshold light/dark to return binary image
		if(!frcSimpleThreshold(ReflectingTape, ReflectingTape, ISO2_LOWER_THRESH, 255)){return 0;}
		
	//let higher function know this worked
		imaqDispose(&SE);
		return 1;
}
#endif
/*******************************************************************************/ 
#if (ISOLATE_NUM == 3)
int IsolateLightTarget(Image* ReflectingTape, Image* srcimage)
//return binary image from source by thresholding the edited blue plane
//lower threshold for machine matching
{
	//Get the blue plane - targets should be light
		if(!imaqExtractColorPlanes(srcimage, IMAQ_RGB, NULL, NULL, ReflectingTape)) {return 0;}

	//Smooth out the image while eliminating small bits of light
		//create a 3x3 structuring element for smoothing function
			StructuringElement SE;
			SE.matrixCols = 7;
			SE.matrixRows = 7;
			SE.hexa = FALSE;
			SE.kernel = (int*) malloc(49 * sizeof(int));
			for(int i; i<49; ++i){SE.kernel[i] = 1;}
		//smooth
			if(!imaqGrayMorphology(ReflectingTape, ReflectingTape, IMAQ_OPEN, &SE)){return 0;}
		
	//exaggerate the dark/light contrast
		if(!imaqMathTransform(ReflectingTape, ReflectingTape, IMAQ_TRANSFORM_EXP, 0, 1000, 2, NULL)){return 0;}
		
	//threshold light/dark to return binary image
		if(!frcSimpleThreshold(ReflectingTape, ReflectingTape, ISO3_LOWER_THRESH, 255)){return 0;}

	//let higher function know this worked
		imaqDispose(&SE);
		return 1;
}
#endif
/*******************************************************************************/
#if (ISOLATE_NUM == 4)
int IsolateLightTarget(Image* ReflectingTape, Image* srcimage)
//return binary image from source image using the very edited luminance plane
{
	//Get the luminance plane - targets should be light
		if(!imaqExtractColorPlanes(srcimage, IMAQ_HSL, NULL, NULL, ReflectingTape)) {return 0;}
	
	//Smooth out the image while eliminating small bits of light
		//create a 3x3 structuring element for smoothing function
			StructuringElement SE;
			SE.matrixCols = 3;
			SE.matrixRows = 3;
			SE.hexa = FALSE;
			SE.kernel = (int*) malloc(9 * sizeof(int));
			for(int i; i<9; ++i){SE.kernel[i] = 1;}
		//smooth
			if(!imaqGrayMorphology(ReflectingTape, ReflectingTape, IMAQ_OPEN, &SE)){return 0;}
				
	//exaggerate the dark/light contrast
		if(!imaqMathTransform(ReflectingTape, ReflectingTape, IMAQ_TRANSFORM_EXP, 0, 1000, 2, NULL)){return 0;}
			
	//threshold light/dark to return binary image
		if(!frcSimpleThreshold(ReflectingTape, ReflectingTape, ISO4_LOWER_THRESH, 255)){return 0;}
		
	//let higher function know this worked
		imaqDispose(&SE);
		return 1;
}
#endif
/*******************************************************************************
CLEANING THE IMAGE: Often, the binary image contains small specks or lines from
bright objects beside the target. The objective of this function is to clean
off irrelevant information and to accentuate the target.

This method can be turned off by changing DO_BINARY_IMAGE_CLEAN_UP to false.
*******************************************************************************/ 
#if (DO_BINARY_IMAGE_CLEAN_UP > 0)
int CleanUpBinary(Image* ReflectingTape)
//Change the fed image to a cleaner image
{
	//Elimiate small particles, smooth out large particles
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
		//perform manipulations to (hopefully) enhance the returned binary image
			//IMAQ_OPEN: An erosion followed by a dilation. An opening removes small objects and
			//smooths boundaries of objects in the image.
				//if(!imaqGrayMorphology(ReflectingTape, ReflectingTape, IMAQ_OPEN, &myStructuringElement)){DPRINTF(LOG_INFO, "Unopened");return 0;}
			//IMAQ_ERODE: Reduces the size of an object along its boundary and eliminates isolated
			//points in the image.
				//if(!imaqGrayMorphology(ReflectingTape, ReflectingTape, IMAQ_ERODE, &myStructuringElement)){DPRINTF(LOG_INFO, "Uneroded");return 0;}
			//IMAQ_CLOSE: A dilation followed by an erosion. A closing fills small holes in objects and
			//smooths the boundaries of objects.
				if(!imaqGrayMorphology(ReflectingTape, ReflectingTape, IMAQ_CLOSE, &myStructuringElement)){DPRINTF(LOG_INFO, "Unclosed");return 0;}
			//IMAQ_AUTOM: A function that uses dual combinations of opening and closing operations
			//to smooth the boundaries of objects.
				//if(!imaqGrayMorphology(ReflectingTape, ReflectingTape, IMAQ_AUTOM, &myStructuringElement)){return 0;}
				
#if (DO_BINARY_IMAGE_CLEAN_UP == 1)
		//filter out small particles
			//set up particle filter criteria for size (eliminate small particles)
				ParticleFilterCriteria2 PFC[1];
				PFC[0].parameter = IMAQ_MT_AREA;
				PFC[0].lower = 75;
				PFC[0].upper = 0xFFFFFF;
				PFC[0].calibrated = false;
				PFC[0].exclude = false;
				ParticleFilterOptions PFO;
				PFO.rejectMatches = false;
				PFO.rejectBorder = false;
				PFO.connectivity8 = false;
			//Do particle filtering by above criteria
				if(!imaqParticleFilter3(ReflectingTape, ReflectingTape, &PFC[0], 1, &PFO, NULL, NULL)) {return 0;}
#endif
				
#if (DO_BINARY_IMAGE_CLEAN_UP == 2)
	//filter out small particles
		//set up particle filter criteria for size (eliminate small particles)
			ParticleFilterCriteria2 PFC[2];
			PFC[0].parameter = IMAQ_MT_AREA;
			PFC[0].lower = 75;
			PFC[0].upper = 0xFFFFFF;
			PFC[0].calibrated = false;
			PFC[0].exclude = false;
			PFC[1].parameter = IMAQ_MT_CENTER_OF_MASS_X;
			PFC[1].lower = 80;
			PFC[1].upper = 240;
			PFC[1].calibrated = false;
			PFC[1].exclude = false;
			ParticleFilterOptions PFO;
			PFO.rejectMatches = false;
			PFO.rejectBorder = false;
			PFO.connectivity8 = false;
		//Do particle filtering by above criteria
			if(!imaqParticleFilter3(ReflectingTape, ReflectingTape, &PFC[0], 2, &PFO, NULL, NULL)) {return 0;}
#endif
	
	//change any non-zero pixel values to 255, which is white in Python script's returned images
		//create a lookup table (lookup function take value of pixel and replaces the pixel's value
		//with the value of table[value of pixel]
			short int table[256];
			for (int i = 0; i<256; ++i){table[i] = 255;} table[0]=0;
		//replace values with lookup function
			imaqLookup(ReflectingTape, ReflectingTape, &table[0], NULL); 

	imaqDispose(&myStructuringElement);
	return 1;
}
#endif

/*******************************************************************************
METHODS OF IDENTIFICATION: The intention of these functions is to look at the 
particles ('on'-spots) in a binary image and to decide which particle is the
real target.

The method used is determined by the value of IDENTIFY_NUM. The numbers for 
ISOLATE_NUM are in chronological order.
*******************************************************************************/ 
/*bool MyInArea(Image* binaryImage, int* particleIndex)
//determines if a particle is roughly in the preferred area. 
{
	double px;
	double py;
	imaqMeasureParticle(binaryImage, particleIndex, 0, 
				IMAQ_MT_CENTER_OF_MASS_X, &px);
	imaqMeasureParticle(binaryImage, particleIndex, 0, 
					IMAQ_MT_CENTER_OF_MASS_Y, &py);
}*/
#if (IDENTIFY_NUM == 0)
int FindTargets(Image* binaryImage, double* targetCenterNormalized, bool* CanSeeTargets)
// return the center of the widest particle
	{
		//variables
			int numParticles;
			int widestParticleIndex;
			ParticleAnalysisReport Target;
			double widestParticleWidth = 0;
			double particleWidth;
		
		//count the number of particles in the image
			int success = frcCountParticles(binaryImage, &numParticles);	
			if ( !success )	{  printf("Error: %d\n", imaqGetLastError()); return success;}			
		
		// if no particles found we quit here
			if (numParticles == 0)  
			{  
				DPRINTF("No widest particle found...\n"); 
				*CanSeeTargets = false;
				return 1;
			} 
		
		//This function is WPI's, but it searches for the widest particle's index number instead of largest
			for (int i = 0; i < numParticles; ++i) 
			{		
				success = imaqMeasureParticle(binaryImage, i, 0, IMAQ_MT_BOUNDING_RECT_WIDTH, &particleWidth);
				if ( !success )	{ return success; }		
				if (particleWidth > widestParticleWidth) 	{
					// see if is in the right area
					if (InArea(binaryImage, i, IMAQ_NO_RECT) ) {
						widestParticleWidth = particleWidth;
						widestParticleIndex = i;  // return index to caller
					}
				}
			}

		//analyze the particle to get a ParticleAnalysisReport, which has desired info
			if(!frcParticleAnalysis(binaryImage, widestParticleIndex, &Target))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "Target particle analysis failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				*CanSeeTargets = false;
				return 0;
			} else {DPRINTF(LOG_INFO, "Analyzed center target.");}
			
		//return the target's location (normalized) to caller
			*targetCenterNormalized = Target.center_mass_x_normalized - CAMERA_OFFSET;
			*CanSeeTargets = true;
		
		return success;
	};
#endif
/*******************************************************************************/ 
#if (IDENTIFY_NUM == 1)
int FindTargets(Image* binaryImage, double* targetCenterNormalized, bool* CanSeeTargets)
//find the targets with a pattern-matching algorithm
{
	//setting up template image
		//make the image pattern
			static Image* pattern = frcCreateImage(IMAQ_IMAGE_U8);
		//make pattern the same image as the one in the CRIO
			if(!imaqReadFile(pattern, TEMPLATE_FILE_NAME, NULL, NULL))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "Template file reading failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				*CanSeeTargets = false;
				return 0;
			} else {DPRINTF(LOG_INFO, "Read template.");}
		/*//setting up LPAO - using defaults
			LearnPatternAdvancedOptions LPAO;
			LearnPatternAdvancedShiftOptions LPASO;
			LearnPatternAdvancedRotationOptions LPARO;
			LPARO.searchStrategySupport = IMAQ_AGGRESSIVE;
			LPAO.shiftOptions = &LPASO;
			LPAO.rotationOptions = &LPARO;
		//learn the pattern (no idea what this does)
			if(!imaqLearnPattern2(pattern, IMAQ_LEARN_SHIFT_INFORMATION, &LPAO))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "Template learning failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				*CanSeeTargets = false;
				return 0;
			} else {DPRINTF(LOG_INFO, "Learned template.");}*/
			if(!imaqLearnPattern2(pattern, IMAQ_LEARN_ALL, NULL))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "Template file reading failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				*CanSeeTargets = false;
				return 0;
			}
	
	//Try matching the pattern to the image
		//holder for number of matches
			int numMatches;
		//Setting up how the pattern matching method works (much is default value)
			MatchPatternOptions MPO;
			MPO.mode = IMAQ_MATCH_SHIFT_INVARIANT;
			MPO.minContrast = 15;
			MPO.subpixelAccuracy = false;
				RotationAngleRange RAR;
				RAR.lower = -10;
				RAR.upper = 10;
			MPO.angleRanges = &RAR;
			MPO.numRanges = 1;
			MPO.numMatchesRequested = 1;
			MPO.matchFactor = 0;
			MPO.minMatchScore = 300;
		/*//setting up MPAO DOES NOT SEEM TO BE NEEDED
			MatchPatternAdvancedOptions MPAO;
			MPAO.subpixelIterations = 20; //default
			MPAO.subpixelTolerance = 0; //default
			MPAO.initialMatchListLength = 5; //arbitrary
			MPAO.matchListReductionFactor = 5; //default
			MPAO.initialStepSize = 0; //default
			MPAO.searchStrategy = IMAQ_VERY_AGGRESSIVE;//arbitrary
			MPAO.intermediateAngularAccuracy = 3; //arbitrary*/
		//match the pattern to the image
			PatternMatch* PM = imaqMatchPattern2(binaryImage, pattern, &MPO, &MPO, IMAQ_NO_RECT, &numMatches);
			

	//examine pattern matching results to decide which is the right pattern match
		//index of the best pattern match variable
			int index;
		//if no matches, stop
			if(numMatches == 0) 
			{
				*CanSeeTargets = false;
				return 1;
			}
		//if there's more than 1 match, get the best pattern match's index
			else if(numMatches > 1)
			{
				float bestScore = 0;
				//get the index of the best pattern match
					for (int i = 0; i < numMatches; ++i) 
					{
						if (PM[i].score > bestScore) 	
							{
								bestScore = PM[i].score;
								index = i;
							}
					}
			}
		//only 1 match, so index is 1
			else if(numMatches ==1)
			{
				index = 1;
			}
	
	//return the information from the best pattern match, alert higher function this worked
		*targetCenterNormalized = (PM[index].position.x/160) - CAMERA_OFFSET;
		*CanSeeTargets = true;
		//save memory by deleting pattern match reports
			imaqDispose(PM);
		return 1;
	
}
#endif


/*******************************************************************************
CALLED FUNCTION: For the most part, this just calls the above functions to 
process the image and return the location as a number from -1 to 1, with 1 to 
the right.

Additionally, it returns a "ColoredBinaryImage", which is the binary image the
computer sees at the end. It is used with a Python script that lets the user see
what the computer sees. The bool "ImageReturned" says whether an image was 
returned. 
*******************************************************************************/ 
int ProcessTheImage(Image* srcimage, double* targetCenterNormalized, Image* ColoredBinaryImage, bool* CanSeeTargets, bool* ImageReturned, bool SaveImages)  
//Processes the image, but also returns a picture for a snapshot
{
	//Defining the needed variables
		Image* ReflectingTape = frcCreateImage(IMAQ_IMAGE_U8);

#if (CHECK_IMAGE == 1)
	//Test image to make sure it has targets: histogram mean is lower when there are very bright points in image/the image is dark		
	/*//get blue plane histogram
		if(!imaqExtractColorPlanes(srcimage, IMAQ_RGB, NULL, NULL, ReflectingTape)) {return 0;}
		HistogramReport* HR = imaqHistogram(ReflectingTape, 1, 0, 999999, NULL);
		TPRINTF(LOG_INFO, "Blue stdDev=%f", HR->stdDev);
		TPRINTF(LOG_INFO, "Blue mean=%f", HR->mean);*/
	//get luminance plane 
		if(!imaqExtractColorPlanes(srcimage, IMAQ_HSL, NULL, NULL, ReflectingTape)) {return 0;}
		HistogramReport* HR2 = imaqHistogram(ReflectingTape, 1, 0, 999999, NULL);
		TPRINTF(LOG_INFO, "Lum stdDev=%f", HR2->stdDev);
		TPRINTF(LOG_INFO, "Lum mean=%f", HR2->mean);
	//logic based on image clarity
		if(HR2->mean > HISTOGRAM_MAX_LUM)
		{
			TPRINTF(LOG_INFO, "Not suitable image.");
			DPRINTF(LOG_INFO, "Not suitable image.");
			*CanSeeTargets = false;
			return 1;
		} 
		else
		{
			TPRINTF(LOG_INFO, "Suitable image.");
			DPRINTF(LOG_INFO, "Suitable image.");
		}
#endif
		
	//Isolate target with threshold
		if(!IsolateLightTarget(ReflectingTape, srcimage))
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Alternative Isolation of targets to binary failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			*CanSeeTargets = false;
			return 0;
		} else {DPRINTF(LOG_INFO, "Isolated targets by alternate.");}

#if (DO_BINARY_IMAGE_CLEAN_UP == 1)
	//Make the binary image more readable for analysis
		//Debugging info is REPORTED IN THE FUNCTION BY ITSELF.
		if(!CleanUpBinary(ReflectingTape))
		{
			DPRINTF(LOG_INFO, "Binary Image enhancement failed - see above.");
			*CanSeeTargets = false;
			return 0;
		} 
		else {DPRINTF(LOG_INFO, "Cleaned up targets.");}
#endif
		
	//Duplicate the image for testing if desired
		if(SaveImages)
		{
			if(!imaqDuplicate(ColoredBinaryImage, ReflectingTape))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "Duplication failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				*ImageReturned = false;
			} else { DPRINTF(LOG_INFO, "Duplicated Image"); *ImageReturned = true;}
		}
		else
		{
			*ImageReturned = false;
		}
		
	//Using usable image to return wide particle (target) info
		//Determine the wanted target: the central circle/the central circle and its strip-friend
		//If it worked, analyze the particle
		if(!FindTargets(ReflectingTape, targetCenterNormalized, CanSeeTargets)) 
		{
			int errCode = GetLastVisionError();
			DPRINTF(LOG_INFO, "Target identification failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF(LOG_INFO, "errString= %s", errString);
			*CanSeeTargets = false;
			return 0;
		} else {DPRINTF(LOG_INFO, "Got target.");}

		return 1;
};
#endif

/*
						_____	   _____
					   /     \    /     \
					  /       \  /       \
					 |         \/         |
					 |                    |
					 |      M  hates      |
					  \    commenting.   /
					   \       :        /
						\	   :       /
						 \    A lot.  /
						  \          /
						   \        /
						    \      /
						     \    /
						      \  /
						       \/



 */
#if !REGULAR_METHOD
int ProcessTheImage(Image* srcimage, double* targetCenterNormalized, Image* ColoredBinaryImage, bool* CanSeeTargets, bool* ImageReturned, bool SaveImages)  
//Processes the image, but also returns a picture for a snapshot
{
	//setting up template image
		//make the image pattern
			static Image* pattern = frcCreateImage(IMAQ_IMAGE_U8);
		//make pattern the same image as the one in the CRIO
			if(!imaqReadFile(pattern, TEMPLATE_FILE_NAME, NULL, NULL))
			{
				int errCode = GetLastVisionError();
				DPRINTF(LOG_INFO, "Template file reading failed - errorcode %i", errCode);
				char *errString = GetVisionErrorText(errCode);
				DPRINTF(LOG_INFO, "errString= %s", errString);
				*CanSeeTargets = false;
				return 0;
			} else {DPRINTF(LOG_INFO, "Read template.");}
		//learning the pattern
			ColorInformation* CI = imaqLearnColor(pattern, NULL, IMAQ_SENSITIVITY_LOW, 64);
			int* idunno = imaqMatchColor(srcimage, CI, NULL, NULL);
	//checking for matches
		
			
	*SaveImages = false;
	return 1;
}
#endif



