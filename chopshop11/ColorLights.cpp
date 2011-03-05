/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: ColorLights.cpp
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to run the LEDs to tell the feeders the needed colors
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "ColorLights.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	float voltage;
};

//  Memory Log
// <<CHANGEME>>
class BatteryLog : public MemoryLog
{
public:
	BatteryLog() : MemoryLog(
			sizeof(struct abuf), COLORLIGHT_CYCLE_TIME, "battery",
			"Elapsed Time,Battery Voltage\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~BatteryLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(float);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int BatteryLog::PutOne(float voltage)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->voltage = voltage;
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int BatteryLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%4.5f,%2.6f\n",
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.))
			,ab->voltage
	);
	
	// Done
	return (sizeof(struct abuf));
}

// task constructor
ColorLightTask::ColorLightTask(void):Team166Task(0),
	Red(COLOR_PIN_RED), White(COLOR_PIN_WHITE), Blue(COLOR_PIN_BLUE)
{
	Start((char *)"166ColorLightsTask", COLORLIGHT_CYCLE_TIME);
	
	// Register the proxy
	proxy = Proxy::getInstance();
	
	
	return;
};
	
// task destructor
ColorLightTask::~ColorLightTask(void)
{
	return;
};
	
// Main function of the task
int ColorLightTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the Color Lights Task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	BatteryLog bl;
	
	// Register main robot task
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&bl);
    // General main loop (while in Autonomous or Tele mode)
	while (true) 
	{
		if(proxy->get(PRESET_TYPE_AXIS) == 0) {
			if(proxy->get(RED_LIGHT_BUTTON)) //red
			{
				Red.Set(true);
				White.Set(false);
				Blue.Set(false);
			}
			else if(proxy->get(WHITE_LIGHT_BUTTON)) //white
			{
				Red.Set(false);
				White.Set(true);
				Blue.Set(false);
			}
			else if(proxy->get(BLUE_LIGHT_BUTTON)) //blue
			{
				Red.Set(false);
				White.Set(false);
				Blue.Set(true);
			}
			else if(proxy->get(TURNOFF_LIGHTS_BUTTON)) //clear all
			{
				Red.Set(false);
				White.Set(false);
				Blue.Set(false);
			}
		}
		
		bl.PutOne(lHandle->dsHandle->GetBatteryVoltage());
		
		// Wait for our next lap
		WaitForNextLoop();
	}
	return (0);
};
