// TGMTtemplate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include "CameraExample.h"
#include "TGMTConfig.h"
#include "TGMTcamera.h"
#include "TGMTobjDetect.h"
#include "TGMTdraw.h"


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
		PRINT_FPS(frames[i]);

		std::vector<cv::Rect> rects = GetTGMTobjDetect()->Detect(frames[i]);
		TGMTdraw::DrawRectangles(frames[i], rects, 1, BLUE);
		ShowImage(frames[i], "frame_%d", i);


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

	GetTGMTobjDetect()->Init("cascade.xml");

	GetTGMTcamera()->OnNewFrames = OnCameraFrames;
	GetTGMTcamera()->OnStopped = OnCameraStop;

	GetTGMTcamera()->LoadConfig();



	GetTGMTcamera()->Start();

	cv::waitKey();
	getchar();
	return 0;
}

