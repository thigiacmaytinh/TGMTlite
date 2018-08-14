#include "CameraExample.h"
#include "TGMTcamera.h"
#include "TGMTConfig.h"
#include "TGMTutil.h"

void OnKeyEvent(int key)
{
	if (key == KEY_ESC)
	{
		GetTGMTcamera()->StopCamera();
	}
}

void OnCameraFrames(std::vector<cv::Mat> frames)
{
	for (int i = 0; i < frames.size(); i++)
	{
		PRINT_FPS(frames[i]);
		ShowImage(frames[i], "frame_%d", i);
		OnKeyEvent(cv::waitKey(1));
	}
}

void OnCameraStop()
{
}

void CameraExample::main(int argc, char** argv)
{
	GetTGMTConfig()->SetWriteDefaultValueIfNotExist(true);
	GetTGMTConfig()->LoadSettingFromFile();

	
	GetTGMTcamera()->OnNewFrames = OnCameraFrames;
	GetTGMTcamera()->OnStopped = OnCameraStop;

	GetTGMTcamera()->LoadConfig();

	

	GetTGMTcamera()->Start();
}