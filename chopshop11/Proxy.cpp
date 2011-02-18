/*******************************************************************************
*  Project   		: Framework
*  File Name  		: Proxy.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Code for Proxy class to hold shared variables
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "wpilib.h"
#include "Robot.h"
#include <string>

// Enable proxy logging?
#define LoggingProxy (0)

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

map<string,pair<float, SEM_ID> > Proxy::data = map<string,pair<float, SEM_ID> >();
map<string,int> Proxy::tracker = map<string,int>();
short Proxy::newpress_values[NUMBER_OF_JOYSTICKS][NUMBER_OF_JOY_BUTTONS];

Proxy *Proxy::ProxyHandle = 0;

/**
 * @brief Starts the Proxy166 task.
 */
Proxy::Proxy(void):
	stick1(1), stick2(2), stick3(3), stick4(4)
{
	//
	// Add the built in storage areas
	//
	manualJoystick[0]=manualJoystick[1]=manualJoystick[2]=manualJoystick[3]=false;
	static bool runonce = 0;
	if (runonce == 0) {
		ProxyHandle = this;
		for (int switchid=1; switchid < NUMBER_OF_SWITCHES+1; switchid++) {
			//Add switches to storage
			add("Switch" + switchid);
		}
		for (int analoginid=1; analoginid < NUMBER_OF_ANALOG_IN; analoginid++) {
			add("AnalogIn" + analoginid);
		}
		// Lets do this the easy way:
		for (int joyid=1; joyid <NUMBER_OF_JOYSTICKS+1; joyid++) {
			char tmp[32];
			sprintf(tmp, "Joy%d", joyid);
			string joywid = tmp;
			add(joywid + "X");
			add(joywid + "Y");
			add(joywid + "Z");
			add(joywid + "R");
			add(joywid + "T");
			add(joywid + "BT");
			add(joywid + "BTN");
			//Add Buttons, and newpress
			for (int buttonid=1;buttonid<NUMBER_OF_JOY_BUTTONS+1;buttonid++) {
				char tmp[32];
				sprintf(tmp, "%sB%d", joywid.c_str(), buttonid);
				string butwid = tmp;
				add(butwid);
				add(butwid + "N");
			}
		}
		//Make sure they're only added once
		runonce = 1;
	}
	add("matchtimer");
	// Start the actual task
	Start((char *)"166ProxyTask", PROXY_CYCLE_TIME);
}

Proxy::~Proxy(void)
{
	for(map<string,pair<float, SEM_ID> >::iterator it = data.begin();it != data.end();it = data.begin()) {
		del(it->first);
	}
	return;
}
/**
 * @brief Main thread function for Proxy166.
 * Runs forever, until MyTaskInitialized is false. 
 * 
 * @todo Update DS switch array
 */
int Proxy::Main(	int a2, int a3, int a4, int a5,
					int a6, int a7, int a8, int a9, int a10) {
	
	Robot *lHandle = NULL;
	WaitForGoAhead();
	
	lHandle = Robot::getInstance();
	Timer matchTimer;
	
	while(MyTaskInitialized) {
		setNewpress();
		if(lHandle->IsOperatorControl() && true) {
			if(manualJoystick[0]) {
				SetJoystick(1, stick1);
			}
			if(manualJoystick[1]) {
				SetJoystick(2, stick2);
			}
			if(manualJoystick[2]) {
				SetJoystick(3, stick3);
			}
			if(manualJoystick[3]) {
				SetJoystick(4, stick4);
			}
		}
		if(!lHandle->IsEnabled()) {
			matchTimer.Reset();
			// It became disabled
			matchTimer.Stop();
			set("matchtimer",0);
		} else {
			// It became enabled
			matchTimer.Start();
			if(lHandle->IsAutonomous()) {
				set("matchtimer",max( 15 - matchTimer.Get(),0));
			} else {
				set("matchtimer",max(120 - matchTimer.Get(),0));
			}
		}
		// The task ends if it's not initialized
		WaitForNextLoop();
	}
	return 0;
}

