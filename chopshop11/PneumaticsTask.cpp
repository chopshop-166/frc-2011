/*******************************************************************************
*  Project   		: Framework
*  File Name  		: PneumaticsTask.cpp    
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Task for Pneumatics
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/* Find & Replace "PneumaticsTask" with the name you would like to give this task     */
/* Find & Replace "Testing" with the name you would like to give this task      */
/* Find & Replace "TaskPneumaticsTask" with the name you would like to give this task */
/*------------------------------------------------------------------------------*/

#include "WPILib.h"
#include "PneumaticsTask.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	// Any values that need to be logged go here
	// <<CHANGEME>>
};

//  Memory Log
// <<CHANGEME>>
class PneumaticsTaskLog : public MemoryLog
{
public:
	PneumaticsTaskLog() : MemoryLog(
			sizeof(struct abuf), PNEUMATICS_CYCLE_TIME, "PneumaticsTask",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~PneumaticsTaskLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int PneumaticsTaskLog::PutOne(void)
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
unsigned int PneumaticsTaskLog::DumpBuffer(char *nptr, FILE *ofile)
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
PneumaticsTask::PneumaticsTask(void)
{
	Start((char *)"PneumaticsTask", PNEUMATICS_CYCLE_TIME);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	return;
};
	
// task destructor
PneumaticsTask::~PneumaticsTask(void)
{
	return;
};
	
// Main function of the task
int PneumaticsTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	PneumaticsTaskLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 PneumaticsTask task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	// Register the proxy
	proxy = Proxy::getInstance();
	
	// assign pressure switch and compressor relay channels	
	unsigned int pressureSwitchChannel = 7;
	unsigned int compressorRelayChannel = 2;
	AnalogChannel ps(7);
	double pressure, ppressure = 0;
			
	//initialize object and start compressor	
	Compressor c(pressureSwitchChannel,compressorRelayChannel);
	c.Start();
		
    // General main loop (while in Autonomous or Tele mode)
	while (1) {
		
		// Capture the pressure by adjusted voltage
		// Subtract 0.5 because sensor ranges from 0.5 to 4.5
		pressure = (ps.GetVoltage()-0.5);
		
		//convert voltage to psi
		ppressure = (pressure * 62.5);
		
        // Logging any values
		// <<CHANGEME>>
		// Make this match the declaraction above
		sl.PutOne();
		
		
		
		lHandle->DriverStationDisplay("PSI: %d", ppressure);
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	
	c.Stop();
	return (0);
	
};
