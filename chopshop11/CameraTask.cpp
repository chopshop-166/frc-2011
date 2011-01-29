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

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(true)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	// Any values that need to be logged go here
	// <<CHANGEME>>
};

//  Memory Log
class CameraLog : public MemoryLog
{
public:
	CameraLog() : MemoryLog(
			sizeof(struct abuf), CAMERA_CYCLE_TIME, "camera",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~CameraLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int CameraLog::PutOne(void)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		// Add any values to be logged here
		// <<CHANGEME>>
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
	fprintf(ofile, "%u,%u,%4.5f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.))
			// Add values here
			// <<CHANGEME>>
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
CameraTask::CameraTask(void):camera(AxisCamera::GetInstance())
{
	/* allow writing to vxWorks target */
	Priv_SetWriteFileAllowed(1);   
		
	this->MyTaskIsEssential=0;
	
	SetDebugFlag ( DEBUG_SCREEN_ONLY  ) ;
	camera.WriteResolution(AxisCamera::kResolution_320x240);
	//camera.WriteCompression(20);
	//camera.WriteBrightness(0);

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
	CameraLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_INFO,"In the 166 Camera task\n");
	
	WaitForGoAhead(); // THIS IS VERY IMPORTANT

	// Register our logger
	lHandle = Robot::getInstance();	
	lHandle->RegisterLogger(&sl);
	DPRINTF(LOG_INFO,"CameraTask registered logger");
	
	lHandle->DriverStationDisplay("Camera Task...");
	DPRINTF(LOG_INFO,"CameraTask informed DS");
	
	// Register the proxy
	proxy = Proxy::getInstance();
	DPRINTF(LOG_INFO,"CameraTask got proxy");
	
    // General main loop (while in Autonomous or Tele mode)
	while (1) {				
		// Wait for our next lap
		WaitForNextLoop();
		
		/* When this works, store a picture to cRIO */
		TakeSnapshot("cRIOimage.jpg");
				
		/* Look for target */
		FindTargets();

	    // Logging any values
		// Make this match the declaration above
		//sl.PutOne();
		
		Wait(10.0);
	}
	return (0);
	
};

void CameraTask::FindTargets()  {
	/* TBD  
	 * */
	lHandle->DriverStationDisplay("ProcessIMage:%0.6f",GetTime());

	// get the camera image
		HSLImage *image = camera.GetImage();

		// find FRC targets in the image
		vector<TargetCircle> targets = TargetCircle::FindCircularTargets(image);
		//delete image;
		if (targets.size() == 0 || targets[0].m_score < MINIMUM_SCORE)
		{
			// no targets found. Make sure the first one in the list is 0,0
			// since the dashboard program annotates the first target in green
			// and the others in magenta. With no qualified targets, they'll all
			// be magenta.
			TargetCircle nullTarget;
			nullTarget.m_majorRadius = 0.0;
			nullTarget.m_minorRadius = 0.0;
			nullTarget.m_score = 0.0;
			if (targets.size() == 0)
				targets.push_back(nullTarget);
			else
				targets.insert(targets.begin(), nullTarget);
			if (targets.size() == 0)
				DPRINTF(LOG_DEBUG, "No target found\n\n");
			else
				DPRINTF(LOG_DEBUG, "No valid targets found, best score: %f ", 
						targets[0].m_score);
		}
		else {
			// We have some targets.
			SaveImage("targetImage.jpg", (Image*)image);
			// set the new PID heading setpoint to the first target in the list
			double hAngle = targets[0].GetHorizontalAngle();
			double vAngle = targets[0].GetVerticalAngle();
			double size = targets[0].GetSize();
			
			// send dashbaord data for target tracking
			DPRINTF(LOG_DEBUG, "Target found %f ", targets[0].m_score);
			DPRINTF(LOG_DEBUG, "H: %f  V: %f  SIZE: %f ", hAngle, vAngle, size);
//			targets[0].Print();
		}
		delete image;
};

/**
 * Take a picture and store it to the cRIO in the specified path
 */
void CameraTask::TakeSnapshot(char* imageName)  {
	
	lHandle->DriverStationDisplay("storing %s",imageName);
	//DPRINTF(LOG_DEBUG, "taking a SNAPSHOT ");
	
	Image* cameraImage = frcCreateImage(IMAQ_IMAGE_HSL);
	if (!cameraImage) {
		DPRINTF (LOG_INFO,"frcCreateImage failed - errorcode %i",GetLastVisionError()); 
	}

	/* If there is an unacquired image to get, acquire it */
	if ( camera.IsFreshImage() ) {
		if ( !camera.GetImage(cameraImage) ) {
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
