// TGMTtemplate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include "TGMTdebugger.h"
#include "TGMTfile.h"
#include "TGMTblob.h"

int low = 140;
int high = 190;

cv::Mat mat;

void BrainHemorrhageOnTrackbar(int, void*)
{
	cv::Mat matBin;

	cv::threshold(mat, matBin, high, 255, CV_THRESH_TOZERO_INV);
	cv::threshold(matBin, matBin, low, 255, CV_THRESH_TOZERO);
	cv::threshold(matBin, matBin, 0, 255, CV_THRESH_BINARY);

	cv::imshow("brain", matBin);

	auto blobs = TGMTblob::FindBlobs(matBin, cv::Size(20, 20), cv::Size(300, 300));

	cv::Mat matDraw = matBin.clone();
	for (int i = 0; i < blobs.size(); i++)
	{
		float ratio = (float)blobs[i].points.size() / blobs[i].boundingRect.area();
		if (ratio < 0.3)
			continue;

		matDraw = TGMTblob::DrawBlob(matDraw, blobs[i], cv::Point(), RED);
	}

	cv::imshow("brain_output", matDraw);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void main(int argc, char** argv)
{
	std::vector<std::string> imagePaths = TGMTfile::GetImageFilesInDir(TGMTfile::GetCurrentDir());
	ASSERT(imagePaths.size() > 0, "Can not load any image");

	cv::Mat mat = cv::imread(imagePaths[0]);
	ASSERT(mat.data, "Can not load image");

	cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);
	cv::equalizeHist(mat, mat);

	cv::namedWindow("brain");
	cv::createTrackbar("low", "brain", &low, 255, BrainHemorrhageOnTrackbar);
	cv::createTrackbar("high", "brain", &high, 255, BrainHemorrhageOnTrackbar);
	cv::imshow("brain", mat);

	BrainHemorrhageOnTrackbar(0, nullptr);
}


