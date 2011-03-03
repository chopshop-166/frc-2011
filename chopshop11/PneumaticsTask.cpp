/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: PneumaticsTask.cpp    
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task for running compressor and measuring pressure
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/


#include "WPILib.h"
#include "PneumaticsTask.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	float pressure;
};

//  Memory Log
class PneumaticsTaskLog : public MemoryLog
{
public:
	PneumaticsTaskLog() : MemoryLog(
			sizeof(struct abuf), PNEUMATICS_CYCLE_TIME, "PneumaticsTask",
			"Elapsed Time, Pressure\n"
			) {
		return;
	};
	~PneumaticsTaskLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	
	unsigned int PutOne(float);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int PneumaticsTaskLog::PutOne(float pressure_in)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->pressure = pressure_in;
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
	fprintf(ofile, "%4.5f,%f\n",
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->pressure
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
PneumaticsTask::PneumaticsTask(void): PSITransducer(PRESSURETRANSDUCER), AirCompresser(PRESSURESWITCH,COMPRESSORRELAY)
{
	Start((char *)"PneumaticsTask", PNEUMATICS_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();

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
	// Register our logger
	PneumaticsTaskLog sl;                   // log
	
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 PneumaticsTask task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register main robot task
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	double PressureVolts=0, PSI = 0;
			
	//initialize object and start compressor
	AirCompresser.Start();
		
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		
		// Capture the pressure by adjusted voltage
		// Subtract 0.5 because sensor ranges from 0.5 to 4.5
		PressureVolts = (PSITransducer.GetVoltage()-0.5);
		
		//convert voltage to psi
		PSI = (PressureVolts * PSI_PER_VOLT);
		
        // Logging any values
		sl.PutOne(PSI);
		
//		SmartDashboard::Log(PSI, "PSI");
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	
	AirCompresser.Stop();
	return (0);
	
};
