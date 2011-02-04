/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: CameraTask.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: This task initializes the camera and calls the 2011
*           targeting code. It offers a static fuction that any task can call 
*                   CameraTask::TakeSnapshot(char* imageName))
*			to save a picture to the cRIO.
*           Logged target data: size, location, and score (a
*                     quality metric)
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Robot.h"
#include "CameraTask.h"
#include "TargetCircle.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf
#define CAMERA_CENTER_OFFSET 0. // normalized, the offset of the centerline of the camera vs. the robot's centerline at the ideal target center

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	// Any values that need to be logged go here
	double targetHAngle;
	double targetVAngle;
	double targetSize;	
};

//  Memory Log
class CameraLog : public MemoryLog
{
public:
	CameraLog() : MemoryLog(
			sizeof(struct abuf), CAMERA_CYCLE_TIME, "camera",
			"Seconds,Nanoseconds,Elapsed Time,H-Angle, V-Angle, Size,\n" 
			) {
		return;
	};
	~CameraLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file

	unsigned int PutOne(double,double,double);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int CameraLog::PutOne(double hAngle,double vAngle,double size)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		// Add any values to be logged here

		ob-> targetHAngle = hAngle;
		ob-> targetVAngle = vAngle;
		ob-> targetSize = size;	
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int CameraLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f, %3.3f, %3.3f, %4.4f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			// Values to log
			ab-> targetHAngle, ab-> targetVAngle, ab-> targetSize		
	);
	
	// Done
	return (sizeof(struct abuf));
}

CameraTask *CameraTask::myHandle = NULL;

// task constructor
CameraTask::CameraTask(void):
		camera(AxisCamera::GetInstance()), 
		ReflectingTape(frcCreateImage(IMAQ_IMAGE_U8)), 
		srcimage(frcCreateImage(IMAQ_IMAGE_RGB))
{
	myHandle = this;
	
	/* allow writing to vxWorks target */
	Priv_SetWriteFileAllowed(1);   
		
	this->MyTaskIsEssential=0;
	
	SetDebugFlag ( DEBUG_SCREEN_ONLY  ) ;
	camera.WriteResolution(AxisCamera::kResolution_320x240);
	//camera.WriteCompression(20);
	//camera.WriteBrightness(0);
	R_Range.minValue = 100;
	R_Range.maxValue = 255;
	G_Range.minValue = 100;
	G_Range.maxValue = 255;
	B_Range.minValue = 100;
	B_Range.maxValue = 255;

	numParticles=0; 
	largestParticleIndex=0;
	widestParticleIndex=0;
	targetCenterNormalized=0;
	imageProcessResult=0;

	int fps = camera.GetMaxFPS();
	Start((char *)"CameraTask", CAMERA_CYCLE_TIME);

	DPRINTF(LOG_INFO,"CameraTask FPS=%i task cycle time=%i",fps,CAMERA_CYCLE_TIME);

	return;
};
	
// task destructor
CameraTask::~CameraTask(void)
{
	return;
};
	
// Main function of the task
int CameraTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	// Register our logger
	CameraLog sl;                   // log
	
	// Register the proxy
	proxy = Proxy::getInstance();
	DPRINTF(LOG_INFO,"CameraTask got proxy");
	
	
	// Let the world know we're in
	DPRINTF(LOG_INFO,"In the 166 Camera task\n");
	
	WaitForGoAhead(); // THIS IS VERY IMPORTANT
	
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	DPRINTF(LOG_INFO,"CameraTask registered logger");
	
	lHandle->DriverStationDisplay("Camera Task...");
	DPRINTF(LOG_INFO,"CameraTask informed DS");
	
	DPRINTF(LOG_INFO,"CameraTask informed DS");
	
    // General main loop (while in Autonomous or Tele mode)
	while (true) {				
		// Wait for our next lap
		WaitForNextLoop();
		
		/* Store a picture to cRIO */
		TakeSnapshot("cRIOimage.jpg");
				
		/* Look for target */
		bool found = FindTargets();
		
		/*if (imageProcessResult ==0)//nothing was found
		{
			
		}
		else if (imageProcessResult==1)//only the widest particle was found
		{
		
		}
		else if (imageProcessResult==2)// only the largest particle was found
		{

	    // Logging values if a valid target found
		if (found) {
			sl.PutOne(targetHAngle,targetVAngle,targetSize);
		}
		else if (imageProcessResult==3)//both large particle and wide particle were found*/
		
		// JUST FOR DEBUGGING - give us time to look at the screen
		// REMOVE THIS WAIT to go operational!
		Wait(10.0);
	}
	return (0);
	
};

