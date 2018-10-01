
#include <opencv2\opencv.hpp>
#include "ImagePreprocessor.h"

using namespace cv;

int main()
{
	ImagePreprocessor processor("C:\\Users\\grand_000\\Pictures\\Curvy-road.jpg", true, true);
	processor.drawLines(true, true);

	waitKey(0);
	destroyAllWindows();
	return 0;
}