/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: MiniDeployTask.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to handle minibot deployment logic
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/


#include "WPILib.h"
#include "MiniDeployTask.h"
#include "Solenoid.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	float axis;
	int state;
	int limit;
};

//  Memory Log
class MiniDeployLog : public MemoryLog
{
public:
	MiniDeployLog() : MemoryLog(
			sizeof(struct abuf), MINIDEPLOY_CYCLE_TIME, "MiniDeploy",
			"Elapsed Time,Axis,Enum,Limit\n"
			) {
		return;
	};
	~MiniDeployLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(float, int, int);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int MiniDeployLog::PutOne(float a, int s, int l)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->axis = a;
		ob->state = s;
		ob->limit = l;
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int MiniDeployLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%4.5f,%0.5f,%d,%d\n",
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->axis, ab->limit, ab->state
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
MiniDeploy166::MiniDeploy166(void): DeployerExtender(MINIBOT_DEPLOYER_EXTENDER), MiniDeployer(MINIBOT_DEPLOYER),
		MiniRelease(MINIBOT_RELEASE), Deploy_Limit(DEPLOYLIMIT)
{
	Start((char *)"166MiniDeployTask", MINIDEPLOY_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
	Deploy_State = kWait;
	return;
};
	
// task destructor
MiniDeploy166::~MiniDeploy166(void)
{
	return;
};
	
// Main function of the task
int MiniDeploy166::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	// Register our logger
	MiniDeployLog sl;                   // log
		
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 MiniDeploy task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	// Register main robot task
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	lHandle->DriverStationDisplay("MINI: Ready");
	int loopcount =0;
	bool currval=false,lastval=false;
	
	MiniRelease.Set(0);
	
    // General main loop (while in Autonomous or Tele mode) 
	while (true){
		switch (Deploy_State) {
			case kWait: {
				if(
						(proxy->get("matchtimer") <= 10.0) &&
						(proxy->get(DEPLOY_MINIBOT_COPILOT)) <= -0.5) {
					lHandle->DriverStationDisplay("MINI: Swinging");
					Deploy_State = kSwing;
				}
				break;
			}
			case kSwing: {
				//Release latch to swing arm to pole
				MiniRelease.Set(1);
				if (!Deploy_Limit.Get()){
					Deploy_State = kExtend;
					lHandle->DriverStationDisplay("MINI: Extending");
				}
				break;
			}
			case kExtend: {
				//Extend minibot + deployer to pole
				DeployerExtender.Set(1);
				Deploy_State = kDeploy;
				lHandle->DriverStationDisplay("MINI: Deploying");
				break;
			}
			case kDeploy: {
				if (loopcount>=25) {
					//Pull piston back to release minbot
					MiniDeployer.Set(1);
					Deploy_State =kResting;
				}
				
				break;
			}
			case kResting: {
				currval = (proxy->get(DEPLOY_MINIBOT_COPILOT) >=0.5);
				if(currval != lastval) {
					MiniDeployer.Set(!MiniDeployer.Get());
				}
				lastval = currval;
			}
		}
		if ((Deploy_State == kExtend) || (Deploy_State == kDeploy)) {
			loopcount++;
		}
        // Logging any values
		sl.PutOne(proxy->get(DEPLOY_MINIBOT_COPILOT), Deploy_State, !Deploy_Limit.Get());
		printf("%1.6f\t%d\t%d\n", proxy->get(DEPLOY_MINIBOT_COPILOT), Deploy_State, !Deploy_Limit.Get());
		
		// Wait for our next loop
		WaitForNextLoop();		
	}
	return (0);
	
};
