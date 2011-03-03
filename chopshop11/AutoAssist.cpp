/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: AutoAssist.cpp
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task that controls logic to adjust drive to score automatically
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "AutoAssist.h"
#include "PhotoElectric.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
};

//  Memory Log
class AutoAssistLog : public MemoryLog
{
public:
	AutoAssistLog() : MemoryLog(
			sizeof(struct abuf), AUTOASSIST_CYCLE_TIME, "AutoAssist",
			"Elapsed Time\n"
			) {
		return;
	};
	~AutoAssistLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int AutoAssistLog::PutOne(void)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		// Add any values to be logged here
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int AutoAssistLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%4.5f\n",
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.))
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
AutonomousAssistTask::AutonomousAssistTask(void)
{
	Start((char *)"166AutoAssistTask", AUTOASSIST_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
	
	return;
};
	
// task destructor
AutonomousAssistTask::~AutonomousAssistTask(void)
{
	return;
};
	
// Main function of the task
int AutonomousAssistTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	// Register our logger
	AutoAssistLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 AutoAssist task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	// Register main robot task
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	int curr_value;
	x=y=r=0;
	bool auto_enabled = false;
	bool reverse = false; // Go backwards after releasing
#if 0
	int fourked_timer = 0; // Timer for the fork (The u is intentional)
	bool fourked_hit = false; // Has it hit a fork?
#endif
	proxy->add("AutoassistReadyPosition");
	proxy->DisableJoystickAxesByButton(1,DRIVER_AUTOASSIST_REAL);
	
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		auto_enabled = ((lHandle->IsOperatorControl() && proxy->JoystickAxesDisabled(1))
				|| lHandle->IsAutonomous());
		SmartDashboard::Log(auto_enabled, "Autoassist Enabled");

		if(auto_enabled) {
			if(proxy->exists("LineDirection")) {
				curr_value = (int)proxy->get("LineDirection");
#if 0
				if(fourked_timer) {
					fourked_timer--;
					if(proxy->get(LINE_STRAFE_LEFT_BUTTON)) {
						x = -AUTOASSIST_SPEED_STRAFE;
					} else if(proxy->get(LINE_STRAFE_RIGHT_BUTTON)) {
						x = AUTOASSIST_SPEED_STRAFE;
					} else {
						x = 0;
					}
				} else if(fourked_hit) {
					x=0;
					r=0;
				} else {
#endif
					x=0;
					switch(curr_value) {
						case lRight:
							r = AUTOASSIST_SPEED_TURN;
							break;
						case lCenter:
							r = 0;
							break;
						case lLeft:
							r = -AUTOASSIST_SPEED_TURN;
							break;
						case lNo_Line:
							break;
						case lFork:
#if 0
							if(proxy->get(LINE_STRAFE_LEFT_BUTTON)) {
								x = -AUTOASSIST_SPEED_STRAFE;
							} else if(proxy->get(LINE_STRAFE_RIGHT_BUTTON)) {
								x = AUTOASSIST_SPEED_STRAFE;
							} else {
								x = 0;
							}
							fourked_timer = (2000/AUTOASSIST_CYCLE_TIME);
#endif
							r = 0;
							break;
						default:
							r=0;
							break;
					}
#if 0
				}
#endif
			}
			if(proxy->exists("FrontDistance")) {
				float dist = proxy->get("FrontDistance");
				printf("%f\r", dist);
				if(dist > AUTOASSIST_SONAR_FRONT_DISTANCE) {
					y=AUTOASSIST_SPEED_FORWARD;
					y=0;
				} else {
					y=0;
				}
			} else {
				y=0;
			}
			if (y==0) {
				if(proxy->get(LINE_STRAFE_LEFT_BUTTON) && proxy->exists("LeftDistance")) {
					// We want it to go to the left, according to the sonar
					if(proxy->get("LeftDistance") > AUTOASSIST_SONAR_SIDE_DISTANCE) {
						x = -AUTOASSIST_SPEED_STRAFE;
					} else {
						x = 0;
					}
				} else if(proxy->get(LINE_STRAFE_RIGHT_BUTTON) && proxy->exists("RightDistance")) {
					// We want it to go to the right, according to the sonar
					if(proxy->get("RightDistance") > AUTOASSIST_SONAR_SIDE_DISTANCE) {
						x = AUTOASSIST_SPEED_STRAFE;
					} else {
						x = 0;
					}
				} else {
					x = 0;
				}
			}
			
			if(proxy->exists("CanSeeCameraTargets")) {
				if(proxy->get("CanSeeCameraTargets") && proxy->exists("NormalizedTargetCenter")) {
					// It sees targets
					if(proxy->get("NormalizedTargetCenter") < 0) {
						// We need to go right
//						x += AUTOASSIST_SPEED_CAMERA_STRAFE;
					} else if(proxy->get("NormalizedTargetCenter") > 0) {
						// We need to go left
//						x -= AUTOASSIST_SPEED_CAMERA_STRAFE;
					} else {
//						x=0;
					}
				}
			}
			
			if(reverse) {
				// It's let go, so back away.
				x=r=0;
				y=-AUTOASSIST_SPEED_FORWARD;
			}
			
			proxy->set(DRIVE_STRAFE,x);
			proxy->set(DRIVE_FOWARD_BACK,y);
			proxy->set(DRIVE_ROTATION,r);
//			printf("%f\t%f\t%f\r", x, y, r);
			
#if 0
			if(x==0 && y==0 && r==0) {
				//The robot's not moving
				proxy->set("AutoassistReadyPosition",true);
				reverse = true;
				while(proxy->get(GRIPPER_BUTTON)==0) {
					// Wait until it releases the tube
					WaitForNextLoop();
				}
				for(unsigned i=0;i<10;i++) {
					// Slight delay between releasing and moving away
					WaitForNextLoop();
				}
			} else {
				proxy->set("AutoassistReadyPosition",false);
			}
#endif
		} else {
			reverse = false;
		}
		
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
