/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: cameraServo.cpp
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Control a camera by moving the servos its mounted on
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "cameraServo.h"
// To locally enable debug printing: set true, to disable false
#define DPRINTF if(true)dprintf

// Sample in memory buffer
struct abuf166
{
	struct timespec tp;               // Time of snapshot
	// Any values that need to be logged go here
};

//  Memory Log
class CameraServoLog : public MemoryLog166
{
public:
	CameraServoLog() : MemoryLog166(
			sizeof(struct abuf166), CAMERA_SERVO_CYCLE_TIME, "cameraServo",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~CameraServoLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int CameraServoLog::PutOne(void)
{
	struct abuf166 *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf166 *)GetNextBuffer(sizeof(struct abuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		// Add any values to be logged here
		return (sizeof(struct abuf166));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int CameraServoLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf166 *ab = (struct abuf166 *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.))
			// Add values here
	);
	
	// Done
	return (sizeof(struct abuf166));
}


// task constructor
CameraServo::CameraServo(void): cameraX(CAMERA_PORT_X), cameraY(CAMERA_PORT_Y), camera(AxisCamera::GetInstance()),
	destimage(frcCreateImage(IMAQ_IMAGE_U8)),
	srcimage(frcCreateImage(IMAQ_IMAGE_RGB))
{
	Start((char *)"166CameraServos", CAMERA_SERVO_CYCLE_TIME);
	Hue_Range.minValue = 110;
	Hue_Range.maxValue = 130;
	Sat_Range.minValue = 25;
	Sat_Range.maxValue = 220;
	Lum_Range.minValue = 15;
	Lum_Range.maxValue = 130;
	camera.WriteResolution(AxisCamera::kResolution_320x240);
	camera.WriteCompression(20);
	camera.WriteBrightness(0);
	return;
};
	
// task destructor
CameraServo::~CameraServo(void)
{
	return;
};
	
// Main function of the task
int CameraServo::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	CameraServoLog sl;                   // log
#if 0
	unsigned timer = 0;					// Timer to only do certain things so often
	int largestParticleIndex = 0;
#endif
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Template task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	// Register the proxy
	proxy = Proxy::getInstance();
	float CamJoystickX = 0.0;
	float CamJoystickY = 0.0;
	float CamX = 0.5;
	float CamY = 0.5;
	#define DEADBAND (0.2)
	#define CAMMOVE (.5 / (1000/CAMERA_SERVO_CYCLE_TIME))
    // General main loop (while in Autonomous or Tele mode)
	while ((lHandle->RobotMode == T166_AUTONOMOUS) || 
	(lHandle->RobotMode == T166_OPERATOR)) {
		if(!proxy->get("Joy3BT")) {
			CamJoystickX = proxy->get("Joy3X");
			CamJoystickY = proxy->get("Joy3Y");
			if(CamJoystickX >= DEADBAND) {
				CamX += CAMMOVE;
			} else if(CamJoystickX <= -DEADBAND) {
				CamX -= CAMMOVE;
			}
			if(CamJoystickY >= DEADBAND) {
				CamY -= CAMMOVE;
			} else if(CamJoystickY <= -DEADBAND) {
				CamY += CAMMOVE;
			}
			if(CamX > 1.0) {
				CamX = 1.0;
			} else if(CamX < .0) {
				CamX = .0;
			}
			if(CamY > 1.0) {
				CamY = 1.0;
			} else if(CamY < .0) {
				CamY = .0;
			}
		} else {
			CamX = CamY = 0.5;
		}
#if 0
		if ((++timer) % (500/CAMERA_SERVO_CYCLE_TIME) == 0 && camera.IsFreshImage()) {
			timer = 0;
			camera.GetImage(srcimage);
			frcColorThreshold(destimage, srcimage, IMAQ_HSL, &Hue_Range, &Sat_Range, &Lum_Range);
			GetLargestParticle(destimage,&largestParticleIndex);
			if(largestParticleIndex != 0) {
				frcParticleAnalysis(destimage, largestParticleIndex, &Particle_Report);
				printf("\tLargest Area: %f\n", Particle_Report.particleArea);
			} else {
				printf("No particle found...\n");
			}
		}
#endif
        cameraX.Set(CamX);
		cameraY.Set(CamY);
		// Logging any values
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
