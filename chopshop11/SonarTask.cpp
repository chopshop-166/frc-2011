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
SonarTask::SonarTask(void):
	SonarCenter(SONAR_CENTER),
	SonarLeft(SONAR_LEFT),
	SonarRight(SONAR_RIGHT)
{
	Start((char *)"166SonarTask", SONAR_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
	
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
	

	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Sonar task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register main robot task
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	proxy->add("FrontDistance");
	proxy->add("LeftDistance");
	proxy->add("RightDistance");
	
	float frontarray[AVERAGESIZE];
	float leftarray[AVERAGESIZE];
	float rightarray[AVERAGESIZE];
	for (int i=0; i<AVERAGESIZE;i++) {
		frontarray[i]= 0;
		leftarray[i]=0;
		rightarray[i]=0;
	}
	int i = 0;
	//Volts from sensors
	float voltscenter=0;
	float voltsleft=0;
	float voltsright=0;
	
	//Distance in inches
	float DistanceCenter = 0;
	float DistanceLeft = 0;
	float DistanceRight = 0;
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		// Get the adjusted voltage of each sensor
		voltscenter = SonarCenter.GetVoltage();
		voltsleft = SonarLeft.GetVoltage();
		voltsright = SonarRight.GetVoltage();
		if(voltscenter<0){
			voltscenter = 0;
		}else if(voltscenter>3){
			voltscenter = 3;
		}
		if(voltsleft<0){
			voltsleft = 0;
		}else if(voltsleft>3){
			voltsleft = 3;
		}
		if(voltsright<0){
			voltsright = 0;
		}else if(voltsright>3){
			voltsright = 3;
		}
		// Store each sensor's value into the rolling array
		frontarray[(i%AVERAGESIZE)] = voltscenter;
		leftarray[(i%AVERAGESIZE)] = voltsleft;
		rightarray[(i%AVERAGESIZE)] = voltsright;
		
		// Get the Average of each sensor
		// Multiply each by the magic number 81. (yes, with a . at the end)
		// Store each value into the proxy
		DistanceCenter = 0;
		DistanceLeft = 0;
		DistanceRight = 0;
		for(unsigned j = 0;j<AVERAGESIZE;j++) {
			DistanceCenter+= frontarray[j];
			DistanceLeft+= leftarray[j];
			DistanceRight+= rightarray[j];
		}
		DistanceCenter/=AVERAGESIZE;
		DistanceLeft/=AVERAGESIZE;
		DistanceRight/=AVERAGESIZE;
		DistanceCenter*=SONARINPERVOLT;
		DistanceLeft*=SONARINPERVOLT;
		DistanceRight*=SONARINPERVOLT;
		proxy->set("FrontDistance", DistanceCenter);
		proxy->set("LeftDistance", DistanceLeft);
		proxy->set("RightDistance", DistanceRight);
		SmartDashboard::Log(DistanceCenter, "Front Distance");
		SmartDashboard::Log(DistanceLeft, "Left Distance");
		SmartDashboard::Log(DistanceRight, "Right Distance");
		
		// Increment the counter for distance slot in the array
		i++;
        // Logging any values
		sl.PutOne(DistanceCenter,DistanceLeft,DistanceRight);
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
