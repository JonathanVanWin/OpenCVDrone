
#include <opencv2\opencv.hpp>
#include "ImagePreprocessor.h"

using namespace cv;




int main()
{
	ImagePreprocessor processor("C:\\Users\\grand_000\\Pictures\\original.bmp");
	processor.drawLines(true, false);
	namedWindow("Window");

	waitKey(0);
	destroyAllWindows();
	return 0;
}