bool CameraTask::FindTargets()  {
{
	*widestParticleIndex = 0; // points to caller-provided variable
	lHandle->DriverStationDisplay("ProcessImage:%0.6f",GetTime());

	// get the camera image
	//Image * image = frcCreateImage(IMAQ_IMAGE_HSL);
	HSLImage * image = camera.GetImage();
	//non-zeros are successes
	//0 = fail
	//when there's a failure, call imaqGetLastError() to get the extended error info
	if ( !success )	{  printf("Error: %d\n", imaqGetLastError()); return success;}			

	// find FRC targets in the image
	vector<TargetCircle> targets = TargetCircle::FindCircularTargets(image);
	
		if (targets.size()) {
			DPRINTF(LOG_DEBUG, "targetImage SCORE = %f", targets[0].m_score);
			/* do this in the target code
			Image *tmpImage;
			//if (frcCopyImage(tmpImage, (Image*)image) ) { 
			if (imaqCast(NULL, image, IMAQ_IMAGE_HSL,NULL, -1) ) { 
					int errCode = GetLastVisionError();
					DPRINTF (LOG_INFO,"frcCopyImage failed - errorcode %i", errCode);
					char *errString = GetVisionErrorText(errCode);
					DPRINTF (LOG_INFO,"errString= %s", errString);
				*widestParticleIndex = i;  // return index to caller
			}
		}
			}
			else {
				SaveImage("targetImage.jpg", tmpImage);
			} */
		}	
		//delete image;
		delete image;
		if (targets.size() == 0) {
			// no targets found.
			DPRINTF(LOG_DEBUG, "No target found\n\n");
			return false;			
			
		
		camera.GetImage(srcimage);
		//&ReflectingTape = srcimage->ThresholdRGB(115, 255, 153, 255, 128, 255);
		
		frcColorThreshold(ReflectingTape, srcimage, IMAQ_RGB, &R_Range, &G_Range, &B_Range);
		//numParticles = ReflectingTape->GetNumberParticles();
		//frcCountParticles(ReflectingTape, &numParticles);
		
		GetLargestParticle(ReflectingTape, &largestParticleIndex);
			if(largestParticleIndex != 0) {
				frcParticleAnalysis(ReflectingTape, largestParticleIndex, &Biggest);
				printf("\tLargest Area: %f\n", Biggest.particleArea);
				printf("\n\tLargest particle's index: %d\n", largestParticleIndex);
				returnValue = returnValue+2;
				//if only the largest particle is found, returnValue = 2
			} else {
				printf("No large particle found...\n");
		}
		else if (targets[0].m_score < MINIMUM_SCORE) {
			// no good enough targets found
			DPRINTF(LOG_DEBUG, "No valid targets found, best score: %f ", 
						targets[0].m_score);
			return false;			
				returnValue = returnValue+1;
				//if only the largest particle is found, returnValue = 1
			} else {
				printf("No wide particle found...\n");
		}
		else {
			// We have some targets.
			// set the new PID heading setpoint to the first target in the list
			targetHAngle = targets[0].GetHorizontalAngle();
			targetVAngle = targets[0].GetVerticalAngle();
			targetSize = targets[0].GetSize();

			// send dashboard data for target tracking
			DPRINTF(LOG_DEBUG, "Target found %f ", targets[0].m_score);
			DPRINTF(LOG_DEBUG, "H: %3.0f  V: %3.0f  SIZE: %3.3f ", 
					targetHAngle, targetVAngle, targetSize);
//			targets[0].Print();
		}
		return true;
	//if both worked, returnValue = 3

/**
 * Take a picture and store it to the cRIO in the specified path
 * Any task should be able to call this to take and save a snapshot
 */
};
void CameraTask::TakeSnapshot(char* imageName)  {
	/* Warning - getting a write error when this is tried 
	 * ERR_WRITE_FILE_NOT_SUPPORTED  -1074395313   
	 * */
	
	myHandle->lHandle->DriverStationDisplay("storing %s",imageName);
	//DPRINTF(LOG_DEBUG, "taking a SNAPSHOT ");
	
	Image* cameraImage = frcCreateImage(IMAQ_IMAGE_HSL);
	if (!cameraImage) {
		DPRINTF (LOG_INFO,"frcCreateImage failed - errorcode %i",GetLastVisionError()); 
	}

	/* If there is an unacquired image to get, acquire it */
	if ( myHandle->camera.IsFreshImage() ) {
		if ( !myHandle->camera.GetImage(cameraImage) ) {
			DPRINTF (LOG_INFO,"\nImage Acquisition from camera failed %i", GetLastVisionError());
		} else { 	
			SaveImage(imageName, cameraImage);
		  	// always dispose of image objects when done
		  	frcDispose(cameraImage);
		} 
	}
	else {
			DPRINTF (LOG_INFO,"Image is stale");	
	} // fresh
};

/**
 * Store an Image to the cRIO in the specified path
 */
void SaveImage(char* imageName, Image* image)  {	
	DPRINTF (LOG_DEBUG, "writing %s", imageName);
	if (!frcWriteImage(image, imageName) ) { 
		int errCode = GetLastVisionError();
		DPRINTF (LOG_INFO,"frcWriteImage failed - errorcode %i", errCode);
		char *errString = GetVisionErrorText(errCode);
		DPRINTF (LOG_INFO,"errString= %s", errString);
	} 
};

