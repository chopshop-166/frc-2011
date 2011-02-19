/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: ElevatorTask.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to run the elevator
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "ElevatorTask.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	int target_type;
	float motor_speed;
};

//  Memory Log

class ElevatorLog : public MemoryLog
{
public:
	ElevatorLog() : MemoryLog(
			sizeof(struct abuf), ELEVATOR_CYCLE_TIME, "Elevator",
			"Seconds,Nanoseconds,Elapsed Time,Target Height, Elevator Speed\n"
			) {
		return;
	};
	~ElevatorLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	
	unsigned int PutOne(int, float);     // Log the values needed-add in arguments
};

// Write one buffer into memory

unsigned int ElevatorLog::PutOne(int height, float motor_speed)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->target_type = height;
		ob->motor_speed = motor_speed;
		
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int ElevatorLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f, %d, %f1.6\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->target_type,
			ab->motor_speed
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
ElevatorTask::ElevatorTask(void): elevator(ELEVATOR_JAGUAR), speed(0.25), deadband(0.05), height_deadband(2)
	, brakeSolenoid(ELEVATOR_BRAKE_EXTEND)
	, Height(HEIGHT_INPUT_A,HEIGHT_INPUT_B) 
{
	MovementMode = kUnKnown;
	Start((char *)"166ElevatorTask", ELEVATOR_CYCLE_TIME);	
	// Register the proxy
	proxy = Proxy::getInstance();
	
	return;
};
	
// task destructor
ElevatorTask::~ElevatorTask(void)
{
	return;
};
	
// Main function of the task
int ElevatorTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	//Create then register logger
	ElevatorLog sl;                   // log
	
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Elevator task\n");

	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register main robot
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	enum {hNone=-1, hFloor=0,
		hLowSide=1, hLowCenter=2,
		hMidSide=3, hMidCenter=4,
		hHighSide=5, hHighCenter=6} target_type = hNone;
	
	// Fix these heights once we can test
	// They currently don't take the arm height into account
	const double height_list[] = {0,30,37,67,74,104,111};
	proxy->add("ElevatorReadyPosition");
	brakeSolenoid.Set(0);
	float HowHigh=0;
	int clicks = 0;
	Height.Start();
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		if(!elevator.GetForwardLimitOK()) {
			MovementMode = kDown;
			Height.Reset();
		} else if (!elevator.GetReverseLimitOK()) {
			MovementMode = kUp;
			Height.Reset();
		}
		clicks=Height.Get();//Saves value of clicks
		if(MovementMode == kDown) {
			HowHigh = MAXHEIGHT+(ClicksPerInch*clicks);
		}
		if(MovementMode == kUp) {
			HowHigh = MINHEIGHT+(ClicksPerInch*clicks);
		}
		SmartDashboard::Log(HowHigh, "Height");
		switch ((int)proxy->get("HeightLocation")) {
			case -1:
				target_type = hNone;
			case 0:
				target_type = hLowSide;
			case 1:
				target_type = hLowCenter;
			case 2:
				target_type = hMidSide;
			case 3:
				target_type = hMidCenter;
			case 4:
				target_type = hHighSide;
			case 5:
				target_type = hHighCenter;
			case 6:
				target_type = hFloor;
		}
		
		if(target_type != hNone) {
			float target = height_list[target_type];
			elevator.Set((target < HowHigh)? speed : ((target > HowHigh)? -speed : 0));
			if(fabs(target-HowHigh) < height_deadband) {
				target_type = hNone;
			}
			proxy->set("ElevatorReadyPosition", true);
			
		} else {
			float axis = proxy->get(ELEVATOR_AXIS);
			if(axis >= deadband || axis <= -deadband) {
				brakeSolenoid.Set(1);
				elevator.Set(axis);
			} else {
				brakeSolenoid.Set(0);
				elevator.Set(0);
			}
		}

		
        // Logging any values
		sl.PutOne(target_type, elevator.Get());
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