void Proxy::SetEnhancedIO()
{
	Robot *lHandle = Robot::getInstance();
	for(int i=1; i<NUMBER_OF_SWITCHES+1; i++) {
		char tmp[32];
		sprintf(tmp, "Switch%d", i);
		add(tmp);
		set(tmp, lHandle->dsHandle->GetDigitalIn(i));
	}
	for(int i=1; i<NUMBER_OF_ANALOG_IN+1; i++) {
		char tmp[32];
		sprintf(tmp, "AnalogIn%d", i);
		add(tmp);
		set(tmp, lHandle->dsHandle->GetAnalogIn(i));
	}
}

void Proxy::setNewpress()
{
	for(unsigned joy_id=1;joy_id < NUMBER_OF_JOYSTICKS+1; joy_id++) {
		for(unsigned btn_id=1;btn_id < NUMBER_OF_JOY_BUTTONS+1; btn_id++) {
			char tmp[32];
			sprintf(tmp, "Joy%dB%d", joy_id, btn_id);
			string name = tmp;
			bool buttonval = (bool)get(name);
			if(buttonval) {
				newpress_values[joy_id-1][btn_id-1]++;
			} else {
				newpress_values[joy_id-1][btn_id-1] = 0;
			}
			if(newpress_values[joy_id-1][btn_id-1]==1) {
				set(name + "N", 1.0);
				if(tracker.find(name) != tracker.end()) {
					tracker[name]++;
				}
			} else {
				set(name + "N", 0.0);
			}
			if(newpress_values[joy_id-1][btn_id-1]>1) {
				newpress_values[joy_id-1][btn_id-1] = 2;
			}
		}
		char tmp[32];
		sprintf(tmp, "Joy%dBTN", joy_id);
		string bn = tmp;
		sprintf(tmp, "Joy%dB1N", joy_id);
		set(bn,get((string)tmp));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

// This must be called to add values to the data field
bool Proxy::add(string name)
{
	for(unsigned i=0;i<name.size();i++) {
		name[i] = toupper(name[i]);
	}
	if(data.find(name) == data.end()) {
		data[name] = make_pair(0.0,semBCreate(SEM_Q_PRIORITY, SEM_FULL));
		return true;
	} else {
		return false;
	}
}

// This will get a Proxy value
float Proxy::get(string name, bool reset)
{
	for(unsigned i=0;i<name.size();i++) {
		name[i] = toupper(name[i]);
	}
	if(data.find(name) == data.end()) {
		Robot::getInstance()->DriverStationDisplay("Proxy ERR: %s", name.c_str());
		printf("Proxy::get cannot find variable: `%s`\n", name.c_str());
		TASK_DESC errtask;
		taskInfoGet(taskIdSelf(),&errtask);
		printf("\tFrom task: %s\n",errtask.td_name);
		return 0;
	}
	semTake(data[name].second, WAIT_FOREVER);
	float ret = data[name].first;
	data[name].first = (reset)? 0 : data[name].first;
	semGive(data[name].second);
	return ret;
}

// Set a new proxy value
float Proxy::set(string name, float val)
{
	for(unsigned i=0;i<name.size();i++) {
		name[i] = toupper(name[i]);
	}
	if(data.find(name) == data.end()) {
		Robot::getInstance()->DriverStationDisplay("Proxy ERR: %s", name.c_str());
		printf("Proxy::set cannot find variable: `%s`\n", name.c_str());
		TASK_DESC errtask;
		taskInfoGet(taskIdSelf(),&errtask);
		printf("\tFrom task: %s\n",errtask.td_name);
		return 0;
	}
	semTake(data[name].second, WAIT_FOREVER);
	data[name].first = val;
	semGive(data[name].second);
	return val;
}

// Stop tracking a variable in Proxy
bool Proxy::del(string name)
{
	for(unsigned i=0;i<name.size();i++) {
		name[i] = toupper(name[i]);
	}
	if(data.find(name) != data.end()) {
		semTake(data[name].second, WAIT_FOREVER);
		semDelete(data[name].second);
		data.erase(name);
		return true;
	} else {
		return false;
	}
}

// Tell if a value exists in the proxy
bool Proxy::exists(string name)
{
	for(unsigned i=0;i<name.size();i++) {
		name[i] = toupper(name[i]);
	}
	return(data.find(name) != data.end());
}

/**
 * @brief Sets a cached joystick value.
 * @param joy_id Which joystick to set the cached value for.
 * @param stick A Joystick object with the X, Y, and Z axes set, as well as each of the buttons.
 */
void Proxy::SetJoystick(int joy_id, Joystick & stick)
{
	wpi_assert(joy_id < NUMBER_OF_JOYSTICKS+1 && joy_id >= 0);
	string name;
	char tmp[32];
	sprintf(tmp, "Joy%d", joy_id);
	name = tmp;
	set(name + 'X', stick.GetX());
	set(name + 'Y', stick.GetY());
	set(name + 'Z', stick.GetZ());
	set(name + 'R', stick.GetTwist());
	set(name + 'T', stick.GetThrottle());
	string bname;
	for(unsigned i=1;i<NUMBER_OF_JOY_BUTTONS+1;i++) {
		char tmp1[32];
		sprintf(tmp1, "%sB%d", name.c_str(), i);
		bname = tmp1;
		set(bname,stick.GetRawButton(i));
		
	}
	set(name+"BT", stick.GetTrigger());
}

/**
 * @brief Gets the pending amount of times a button was pressed and released since last call.
 * @param joystick_id Which joystick to check
 * @param button_id Which button on the joystick to check
 * @return How many times the button was pressed and released since last call.
 */
int Proxy::GetPendingCount(string JoyButton) {
	for(unsigned i=0;i<JoyButton.size();i++) {
		JoyButton[i] = toupper(JoyButton[i]);
	}
	wpi_assertWithMessage(tracker.size() == 0, "Tried to fetch pending count for a non-registered button.");
	map<string,int>::iterator it = tracker.find(JoyButton);
	wpi_assertWithMessage(it != tracker.end(), "Tried to fetch pending count for a non-registered button.");
	int tmp=(it->second);
	it->second = 0;
	return (tmp);
}

/**
 * @brief Register a button to track the number of times it was pressed.
 * @param joystick_id Which joystick to track a button on
 * @param button_idd Which button on the joystick to track
 */
bool Proxy::RegisterCounter(string JoyButton) {
	for(unsigned i=0;i<JoyButton.size();i++) {
		JoyButton[i] = toupper(JoyButton[i]);
	}
	if(tracker.find(JoyButton) == tracker.end()) {
		tracker[JoyButton] = 0;
		return true;
	} else {
		return false;
	}
}

/**
 * @brief Unregister a button being tracked.
 * @param joystick_id Which joystick to track a button on
 * @param button_idd Which button on the joystick to track
 */
bool Proxy::UnregisterCounter(string JoyButton) {
	for(unsigned i=0;i<JoyButton.size();i++) {
		JoyButton[i] = toupper(JoyButton[i]);
	}
	return tracker.erase(JoyButton);
}
/**
 * @brief Whether a joystick is registered for tracking
 * @param JoyButton Joystick and Button 
 * @return Whether it is registered.
 */
bool Proxy::IsRegistered(string JoyButton) {
	for(unsigned i=0;i<JoyButton.size();i++) {
		JoyButton[i] = toupper(JoyButton[i]);
	}
	return (tracker.find(JoyButton) != tracker.end());
}

Proxy* Proxy::getInstance(void)
{
	if(ProxyHandle == 0) {
		ProxyHandle = new Proxy;
	}
	return ProxyHandle;
}

void Proxy::UseUserJoystick(int stick, bool manual) {
	wpi_assert(stick >= 1 && stick <= 4);
	manualJoystick[stick-1] = manual;
}

bool Proxy::AreSettingJoysticks() {
	return (manualJoystick[0]|manualJoystick[1]|manualJoystick[2]|manualJoystick[3]);
}
void Proxy::ToggleSettingJoysticks(bool in) {
	manualJoystick[0]=manualJoystick[1]=manualJoystick[2]=manualJoystick[3]=in;
}
