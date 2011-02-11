/*******************************************************************************
*  File Name  		: DriveTask.cpp  
*  Project   		: Chopshop11   
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Takes X, Y, Z inputs from joystick generates outputs for mecanum wheels
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "DriveTask.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf166
{
	struct timespec tp;             // Time of snapshot
	float x,y,z;					// Joystick axis
	double LF_Speed, RF_Speed, LB_Speed, RB_Speed;
	float WS1, WS2, WS3, WS4; 
};

//  Memory Log
class DriveLog : public MemoryLog
{
public:
	DriveLog() : MemoryLog(
			sizeof(struct abuf166), DRIVE_TASK_CYCLE_TIME, "DriveTask",
			"Seconds,Nanoseconds,Elapsed Time,X,Y,Z,LF Speed, RF Speed, LB Speed, RB Speed, FLWS, FRWS, BLWS, BRWS\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~DriveLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	
	unsigned int PutOne(float,float,float,double,double,double, double, float, float, float, float);     // Log the values needed-add in arguments
};

// Write one buffer into memory

unsigned int DriveLog::PutOne(float x, float y, float z, double LF, double RF, double LB, double RB, float FrontLeftWS, float FrontRightWS, float BackLeftWS, float BackRightWS)
{
	struct abuf166 *ob;               // Output buffer
	// Get output buffer
	if ((ob = (struct abuf166 *)GetNextBuffer(sizeof(struct abuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->x = x;
		ob->y = y;
		ob->z = z;
		ob->LF_Speed = LF;
		ob->RF_Speed = RF;
		ob->LB_Speed = LB;
		ob->RB_Speed = RB;
		ob->WS1 = FrontLeftWS;
		ob->WS2 = FrontRightWS;
		ob->WS3 = BackLeftWS;
		ob->WS4 = BackRightWS;
		// Add any values to be logged here
		
		return (sizeof(struct abuf166));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int DriveLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf166 *ab = (struct abuf166 *)nptr;
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->x,
			ab->y,
			ab->z,
			ab->LF_Speed,
			ab->LB_Speed,
			ab->RF_Speed,
			ab->RB_Speed,
			ab->WS1,
			ab->WS2,
			ab->WS3,
			ab->WS4
	);
	
	// Done
	return (sizeof(struct abuf166));
}

/**
 * Normalize all wheel speeds if the magnitude of any wheel is greater than 1.0.
 * Taken from RobotDrive
 */
void DriveTask::Normalize(float *wheelSpeeds)
{
	float maxMagnitude = fabs(wheelSpeeds[0]);
	INT32 i;
	for (i=1; i<4; i++)
	{
		float temp = fabs(wheelSpeeds[i]);
		if (maxMagnitude < temp) maxMagnitude = temp;
	}
	if (maxMagnitude > 1.0)
	{
		for (i=0; i<4; i++)
		{
			wheelSpeeds[i] = wheelSpeeds[i] / maxMagnitude;
		}
	}
}

// task constructor
#if 1
DriveTask::DriveTask(void): m_maxOutput(500), syncGroup(0x80), fl(4, CANJaguar::kSpeed), fr(3, CANJaguar::kSpeed), bl(6, CANJaguar::kSpeed), br(5, CANJaguar::kSpeed)
#else 
DriveTask::DriveTask(void): m_maxOutput(1), syncGroup(0x80), fl(4), fr(3), bl(6), br(5)
#endif 
{
	Start((char *)"166DriveTask", DRIVE_TASK_CYCLE_TIME);
	wheelSpeeds[0] = wheelSpeeds[1] = wheelSpeeds[2] = wheelSpeeds[3] = 0;
	//Front Left
	fl.ConfigEncoderCodesPerRev(1024);
	fl.SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	fl.SetPID(0.150, 0.004, 0.00);
	fl.EnableControl(0);
	//Front Right
	fr.ConfigEncoderCodesPerRev(1024);
	fr.SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	fr.SetPID(0.150, 0.004, 0.00);
	fr.EnableControl(0);
	//Back Left
	bl.ConfigEncoderCodesPerRev(1024);
	bl.SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	bl.SetPID(0.150, 0.004, 0.00);
	bl.EnableControl(0);
	//Back Right
	br.ConfigEncoderCodesPerRev(1024);
	br.SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	br.SetPID(0.150, 0.005, 0.00);
	br.EnableControl(0);
	// Register the proxy
	proxy = Proxy::getInstance();
	return;
};
	
// task destructor
DriveTask::~DriveTask(void)
{
	return;
};

double DriveTask::SignPreservingSquare(double raw)
{
	if(raw < 0) {
		return -(raw * raw);
	} else {
		return (raw * raw);
	}
}
double DriveTask::TruncateDouble(double input)
{
	char buffer[6];
	sprintf(buffer,"%1.2f", input);
	return atof(buffer);
}
	
// Main function of the task
int DriveTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	// Register our logger
	DriveLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Drive task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	float FLSpeed = 0;
	float FRSpeed = 0;
	float BLSpeed = 0;
	float BRSpeed = 0;
	// General main loop (while in Autonomous or Tele mode)
	while (true) {
		FLSpeed = fl.GetSpeed();
		FRSpeed = fr.GetSpeed();
		BLSpeed = bl.GetSpeed();
		BRSpeed = br.GetSpeed();
		x=TruncateDouble(SignPreservingSquare(proxy->get(DRIVE_STRAFE)));
		y=TruncateDouble(SignPreservingSquare(proxy->get(DRIVE_FOWARD_BACK)));
		r=TruncateDouble(SignPreservingSquare(proxy->get(DRIVE_ROTATION)));
		
		SmartDashboard::Log(x,"X");
		SmartDashboard::Log(y,"Y");
		SmartDashboard::Log(r,"R");
		
		SmartDashboard::Log(FLSpeed, "FL Wheel Speed");
		SmartDashboard::Log(FRSpeed, "FR Wheel Speed");
		SmartDashboard::Log(BLSpeed, "BL Wheel Speed");
		SmartDashboard::Log(BRSpeed, "BR Wheel Speed");
		
		wheelSpeeds[0] = x - y + r;
		wheelSpeeds[1] = -x - y - r;
		wheelSpeeds[2] = -x - y + r;
		wheelSpeeds[3] = x - y - r;
		
		Normalize(wheelSpeeds);
		
		fl.Set(wheelSpeeds[0]* m_maxOutput, syncGroup);
		fr.Set(-wheelSpeeds[1]* m_maxOutput, syncGroup);
		bl.Set(wheelSpeeds[2]* m_maxOutput, syncGroup);
		br.Set(-wheelSpeeds[3]* m_maxOutput, syncGroup);
		
		CANJaguar::UpdateSyncGroup(syncGroup);

		sl.PutOne(x,y,r,FLSpeed,FRSpeed,BLSpeed,BRSpeed, wheelSpeeds[0], wheelSpeeds[1], wheelSpeeds[2], wheelSpeeds[3]);
		
		// Wait for our next lap
		WaitForNextLoop();
	}
	return (0);
};
