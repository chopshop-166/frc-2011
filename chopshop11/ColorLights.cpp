/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: ColorLights.cpp
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to run the LEDs to tell the feeders the needed colors
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2011.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "ColorLights.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// task constructor
ColorLightTask::ColorLightTask(void):red(4,Relay::kForwardOnly), white(5,Relay::kForwardOnly), blue(6,Relay::kForwardOnly)
{
	Start((char *)"166ColorLightsTask", COLORLIGHT_CYCLE_TIME);
	
	// Register the proxy
	proxy = Proxy::getInstance();
	// Register main robot task
	lHandle = Robot::getInstance();
	
	return;
};
	
// task destructor
ColorLightTask::~ColorLightTask(void)
{
	return;
};
	
// Main function of the task
int ColorLightTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the Color Lights Task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
    // General main loop (while in Autonomous or Tele mode)
	while (true) 
	{
		if(proxy->get("Joy3B4N")) //red
		{
			red.Set(Relay::kOn);
			white.Set(Relay::kOff);
			blue.Set(Relay::kOff);
		}
		else if(proxy->get("Joy3B3N")) //white
		{
			white.Set(Relay::kOn);
			red.Set(Relay::kOff);
			blue.Set(Relay::kOff);
		}
		else if(proxy->get("Joy3B5N")) //blue
		{
			blue.Set(Relay::kOn);
			red.Set(Relay::kOff);
			white.Set(Relay::kOff);
		}
		else if(proxy->get("Joy3B2N")) //clear all
		{
			red.Set(Relay::kOff);
			white.Set(Relay::kOff);
			blue.Set(Relay::kOff);
		}
		
		// Wait for our next lap
		WaitForNextLoop();
	}
	return (0);
};