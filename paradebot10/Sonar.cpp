/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: Lift166.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Robot code which controls the banner sensors
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Sonar.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(true)dprintf

// Sample in memory buffer
struct abuf166
{
	struct timespec tp;               // Time of snapshot
	// Any values that need to be logged go here
};

//  Memory Log
class SonarLog : public MemoryLog166
{
public:
	SonarLog() : MemoryLog166(
			sizeof(struct abuf166), SONAR_CYCLE_TIME, "sonar",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~SonarLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int SonarLog::PutOne(void)
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
unsigned int SonarLog::DumpBuffer(char *nptr, FILE *ofile)
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
Sonar166::Sonar166(void): US(T166_ANALOG_MODULE_1,T166_US), IR(T166_ANALOG_MODULE_1,T166_IR)
{
	Start((char *)"166SonarTask", SONAR_CYCLE_TIME);
	// ^^^ Rename those ^^^
	return;
};
	
// task destructor
Sonar166::~Sonar166(void)
{
	return;
};
	
// Main function of the task
int Sonar166::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	SonarLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Sonar task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	// Register the proxy
	proxy = Proxy::getInstance();
	
	//Original voltage
	float orig_voltage_us = 0;
	float orig_voltage_ir = 0;
	//Distance in inches
	float cur_distance_us = 0;
	unsigned int index = 0;
	float distances[20];
	float average = 0;
	
	// Distance Multiplier for US sensor
	#define T166_MV_TO_IN (9.8)
    // General main loop (while in Autonomous or Tele mode)
	while ((lHandle->RobotMode == T166_AUTONOMOUS) || 
			(lHandle->RobotMode == T166_OPERATOR)) {
		//Get Voltage from US sensor
		orig_voltage_us = US.GetVoltage();
		orig_voltage_ir = IR.GetVoltage();
		//lHandle->DriverStationDisplay("Voltage: %f", orig_voltage_ir);
		//Convert Voltage to milliamps
		orig_voltage_us = orig_voltage_us * 1000;
		//Convert Milliamps to inches
		cur_distance_us = orig_voltage_us / T166_MV_TO_IN;
		//Add Distance into Distance array for averageing
		distances[(index++ % 20)] = cur_distance_us;
		for(int i=0; i<20; i++) {
			average += distances[i];
		}
		
		average = average / 20;
		//DPRINTF(LOG_DEBUG, "Distance: %f Voltage: %f", average, orig_voltage_us);
        // Logging any values
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
