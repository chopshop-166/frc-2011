/*******************************************************************************
*  Project   		: Framework
*  File Name  		: TaskTemplate.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Template source file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "DriveTask.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf166
{
	struct timespec tp;             // Time of snapshot
	double x,y,z;					// Joystick axis
	// Any values that need to be logged go here
	// <<CHANGEME>>
};

//  Memory Log
// <<CHANGEME>>
class DriveLog : public MemoryLog
{
public:
	DriveLog() : MemoryLog(
			sizeof(struct abuf166), DRIVE_TASK_CYCLE_TIME, "template",
			"Seconds,Nanoseconds,Elapsed Time,X,Y,Z\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~DriveLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(double,double,double);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int DriveLog::PutOne(double x, double y, double z)
{
	struct abuf166 *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf166 *)GetNextBuffer(sizeof(struct abuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->x = x;
		ob->y = y;
		ob->z = z;
		// Add any values to be logged here
		// <<CHANGEME>>
		return (sizeof(struct abuf166));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int DriveLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf166 *ab = (struct abuf166 *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%1.6f,%1.6f,%1.6f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->x,
			ab->y,
			ab->z
			// Add values here
			// <<CHANGEME>>
	);
	
	// Done
	return (sizeof(struct abuf166));
}

/**
 * Normalize all wheel speeds if the magnitude of any wheel is greater than 1.0.
 * Taken from RobotDrive
 */
void DriveTask::Normalize(double *wheelSpeeds)
{
	double maxMagnitude = fabs(wheelSpeeds[0]);
	INT32 i;
	for (i=1; i<4; i++)
	{
		double temp = fabs(wheelSpeeds[i]);
		if (maxMagnitude < temp) maxMagnitude = temp;
	}
	if (maxMagnitude > 1.0)
	{
		for (i=0; i<4; i++)
		{
			wheelSpeeds[i] = wheelSpeeds[i] / maxMagnitude;
		}
	}
}

// task constructor
DriveTask::DriveTask(void): m_maxOutput(1), syncGroup(0x80), fl(10), fr(5), bl(7), br(9)
{
	Start((char *)"166DriveTask", DRIVE_TASK_CYCLE_TIME);
	// ^^^ Rename those ^^^
	wheelSpeeds[0] = wheelSpeeds[1] = wheelSpeeds[2] = wheelSpeeds[3] = 0;
	// <<CHANGEME>>
	return;
};
	
// task destructor
DriveTask::~DriveTask(void)
{
	return;
};
	
// Main function of the task
int DriveTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	DriveLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Drive task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	// Register the proxy
	proxy = Proxy::getInstance();
	int curr_value=0;
	
    // General main loop (while in Autonomous or Tele mode)
	while (1) {
		if(proxy->get(DRIVER_AUTOASSIST)) {
			curr_value = (int)proxy->get("LineDirection");
			x=0;
			switch(curr_value) {
				case 2:
					y = 0;
					r = 0;
					break;
				case 1:
					y = 0;
					r = -AUTO_SPEED_TURN;
					break;
				case 0:
					y = AUTO_SPEED_FORWARD;
					r = 0;
					break;
				case -1:
					y = 0;
					r = AUTO_SPEED_TURN;
					break;
				case -2:
					break;
				default:
					r=y=0;
					break;
			}
		} else {
			x=proxy->get("Joy1X");
			y=proxy->get("Joy1Y");
			r=proxy->get("Joy1R");
		}
		
		wheelSpeeds[0] = x - y + r;
		wheelSpeeds[1] = -x - y - r;
		wheelSpeeds[2] = -x - y + r;
		wheelSpeeds[3] = x - y - r;
		
		Normalize(wheelSpeeds);
		
		fl.Set(wheelSpeeds[0]* m_maxOutput, syncGroup);
		fr.Set(-wheelSpeeds[1]* m_maxOutput, syncGroup);
		bl.Set(wheelSpeeds[2]* m_maxOutput, syncGroup);
		br.Set(-wheelSpeeds[3]* m_maxOutput, syncGroup);
		
		CANJaguar::UpdateSyncGroup(syncGroup);
		
        // Logging any values
		// <<CHANGEME>>
		// Make this match the declaration above
		sl.PutOne(x,y,r);
		
		// Wait for our next lap
		WaitForNextLoop();
	}
	return (0);
};
