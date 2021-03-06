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
	double LF_Speed, RF_Speed, LB_Speed, RB_Speed;
	float WS1, WS2, WS3, WS4;
#if DRIVE_USES_CAN
	int LF_Fault, RF_Fault, LB_Fault, RB_Fault;
#endif
};

//  Memory Log
class DriveLog : public MemoryLog
{
public:
	DriveLog() : MemoryLog(
			sizeof(struct abuf166), DRIVE_TASK_CYCLE_TIME, "DriveTask",
#if DRIVE_USES_CAN
			"Elapsed Time,LF Speed, RF Speed, LB Speed, RB Speed, LF WS, RF WS, LB WS, RB WS, LF Faults, RF Faults, LB Faults, RB Faults\n"
#else
			"Elapsed Time,LF Speed, RF Speed, LB Speed, RB Speed, LF WS, RF WS, LB WS, RB WS\n"
#endif
			) {
		return;
	};
	~DriveLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	
#if DRIVE_USES_CAN
	unsigned int PutOne(double,double,double, double, float, float, float, float, int, int, int, int);
#else
	unsigned int PutOne(double,double,double, double, float, float, float, float);
#endif
};

// Write one buffer into memory

unsigned int DriveLog::PutOne(double LF, double RF, double LB, double RB,
		float FrontLeftWS, float FrontRightWS, float BackLeftWS, float BackRightWS
#if DRIVE_USES_CAN
		,int LF_Faults, int RF_Faults, int LB_Faults, int RB_Faults
#endif
		)
{
	struct abuf166 *ob;               // Output buffer
	// Get output buffer
	if ((ob = (struct abuf166 *)GetNextBuffer(sizeof(struct abuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->LF_Speed = LF;
		ob->RF_Speed = RF;
		ob->LB_Speed = LB;
		ob->RB_Speed = RB;
		ob->WS1 = FrontLeftWS;
		ob->WS2 = FrontRightWS;
		ob->WS3 = BackLeftWS;
		ob->WS4 = BackRightWS;
#if DRIVE_USES_CAN
		ob->LF_Fault = LF_Faults;
		ob->RF_Fault = RF_Faults;
		ob->LB_Fault = LB_Faults;
		ob->RB_Fault = RB_Faults;
#endif
		
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
#if DRIVE_USES_CAN
	fprintf(ofile, "%4.5f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%d,%d,%d,%d\n",
#else
	fprintf(ofile, "%4.5f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f,%1.6f\n",
#endif
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->LF_Speed,
			ab->LB_Speed,
			ab->RF_Speed,
			ab->RB_Speed,
			ab->WS1,
			ab->WS2,
			ab->WS3,
			ab->WS4
#if DRIVE_USES_CAN
			,ab->LF_Fault,
			ab->RF_Fault,
			ab->LB_Fault,
			ab->RB_Fault
#endif
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
#if DRIVE_USES_CAN
	#if 1
	DriveTask::DriveTask(void): m_maxOutput(500), syncGroup(0x80), fl(4, CANJaguar::kSpeed), fr(3, CANJaguar::kSpeed), bl(6, CANJaguar::kSpeed), br(5, CANJaguar::kSpeed)
	#else 
	DriveTask::DriveTask(void): m_maxOutput(1), syncGroup(0x80), fl(4), fr(3), bl(6), br(5)
	#endif 
#else
	DriveTask::DriveTask(void): m_maxOutput(500), syncGroup(0x80), fl(DRIVE_FL_PWM), fr(DRIVE_FR_PWM), bl(DRIVE_BL_PWM), br(DRIVE_BR_PWM)
#endif
{
	Start((char *)"166DriveTask", DRIVE_TASK_CYCLE_TIME);
	wheelSpeeds[0] = wheelSpeeds[1] = wheelSpeeds[2] = wheelSpeeds[3] = 0;
#if DRIVE_USES_CAN
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
#endif
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
	input = int(input*100);
	return input/100.0;
}
int DriveTask::LostComms(CANJaguar& CheckJag, int location)
{

	if((actualSpeed[location]==0) && ((wheelSpeeds[location]>0.02) || (wheelSpeeds[location]<-0.02)))
	{
		encoderBad[location]=1;
		CheckJag.ChangeControlMode(CANJaguar::kPercentVbus);
		CheckJag.EnableControl();
	} else{
		return 0;
	}
	return 1;
}
string DriveTask::ControlModeString(CANJaguar::ControlMode ControlModeIn)
{
	switch(ControlModeIn) {
		case CANJaguar::kVoltage:
			return "Voltage";
		case CANJaguar::kCurrent:
			return "Current";
		case CANJaguar::kPercentVbus:
			return "Percent Vbus";
		case CANJaguar::kPosition:
			return "Position";
		case CANJaguar::kSpeed:
			return "Speed";
		default:
			return "";
	}
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
	int Throttle=0;
	// General main loop (while in Autonomous or Tele mode)
	actualSpeed[0]=actualSpeed[1]=actualSpeed[2]=actualSpeed[3]=0;
	while (true) {
		x=TruncateDouble(SignPreservingSquare(proxy->get(DRIVE_STRAFE)));
		y=TruncateDouble(SignPreservingSquare(proxy->get(DRIVE_FORWARD_BACK)));
		r=TruncateDouble(SignPreservingSquare(proxy->get(DRIVE_ROTATION)));
		
#if 0
		SmartDashboard::Log(x,"X");
		SmartDashboard::Log(y,"Y");
		SmartDashboard::Log(r,"R");
		
		SmartDashboard::Log(actualSpeed[0], "FL Wheel Speed");
		SmartDashboard::Log(actualSpeed[1], "FR Wheel Speed");
		SmartDashboard::Log(actualSpeed[2], "BL Wheel Speed");
		SmartDashboard::Log(actualSpeed[3], "BR Wheel Speed");
#endif
		
		wheelSpeeds[0] = x - y + r;
		wheelSpeeds[1] = -x - y - r;
		wheelSpeeds[2] = -x - y + r;
		wheelSpeeds[3] = x - y - r;
		
		if (proxy->get(DRIVER_SWITCH_JOYSTICKS)) {
			CountTimePressed++;
		}
		if (CountTimePressed >= 10) {
			CountTimePressed = 0;
#if DRIVE_USES_CAN
			fl.ChangeControlMode(CANJaguar::kPercentVbus);
			fr.ChangeControlMode(CANJaguar::kPercentVbus);
			bl.ChangeControlMode(CANJaguar::kPercentVbus);
			br.ChangeControlMode(CANJaguar::kPercentVbus);
#endif
			for (int i=0; i<4; i++) {
				encoderBad[i] = 1;
			}
		}
		
//		if ((++valuethrottle) % 50 ==0)
//		{
//			LostComms(fl, 0);
//			LostComms(fr, 1);
//			LostComms(bl, 2);
//			LostComms(br, 3);
//			
//			SmartDashboard::Log(encoderBad[0], "FL Mode");
//			SmartDashboard::Log(encoderBad[1], "FR Mode");
//			SmartDashboard::Log(encoderBad[2], "BL Mode");
//			SmartDashboard::Log(encoderBad[3], "BR Mode");
//		}
			
		Normalize(wheelSpeeds);
		if (proxy->get(DRIVER_SLOW_DRIVE)) {
			for(int jag_id=0; jag_id<4; jag_id++) {
				wheelSpeeds[jag_id] = wheelSpeeds[jag_id] /2;
			}
		}
		if(encoderBad[0]==1){
			fl.Set(wheelSpeeds[0], syncGroup);
		} else {
			fl.Set(wheelSpeeds[0]* m_maxOutput, syncGroup);
		}
		if(encoderBad[1]==1){
			fr.Set(-wheelSpeeds[1], syncGroup);
		} else {
			fr.Set(-wheelSpeeds[1]* m_maxOutput, syncGroup);
		}
		if(encoderBad[2]==1){
			bl.Set(wheelSpeeds[2], syncGroup);
		} else {
			bl.Set(wheelSpeeds[2]* m_maxOutput, syncGroup);
		}
		if(encoderBad[3]==1){
			br.Set(-wheelSpeeds[3], syncGroup);
		} else {
			br.Set(-wheelSpeeds[3]* m_maxOutput, syncGroup);
		}
		
		CANJaguar::UpdateSyncGroup(syncGroup);
#if DRIVE_USES_CAN
		if((++Throttle % (500/DRIVE_TASK_CYCLE_TIME)) == 0) {
			actualSpeed[0] = fl.GetSpeed();
			actualSpeed[1] = fr.GetSpeed();
			actualSpeed[2] = bl.GetSpeed();
			actualSpeed[3] = br.GetSpeed();
			if(lHandle->IsEnabled()) {
				sl.PutOne(actualSpeed[0],actualSpeed[1],actualSpeed[2],actualSpeed[3],
						wheelSpeeds[0], wheelSpeeds[1], wheelSpeeds[2], wheelSpeeds[3],
						fl.GetFaults(), fr.GetFaults(), bl.GetFaults(), br.GetFaults()
						);
			}
		}
#else
		if((++Throttle % (500/DRIVE_TASK_CYCLE_TIME)) == 0) {
			actualSpeed[0] = fl.Get();
			actualSpeed[1] = fr.Get();
			actualSpeed[2] = bl.Get();
			actualSpeed[3] = br.Get();
			if(lHandle->IsEnabled()) {
				sl.PutOne(actualSpeed[0],actualSpeed[1],actualSpeed[2],actualSpeed[3],
						wheelSpeeds[0], wheelSpeeds[1], wheelSpeeds[2], wheelSpeeds[3]
						);
			}
		}
#endif
		// Wait for our next lap
		WaitForNextLoop();
	}
	return (0);
};
