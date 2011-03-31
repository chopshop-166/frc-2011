#pragma once

//CameraTask CameraObject;
//TimerTask TimerTaskObject;

// tasks that cannot run on suitcase
ArmTask ArmTaskObject; // Works, but not logging faults
//AutonomousAssistTask AutoAssistObject; // This thing's a little...wonky 
DriveTask DriveObject; // Works
ElevatorTask ElevatorObject; // Works, but not logging faults
MiniDeploy166 MiniDeployObject; // In progress
PhotoElectricTask PhotoElectricObject; // Works
PneumaticsTask PneumaticsTaskObject; // Works
//SonarTask SonarTaskObject; // Works

int BatteryLogTask(Proxy *proxy, Robot * lHandle) {
	static bool started = false;
	static SimpleLog battery_log("Battery");
	if(!started) {
		started = true;
		lHandle->RegisterLogger(&battery_log);
	}
	battery_log.PutOne(lHandle->dsHandle->GetBatteryVoltage());
	return 0;
}

SimpleTask BatteryLogTaskObject("166BatteryLogTask",BatteryLogTask,100);
