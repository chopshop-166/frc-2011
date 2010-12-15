/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __AXIS_CAMERA_H__
#define __AXIS_CAMERA_H__

#include <taskLib.h>
#include <vxWorks.h> 
#include <sockLib.h> 
#include <inetLib.h>

#include "AxisCameraParams.h"
#include "ColorImage.h"
#include "HSLImage.h"
#include "nivision.h"
#include <set>
#include "Task.h"

/**
 * AxisCamera class.
 * This class handles everything about the Axis 206 FRC Camera.
 * It starts up 2 tasks each using a different connection to the camera:
 * - image reading task that reads images repeatedly from the camera
 * - parameter handler task in the base class that monitors for changes to
 *     parameters and updates the camera
 */
class AxisCamera: public AxisCameraParams
{
	AxisCamera(const char *cameraIP = "192.168.0.90");
public:
	virtual ~AxisCamera();
	static AxisCamera& GetInstance();
	void DeleteInstance();

	bool IsFreshImage();
	SEM_ID GetNewImageSem();

	int GetImage(Image *imaqImage);
	int GetImage(ColorImage *image);
	HSLImage *GetImage();

	int CopyJPEG(char **destImage, int &destImageSize, int &destImageBufferSize);

private:
	static int s_ImageStreamTaskFunction(AxisCamera *thisPtr);
	int ImageStreamTaskFunction();

	int ReadImagesFromCamera();
	void UpdatePublicImageFromCamera(char *imgBuffer, int imgSize);

	virtual void RestartCameraTask();

	static AxisCamera *m_instance;
	int m_cameraSocket;
	typedef std::set<SEM_ID> SemSet_t;
	SemSet_t m_newImageSemSet;

	char* m_protectedImageBuffer;
	int m_protectedImageBufferLength;
	int m_protectedImageSize;
	SEM_ID m_protectedImageSem;
	bool m_freshImage;

	Task m_imageStreamTask;
};

extern "C" {
	void AxisCameraStart();
	int AxisCameraGetImage(Image *image);
	void AxisCameraDeleteInstance();
	int AxisCameraFreshImage();

	// Mid-stream gets & writes
	void AxisCameraWriteBrightness(int brightness);
	int AxisCameraGetBrightness();
	void AxisCameraWriteWhiteBalance(AxisCameraParams::WhiteBalance_t whiteBalance);
	AxisCameraParams::WhiteBalance_t AxisCameraGetWhiteBalance();
	void AxisCameraWriteColorLevel(int colorLevel);
	int AxisCameraGetColorLevel();
	void AxisCameraWriteExposureControl(AxisCameraParams::Exposure_t exposure);
	AxisCameraParams::Exposure_t AxisCameraGetExposureControl();
	void AxisCameraWriteExposurePriority(int exposurePriority);
	int AxisCameraGetExposurePriority();
	void AxisCameraWriteMaxFPS(int maxFPS);
	int AxisCameraGetMaxFPS();

	// New-Stream gets & writes
	void AxisCameraWriteResolution(AxisCameraParams::Resolution_t resolution);
	AxisCameraParams::Resolution_t AxisCameraGetResolution();
	void AxisCameraWriteCompression(int compression);
	int AxisCameraGetCompression();
	void AxisCameraWriteRotation(AxisCameraParams::Rotation_t rotation);
	AxisCameraParams::Rotation_t AxisCameraGetRotation();
}

#endif
