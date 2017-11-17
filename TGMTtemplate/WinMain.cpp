// TGMTtemplate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>

int _tmain(int argc, _TCHAR* argv[])
{
	cv::Mat mat = cv::imread("lena.jpg");
	cv::imshow("Lena", mat);

	cv::waitKey();
	getchar();
	return 0;
}

