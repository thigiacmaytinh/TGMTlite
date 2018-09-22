#include "TGMTobjDetect.h"
#include "TGMTfile.h"
#include "TGMTdebugger.h"
#include "TGMTimage.h"


TGMTobjDetect* TGMTobjDetect::m_instance = nullptr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGMTobjDetect::TGMTobjDetect()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGMTobjDetect::~TGMTobjDetect()
{
	m_cascade.~CascadeClassifier();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string Mat2BinaryString(cv::Mat frame)
{
	cv::Mat frameGray, frameBW;
	//convert to gray
	cvtColor(frame, frameGray, CV_BGR2GRAY);
	adaptiveThreshold(frameGray, frameBW, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 0);
	std::string result;
	for (int row = 0; row < frameBW.rows; ++row)
	{
		for (int col = 0; col < frameBW.cols; ++col)
		{
			result += frameBW.at<char>(row, col) + 1; //+1 because values in [-1;0]
		}
	}
	return result;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<cv::Rect> TGMTobjDetect::Detect(cv::Mat matInput, float scaleFactor, int minNeighbors, cv::Size minSize, cv::Size maxSize)
{
	
	ASSERT(matInput.data, "Load image failed");

	std::vector<cv::Rect> result;
	

	cv::Mat matGray = matInput.clone();
	TGMTimage::ConvertToGray(matGray);

	// Enhance / Normalise the image contrast (optional)
	//cv::equalizeHist(detectImg, detectImg);

	StartCountTime("Detect");
	
	m_cascade.detectMultiScale(matGray, result, scaleFactor, minNeighbors, CV_HAAR_SCALE_IMAGE);
	StopCountTime("Detect");



	return 	result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<cv::Rect> TGMTobjDetect::Detect(std::string filePath, float scaleFactor, int minNeighbors, cv::Size minSize, cv::Size maxSize)
{
	return Detect(cv::imread(filePath), scaleFactor, minNeighbors);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TGMTobjDetect::Init(std::string cascadeFile)
{
	m_cascade = cv::CascadeClassifier(cascadeFile);
	ASSERT(!m_cascade.empty(), "Cascade file is empty");

	return !m_cascade.empty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<cv::Rect> TGMTobjDetect::FindTemplateMatching(cv::Mat matToFind, cv::Mat matTemplate, int method)
{
	ASSERT(method != CV_TM_CCORR && method != CV_TM_CCOEFF, "Method CV_TM_CCORR & CV_TM_CCOEFF not precision");

	std::vector<cv::Rect> rects;
	if (matTemplate.rows > matToFind.rows || matTemplate.cols > matToFind.cols)
	{
		PrintError("Mat template must be smaller than matInput");
		return rects;
	}

	cv::Mat matDisplay, result;
	matToFind.copyTo(matDisplay);
	int result_cols = matToFind.cols - matTemplate.cols + 1;
	int result_rows = matToFind.rows - matTemplate.rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);
	bool method_accepts_mask = (CV_TM_SQDIFF == method || method == CV_TM_CCORR_NORMED);

	bool use_mask = false; //not implemented yet
	if (use_mask && method_accepts_mask)
	{
		cv::Mat mask;
		cv::matchTemplate(matToFind, matTemplate, result, method, mask);
	}
	else
	{
		cv::matchTemplate(matToFind, matTemplate, result, method);
	}
	
	cv::threshold(result, result, 0.9, 1.0, CV_THRESH_TOZERO);
	double minval, maxval, threshold = 0.9;
	cv::Point minloc, maxloc;
	while (true)
	{			
		cv::minMaxLoc(result, &minval, &maxval, &minloc, &maxloc);

		if (maxval > threshold)
		{
			rects.push_back(cv::Rect(maxloc, cv::Point(maxloc.x + matTemplate.cols, maxloc.y + matTemplate.rows)));
			cv::floodFill(result, maxloc, cv::Scalar(0), 0, cv::Scalar(0.1), cv::Scalar(1.0));
		}
		else
			break;
	}
	return rects;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MANAGED
#include "TGMTbridge.h"
using namespace System;
using namespace System::Drawing;
using namespace TGMT;

std::vector<cv::Rect> TGMTobjDetect::Detect(System::String^ filePath, float scaleFactor, int minNeighbors, cv::Size minSize, cv::Size maxSize)
{
	std::string sFilePath = TGMT::TGMTbridge::SystemStr2stdStr(filePath);
	std::vector<cv::Rect> rects = Detect(sFilePath, scaleFactor, minNeighbors, minSize, maxSize);
	return rects;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TGMTobjDetect::Init(System::String^ cascadeFilePath)
{
	return Init(TGMT::TGMTbridge::SystemStr2stdStr(cascadeFilePath));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TGMTobjDetect::Inited()
{
	return !m_cascade.empty();
}
#endif