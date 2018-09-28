#include "ImagePreprocessor.h"

// if withAutoThreshold = true, OTSU algorithm will find optimum thresold
// if withPostering = true, we will get a filtered “posterized” image with color gradients and fine-grain texture flattened.
// if withCanny = true, we will use an edge detection technique, for finding the boundaries of objects within images.
ImagePreprocessor::ImagePreprocessor(const char* path, bool withAutoThreshold, bool withPostering, bool withCanny)
{
	m_src = imread(path);
	Mat postering = m_src;
	if (withPostering)
		pyrMeanShiftFiltering(m_src, postering, 20, 40, 2); //This values work fairly well

	cvtColor(postering, m_imgGray, COLOR_BGR2GRAY);
	GaussianBlur(m_imgGray, m_imgGray, Size(5, 5), 0);

	if (withAutoThreshold)
		threshold(m_imgGray, m_imgCanny, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	else
		threshold(m_imgGray, m_imgCanny, 125, 255, THRESH_BINARY);

	if (withCanny)
		Canny(m_imgCanny, m_imgCanny, 120, 150, 3);

	// MORPH_OPEN = Remove small objects from the img
	morphologyEx(m_imgCanny, m_imgCanny, MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)), Point(-1, -1), 2);

	// MORPH_CLOSE = Fill small holes in the img
	morphologyEx(m_imgCanny, m_imgCanny, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	imshow("Canny", m_imgCanny);
	imshow("Window", m_imgGray);
}

ImagePreprocessor::~ImagePreprocessor()
{
}

void ImagePreprocessor::drawLines(bool withContours, bool drawAllContours)
{
	if (withContours)
	{
		std::vector<std::vector<Point> > contours; // Vector for storing contours
		std::vector<Vec4i> hierarchy;

		findContours(m_imgCanny, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0)); // Find the contours in the image

		RNG rng(12345);
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

		if (drawAllContours)
		{
			for (size_t i = 0; i < contours.size(); i++) // iterate through each contour.
			{
				drawContours(m_src, contours, i, color, 2, 8, hierarchy, 0, Point());
			}
		}
		else
		{
			int largest_area = 0;
			int largest_contour_index = 0;
			Rect bounding_rect;

			for (size_t i = 0; i < contours.size(); i++) // iterate through each contour.
			{
				double area = contourArea(contours[i]);  //  Find the area of contour

				if (area > largest_area)
				{
					largest_area = area;
					largest_contour_index = i;               //Store the index of largest contour
					bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
				}
			}

			drawContours(m_src, contours, largest_contour_index, color, 2, 8, hierarchy, 0, Point()); // Draw the largest contour using previously stored index.
			m_src = m_src(bounding_rect);
		}
		imshow("Contours", m_src);
	}
	else
	{
		Mat cdst;
		cvtColor(m_imgCanny, cdst, CV_GRAY2BGR);

		std::vector<Vec2f> lines;
		// detect lines
		HoughLines(m_imgCanny, lines, 1, CV_PI / 180, 150, 0, 0);

		// draw lines
		for (size_t i = 0; i < lines.size(); i++)
		{
			float rho = lines[i][0], theta = lines[i][1];
			Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));
			line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
		}
		imshow("detected lines", cdst);
	}
}
