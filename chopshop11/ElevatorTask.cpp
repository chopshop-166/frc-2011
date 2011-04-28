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
#if ELEVATOR_USES_CAN
	unsigned faults;
#endif
};

//  Memory Log

class ElevatorLog : public MemoryLog
{
public:
	ElevatorLog() : MemoryLog(
			sizeof(struct abuf), ELEVATOR_CYCLE_TIME, "Elevator",
#if ELEVATOR_USES_CAN
			"Elapsed Time,Target Height, Elevator Speed, Faults\n"
#else
			"Elapsed Time,Target Height, Elevator Speed\n"
#endif
			) {
		return;
	};
	~ElevatorLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	
#if ELEVATOR_USES_CAN
	unsigned int PutOne(int, float, int);     // Log the values needed-add in arguments
#else
	unsigned int PutOne(int, float);     // Log the values needed-add in arguments
#endif
};

// Write one buffer into memory
#if ELEVATOR_USES_CAN
unsigned int ElevatorLog::PutOne(int height, float motor_speed, int faults)
#else
unsigned int ElevatorLog::PutOne(int height, float motor_speed)
#endif
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->target_type = height;
		ob->motor_speed = motor_speed;
#if ELEVATOR_USES_CAN
		ob->faults = faults;
#endif
		
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
#if ELEVATOR_USES_CAN
	fprintf(ofile, "%4.5f, %d, %1.6f, %d\n",
		((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
		ab->target_type,
		ab->motor_speed,
		ab->faults
#else
	fprintf(ofile, "%4.5f, %d, %1.6f\n",
		((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
		ab->target_type,
		ab->motor_speed
#endif
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
ElevatorTask::ElevatorTask(void):
#if ELEVATOR_USES_CAN
	elevator(ELEVATOR_JAGUAR)
#else
	elevator(ELEVATOR_PWM)
#endif
	, speed(1.0), deadband(0.1), height_deadband(40)
	, brakeSolenoid(ELEVATOR_BRAKE_RETRACT)
	, Height(HEIGHT_INPUT_A,HEIGHT_INPUT_B) 
{
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
	const double height_list[] = {0,0,0,1850,3760,5080,8310};
	proxy->add("ElevatorReadyPosition");
	proxy->add("ElevatorHeight");
	proxy->add("ElevatorZeroed");
	brakeSolenoid.Set(0);
	int clicks = 0;
	int bottom_press=0; // Counts the amount of loops a limit has been help down
	float new_speed=0;
	
	Height.Start();
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
//		SmartDashboard::Log(!elevator.GetForwardLimitOK(), "Top");
//		SmartDashboard::Log(!elevator.GetReverseLimitOK(), "Bottom");
		
#if ELEVATOR_USES_CAN
		if (!elevator.GetReverseLimitOK()) {
#else
		if(DigitalInput(ELEVATOR_BOTTOM_LIMIT_SWITCH).Get()) {
#endif
			bottom_press++;
			if(bottom_press >= 5) {
				Height.Reset();
			}
			proxy->set("ElevatorZeroed", true);
		} else {
			bottom_press = 0;
		}
		
		clicks=-Height.Get();//Saves value of clicks
//		SmartDashboard::Log(clicks, "Clicks");

		if(proxy->get(PRESET_TYPE_AXIS) < 0) {
			if(proxy->get(HIGH_PRESET_BUTTON)) {
				// Y button of the controller
				target_type = hHighSide;
			} else if(proxy->get(MID_PRESET_BUTTON)) {
				// X button of the controller
				target_type = hMidSide;
			} else if(proxy->get(LOW_PRESET_BUTTON)) {
				// A button of the controller
				target_type = hLowSide;
			} else if(proxy->get(FLOOR_PRESET_BUTTON)) {
				// Go to the floor
				target_type = hFloor;
			} else {
				// None of the important buttons are pressed
				target_type = hNone;
			}
		} else if(proxy->get(PRESET_TYPE_AXIS) > 0) {
			if(proxy->get(HIGH_PRESET_BUTTON)) {
				// Y button of the controller
				target_type = hHighCenter;
			} else if(proxy->get(MID_PRESET_BUTTON)) {
				// X button of the controller
				target_type = hMidCenter;
			} else if(proxy->get(LOW_PRESET_BUTTON)) {
				// A button of the controller
				target_type = hLowCenter;
			} else if(proxy->get(FLOOR_PRESET_BUTTON)) {
				// Go to the floor
				target_type = hFloor;
			} else {
				// None of the important buttons are pressed
				target_type = hNone;
			}
		}

		if(target_type != hNone) {
			if (clicks < (height_list[(int)target_type] - height_deadband)) {
				new_speed = speed;
				proxy->set("ElevatorReadyPosition", false);
			} else if (clicks > (height_list[(int)target_type] + height_deadband)) {
				new_speed = -speed;
//				new_speed = 0;
				proxy->set("ElevatorReadyPosition", true);
			} else {
				target_type = hNone;
				new_speed = 0;
				proxy->set("ElevatorReadyPosition", true);
			}
		} else {
			float axis = -proxy->get(ELEVATOR_AXIS);
//			SmartDashboard::Log(axis, "Axis Raw");
			if(axis >= deadband) {
				new_speed = axis;
				proxy->set("ElevatorReadyPosition", false);
			} else if(axis <= -deadband) {
				new_speed = (axis/2);
				proxy->set("ElevatorReadyPosition", false);
			} else {
				new_speed = 0;
			}
		}
		
		if(fabs(new_speed)<=0.1) {
			brakeSolenoid.Set(false);
		} else {
			brakeSolenoid.Set(true);
		}
		
		// Adjust down speed to be half as fast
		elevator.Set(new_speed);
		
		SmartDashboard::Log(target_type, "TargetType");
		if(target_type != hNone) {
			SmartDashboard::Log(height_list[(int)target_type], "Target Height");
		}
		proxy->set("ElevatorHeight", clicks);
		SmartDashboard::Log(clicks, "Clicks");
        // Logging any values
		if(lHandle->IsEnabled()) {
#if ELEVATOR_USES_CAN
			sl.PutOne(target_type, elevator.Get(), elevator.GetFaults());
#else
			sl.PutOne(target_type, elevator.Get());
#endif
		}
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
