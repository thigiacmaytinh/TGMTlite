// TGMTtemplate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include "CameraExample.h"
#include "TGMTConfig.h"
#include "TGMTcamera.h"
#include "TGMTobjDetect.h"
#include "TGMTdraw.h"
#include "TGMTdebugger.h"
#include "TGMTfile.h"

bool saveUndetected;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OnKeyEvent(int key)
{
	if (key == KEY_ESC)
	{
		GetTGMTcamera()->StopCamera();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OnCameraFrames(std::vector<cv::Mat> frames)
{
	for (int i = 0; i < frames.size(); i++)
	{
		std::vector<cv::Rect> rects = GetTGMTobjDetect()->Detect(frames[i]);
		if (saveUndetected && rects.size() == 0)
		{
			WriteImage(frames[i], ("img\\" + GetCurrentDateTime(true) + ".png").c_str());
		}
		TGMTdraw::DrawRectangles(frames[i], rects, 1, BLUE);
		ShowImage(frames[i], "frame_%d", i);

		PRINT_FPS(frames[i]);
		OnKeyEvent(cv::waitKey(1));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OnCameraStop()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	GetTGMTConfig()->SetWriteDefaultValueIfNotExist(true);
	GetTGMTConfig()->LoadSettingFromFile();
	saveUndetected = GetTGMTConfig()->ReadValueBool("CascadeDetector", "save_undetected");
	if (saveUndetected)
	{
		TGMTfile::CreateDir("img");
	}
	GetTGMTobjDetect()->Init("cascade.xml");

	GetTGMTcamera()->OnNewFrames = OnCameraFrames;
	GetTGMTcamera()->OnStopped = OnCameraStop;

	GetTGMTcamera()->LoadConfig();



	GetTGMTcamera()->Start();

	cv::waitKey();
	getchar();
	return 0;
}

