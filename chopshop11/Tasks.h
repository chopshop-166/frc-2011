// <<CHANGEME>>
#pragma once
// Declare your task objects here
// Example:
// DriveClass DriveObject;
// No need for pointers, or anything else, if it's derived from Team166Task
#define SUITCASE_BOT 0

// tasks that run everywhere
//CameraTask CameraObject;


#if SUITCASE_BOT
//suitcase only tasks here


#else
// tasks that cannot run on suitcase
AutonomousAssistTask AutoAssistObject;
ColorLightTask ColorLightObject;
//DriveTask DriveObject;
HeightTask166 HeightTaskObject;
MiniDeploy166 MiniDeployObject;
PhotoElectricTask PhotoElectricObject;
PneumaticsTask PneumaticsTaskObject;
SonarTask SonarTaskObject;
TimerTask TimerTaskObject;
#endif
