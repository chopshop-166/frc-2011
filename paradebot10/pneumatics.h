/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: TaskTemplate.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Pneumatics header file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#if !defined(PNEUMATICS_H)
#define PNEUMATICS_H
#include "WPILib.h"
#include "Robot.h"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
// You should rename this when you copy it into a new file 
#define PNEUMATICS_CYCLE_TIME (10) // 10ms

// Rename this, too, or you'll run into collisions
class Pneumatics166 : public Team166Task
{
	
public:
	
	// task constructor
	Pneumatics166(void);

	// task destructor
	virtual ~Pneumatics166(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	// Any variables that the task has as members go here
	Compressor compressor;
	Solenoid cylinder_open;
	Relay fans;
};

#endif // !defined(PNEUMATICS_H)
