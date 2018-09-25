// TGMTtemplate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include "TGMTcamera.h"
#include "TGMTConfig.h"
#include "TGMTdebugger.h"
#include "TGMTutil.h"
#include "TGMTfile.h"

bool autoCapture = false;
int interval;
clock_t _time = clock();

void OnKeyEvent(int key)
{
	if (key == KEY_ESC)
	{
		GetTGMTcamera()->StopCamera();
	}
	else if (key == KEY_ENTER)
	{
		autoCapture = !autoCapture;
	}
}

void OnCameraFrames(std::vector<cv::Mat> frames)
{
	for (int i = 0; i < frames.size(); i++)
	{		
		if (autoCapture && clock() - _time > interval)
		{
			_time = clock();
			WriteImage(frames[i], TGMTutil::FormatString("img\\%d.jpg", _time).c_str());
		}
		ShowImage(frames[i], "frame_%d", i);
		OnKeyEvent(cv::waitKey(1));
	}
}

void OnCameraStop()
{
}

int _tmain(int argc, _TCHAR* argv[])
{
	SET_CONSOLE_TITLE("Auto capture camera");
	GetTGMTConfig()->SetWriteDefaultValueIfNotExist(true);
	GetTGMTConfig()->LoadSettingFromFile();
	interval = GetTGMTConfig()->ReadValueInt("AutoCaptureCamera", "interval", 1000);

	TGMTfile::CreateDir("img");
	GetTGMTcamera()->OnNewFrames = OnCameraFrames;
	GetTGMTcamera()->OnStopped = OnCameraStop;

	GetTGMTcamera()->LoadConfig();



	GetTGMTcamera()->Start();

	cv::waitKey();
	getchar();
	return 0;
}

