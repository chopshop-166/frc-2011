// <<CHANGEME>>
#pragma once
// Declare your task objects here
// Example:
// DriveClass DriveObject;
// No need for pointers, or anything else, if it's derived from Team166Task
#define SUITCASE_BOT 0

// tasks that run everywhere


#if not SUITCASE_BOT
// tasks that cannot run on suitcase
DriveTask DriveObject;

#else
//suitcase only tasks here

#endif
