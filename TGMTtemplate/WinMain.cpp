// TGMTtemplate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include "CameraExample.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CameraExample::main(argc, argv);

	cv::waitKey();
	getchar();
	return 0;
}

