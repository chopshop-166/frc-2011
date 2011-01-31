/*******************************************************************************
*  Project   		: Framework
*  File Name  		: SonarTask.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to get front and side sonar distances
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "SonarTask.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	double f,l,r;					// Sonar values
};

//  Memory Log
class SonarLog : public MemoryLog
{
public:
	SonarLog() : MemoryLog(
			sizeof(struct abuf), SONAR_CYCLE_TIME, "Sonar",
			"Seconds,Nanoseconds,Elapsed Time,Front,Left,Right\n"
			) {
		return;
	};
	~SonarLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(double,double,double);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int SonarLog::PutOne(double f,double l,double r)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->f = f;
		ob->l = l;
		ob->r = r;
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int SonarLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%3.3f,%3.3f,%3.3f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->f,
			ab->l,
			ab->r
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
SonarTask::SonarTask(void): ac(1), acl(2), acr(3), AverageSize(10)
{
	Start((char *)"166SonarTask", SONAR_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
	// Register main robot task
	lHandle = Robot::getInstance();
	return;
};
	
// task destructor
SonarTask::~SonarTask(void)
{
	return;
};
	
// Main function of the task
int SonarTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	// Register our logger
	SonarLog sl;                   // log
	lHandle->RegisterLogger(&sl);

	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Template task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	proxy->add("FrontDistance");
	proxy->add("LeftDistance");
	proxy->add("RightDistance");
	
	double frontarray[AverageSize];
	double leftarray[AverageSize];
	double rightarray[AverageSize];
	int i = 0;
	float volts=0;
	float voltsleft=0;
	float voltsright=0;
	
	double vf = 0;
	double vl = 0;
	double vr = 0;
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		// Get the adjusted voltage of each sensor
		volts = ac.GetVoltage();
		voltsleft = acl.GetVoltage();
		voltsright = acr.GetVoltage();
		if(volts<0){
			volts = 0;
		}else if(volts>3){
			volts = 3;
		}
		
		// Store each sensor's value into the rolling array
		frontarray[(i%AverageSize)] = volts;
		leftarray[(i%AverageSize)] = voltsleft;
		rightarray[(i%AverageSize)] = voltsright;
		
		// Get the Average of each sensor
		// Multiply each by the magic number 81. (yes, with a . at the end)
		// Store each value into the proxy
		vf = 0;
		vl = 0;
		vr = 0;
		for(unsigned j = 0;j<AverageSize;j++) {
			vf+= frontarray[j];
			vl+= leftarray[j];
			vr+= rightarray[j];
		}
		vf/=AverageSize;
		vl/=AverageSize;
		vr/=AverageSize;
		vf*=81.;
		vl*=81.;
		vr*=81.;
		proxy->set("FrontDistance", vf);
		proxy->set("LeftDistance", vl);
		proxy->set("RightDistance", vr);
		SmartDashboard::Log(vf, "Front Distance");
		SmartDashboard::Log(vl, "Left Distance");
		SmartDashboard::Log(vr, "Right Distance");
		
		// Increment the counter for distance slot in the array
		i++;
        // Logging any values
		sl.PutOne(vf,vl,vr);
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
