#include <opencv2/opencv.hpp>

using namespace cv;

void customSmoothing(const Mat& src, Mat& result);
void customGradient(const Mat& src, Mat& result);

int main(int argc, char** argv) {
  if (argc != 2) return -1;

  Mat image, imageRes;
  image = imread(argv[1], 1);

  GaussianBlur(image, imageRes, Size(15, 15), 0, 0);
  namedWindow("Pussy Gaussian", WINDOW_AUTOSIZE);
  imshow("Pussy Gaussian", imageRes);

  customSmoothing(image, imageRes);
  namedWindow("Pussy Custom", WINDOW_AUTOSIZE);
  imshow("Pussy Custom", imageRes);

  cvtColor(image, image, COLOR_BGR2GRAY);
  customGradient(image, imageRes);
  namedWindow("Pussy Gradient", WINDOW_AUTOSIZE);
  imshow("Pussy Gradient", imageRes);

  waitKey(0);
  return 0;
}

void customSmoothing(const Mat& src, Mat& result) {
  Mat kernel(5, 5, CV_64F);

  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      kernel.at<double>(i, j) = 1. / 25.;
    }
  }

  filter2D(src, result, -1, kernel);
}

void customGradient(const Mat& src, Mat& result) {
  Mat kernel(3, 3, CV_32FC1);

  kernel.at<double>(0, 0) = -3;
  kernel.at<double>(0, 1) = -10;
  kernel.at<double>(0, 2) = -3;
  kernel.at<double>(1, 0) = 0;
  kernel.at<double>(1, 1) = 0;
  kernel.at<double>(1, 2) = 0;
  kernel.at<double>(2, 0) = 3;
  kernel.at<double>(2, 1) = 10;
  kernel.at<double>(2, 2) = 3;

  filter2D(src, result, -1, kernel);
}
