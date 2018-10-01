#pragma once
#include <opencv2\opencv.hpp>

using namespace cv;

class ImagePreprocessor
{
	
public:
	Mat m_src, m_imgGray, m_imgCanny;

	ImagePreprocessor(const char* path, bool withAutoThreshold = false, bool withPostering = false, bool withCanny = false);
	~ImagePreprocessor();
	void drawLines(bool withContours, bool drawAllContours);
	
};