#include "DashboardDataSender.h"
#include "Timer.h"

DashboardDataSender *DashboardDataSender::getInstance() {
	if(instance == NULL)
		instance = new DashboardDataSender;
	return instance;
}
/**
 * Send data to the dashboard.
 * This class sends two types of data to the dashboard program:
 * 1. Data representing all the ports on the robot
 * 2. Camera tracking data so the dashboard can annotate the video stream with
 *    target information.
 */
DashboardDataSender *DashboardDataSender::instance = 0;
DashboardDataSender::DashboardDataSender()
{
	// these timers make sure that the data is not sent to the dashboard more
	// than 10 times per second for efficiency.
	IOTimer = new Timer();
	visionTimer = new Timer();
	IOTimer->Start();
	visionTimer->Start();
	instance = this;
	proxy = Proxy::getInstance();
}

/**
 * Send IO port data to the dashboard.
 * Send data representing the output of all the IO ports on the cRIO to the dashboard.
 * This is probably not the best data to send for your robot. Better would be higher
 * level information like arm angle or collector status. But this is a sample and you're
 * free to modify it. Be sure to make the corresponding changes in the LabVIEW example
 * dashboard program running on your driver station.
 */
void DashboardDataSender::sendIOPortData(void) {
	if (IOTimer->Get() < 0.1)
		return;
	IOTimer->Reset();
	Dashboard &dash = DriverStation::GetInstance()->GetLowPriorityDashboardPacker();
	unsigned char solBuf=0;
	dash.AddCluster();
	{
		dash.AddCluster();
		{ //analog modules 
			dash.AddCluster();
			{
				for (int i = 1; i <= 8; i++) {
					dash.AddFloat((float) AnalogModule::GetInstance(1)->GetAverageVoltage(i));
				}
			}
			dash.FinalizeCluster();
			dash.AddCluster();
			{
				for (int i = 1; i <= 8; i++) {
					dash.AddFloat((float) AnalogModule::GetInstance(2)->GetAverageVoltage(i));
				}
			}
			dash.FinalizeCluster();
		}
		dash.FinalizeCluster();

		dash.AddCluster();
		{ //digital modules
			dash.AddCluster();
			{
				dash.AddCluster();
				{
					int module = 4;
					dash.AddU8(DigitalModule::GetInstance(module)->GetRelayForward());
					dash.AddU8(DigitalModule::GetInstance(module)->GetRelayReverse());
					dash.AddU16((short)DigitalModule::GetInstance(module)->GetDIO());
					dash.AddU16((short)DigitalModule::GetInstance(module)->GetDIODirection());
					dash.AddCluster();
					{
						for (int i = 1; i <= 10; i++) {
							dash.AddU8((unsigned char) DigitalModule::GetInstance(module)->GetPWM(i));
						}
					}
					dash.FinalizeCluster();
				}
				dash.FinalizeCluster();
			}
			dash.FinalizeCluster();

			dash.AddCluster();
			{
				dash.AddCluster();
				{
					int module = 6;
					dash.AddU8(DigitalModule::GetInstance(module)->GetRelayForward());
					dash.AddU8(DigitalModule::GetInstance(module)->GetRelayForward());
					dash.AddU16((short)DigitalModule::GetInstance(module)->GetDIO());
					dash.AddU16(DigitalModule::GetInstance(module)->GetDIODirection());
					dash.AddCluster();
					{
						for (int i = 1; i <= 10; i++) {
							dash.AddU8((unsigned char) DigitalModule::GetInstance(module)->GetPWM(i));
						}
					}
					dash.FinalizeCluster();
				}
				dash.FinalizeCluster();
			}
			dash.FinalizeCluster();
		}
		dash.FinalizeCluster();

		// Get the solenoids as an 8-bit int
		dash.AddCluster();
		{
			solBuf = 0;
			for(unsigned i=1;i<8;i++) {
				solBuf |= (Solenoid(i).Get() << (i-1));
			}
			dash.AddU8(solBuf);
		}
		dash.FinalizeCluster();
		// Add anything specific you want to send, like stuff from the proxy, here 
	}
	dash.FinalizeCluster();
	dash.Finalize();
}
