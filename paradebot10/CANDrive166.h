/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: CANDrive166.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 23, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Header for code which monitors jaguar return info
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#if !defined(_CANDRIVE166_H)
#define _CANDRIVE166_H
#include "WPILib.h"
#include "Robot.h"
#include "CANJaguar.h"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
//
#define CAN_CYCLE_TIME (50) // 50ms
// Set to 1 if using arcade drive
#define USING_ARCADE (1)
#define ARCADE_AUTO_SPEED (0.25)


class Team166CANDrive : public Team166Task
{
	
public:
	
	// task constructor
	Team166CANDrive(void);

	// task destructor
	virtual ~Team166CANDrive(void);

	// get handle
	static Team166CANDrive *getInstance(void);
	
	// Limit values to -1 - 1
	float Limit(float);
	void SquareInputs(float&,float&);
	
	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
	CANJaguar leftJag,rightJag;

private:
	/**
	 * @brief The single instance handle
	 */
	static Team166CANDrive* CANDriveHandle;
};

#endif // !defined(_CANDRIVE166)
