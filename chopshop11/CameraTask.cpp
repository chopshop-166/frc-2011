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
	Start((char *)"CameraTask", CAMERA_CYCLE_TIME);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	this->MyTaskIsEssential=0;
	SetDebugFlag ( DEBUG_SCREEN_ONLY  ) ;
	camera.WriteResolution(AxisCamera::kResolution_320x240);
	//camera.WriteCompression(20);
	//camera.WriteBrightness(0);
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
		// <<CHANGEME>>
		// Insert your own logic here
		
        // Logging any values
		// <<CHANGEME>>
		// Make this match the declaration above
		//sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();
		
		/* When this works, store a picture to cRIO */
		//TakeSnapshot("testShot.jpg");
		
		DPRINTF(LOG_INFO,"CameraTask::Main loop...");
		
		/* Look for target */
		ProcessImage();
		Wait(10.0);
	}
	return (0);
	
};

void CameraTask::ProcessImage()  {
	/* TBD  
	 * */
	lHandle->DriverStationDisplay("ProcessIMage:%0.6f",GetTime());
};

void CameraTask::TakeSnapshot(char* imageName)  {
	/* Warning - getting a write error when this is tried 
	 * ERR_WRITE_FILE_NOT_SUPPORTED  -1074395313   
	 * */
	
	//lHandle->DriverStationDisplay("C:%0.6f",GetTime());
	lHandle->DriverStationDisplay("storing %s",imageName);
	DPRINTF(LOG_DEBUG, "taking a SNAPSHOT ");

	/* allow writing to vxWorks target */
	//Priv_SetWriteFileAllowed(1);   	
	
	Image* cameraImage = frcCreateImage(IMAQ_IMAGE_HSL);
	if (!cameraImage) {
		DPRINTF (LOG_INFO,"frcCreateImage failed - errorcode %i",GetLastVisionError()); 
	}
	
	if ( !camera.GetImage(cameraImage) ) {
		DPRINTF (LOG_INFO,"\nImage Acquisition from camera failed %i", GetLastVisionError());
	} else { 
		DPRINTF (LOG_DEBUG, "calling frcWriteImage for %s", imageName);
		if (!frcWriteImage(cameraImage, imageName) ) { 
			int errCode = GetLastVisionError();
			DPRINTF (LOG_INFO,"frcWriteImage failed - errorcode %i", errCode);
			char *errString = GetVisionErrorText(errCode);
			DPRINTF (LOG_INFO,"errString= %s", errString);
		} else { 
			DPRINTF (LOG_INFO,"\n>>>>> Saved image to %s", imageName);	
			// always dispose of image objects when done
			frcDispose(cameraImage);
		}
	}
};
