#include "ImagePreprocessor.h"



ImagePreprocessor::ImagePreprocessor(const char* path)
{
	m_src = imread(path);
	Mat postering;
	//fastNlMeansDenoising(g_src, g_src);
	/*pyrMeanShiftFiltering(g_src, postering, 20, 40, 2);
	cvtColor(postering, img_gray, COLOR_BGR2GRAY);
	GaussianBlur(img_gray, img_gray, Size(5, 5), 0);
	threshold(img_gray, img_gray, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	erode(img_gray, img_gray, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1,-1),2);
	dilate(img_gray, img_gray, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)), Point(-1, -1), 1);
	Canny(img_gray, g_img_canny, 120, 150, 3, true);
	morphologyEx(g_img_canny, g_img_canny, MORPH_CLOSE, getStructuringElement(CV_SHAPE_RECT, Size(9, 9)));

	imshow("Canny", g_img_canny);*/
	cvtColor(m_src, m_imgGray, COLOR_BGR2GRAY);
	GaussianBlur(m_imgGray, m_imgGray, Size(5, 5), 0);

	threshold(m_imgGray, m_imgCanny, 125, 255, THRESH_BINARY);
	//morphological opening (remove small objects from the foreground)
	erode(m_imgCanny, m_imgCanny, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(m_imgCanny, m_imgCanny, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	//morphological closing (fill small holes in the foreground)
	dilate(m_imgCanny, m_imgCanny, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(m_imgCanny, m_imgCanny, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
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
