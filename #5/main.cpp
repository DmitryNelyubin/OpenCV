#include <chrono>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using std::vector;

static int top{120}, bot{400}, height{167};
const int maxHeight{540}, maxBot{450}, maxTop{470};

void findLines(const Mat &bin, vector<Point2f> &centers) {
  Rect window(0, 0, 80, 80);
  vector<Point2f> nonZeros;

  for (window.y = 0; window.y + 80 <= bin.rows; window.y += 80) {
    for (window.x = 0; window.x + 40 < bin.cols; window.x += 40) {
      Mat clice = bin(window);
      vector<Point2f> roi;

      findNonZero(clice, roi);
      if (!roi.empty()) {
        for (unsigned i = 0; i < roi.size(); ++i) roi[i].x += window.x;
        nonZeros.insert(nonZeros.end(), roi.begin(), roi.end());
      } else if (!nonZeros.empty()) {
        centers.push_back(
            Point((nonZeros.front().x + nonZeros.back().x) / 2,
                  window.y + (nonZeros.front().y + nonZeros.back().y) / 2));
        nonZeros.clear();
      }
    }
    if (!nonZeros.empty()) {
      centers.push_back(
          Point((nonZeros.front().x + nonZeros.back().x) / 2,
                window.y + (nonZeros.front().y + nonZeros.back().y) / 2));
      nonZeros.clear();
    }
  }
}

void warp(Mat &src, Mat &dst) {
  Point2f src_vertices[4];
  src_vertices[0] = Point(488 - top, maxHeight - height);
  src_vertices[1] = Point(488 + top, maxHeight - height);
  src_vertices[2] = Point(505 + bot, src.rows - 1);
  src_vertices[3] = Point(505 - bot, src.rows - 1);

  Point2f dst_vertices[4];
  dst_vertices[0] = Point(0, 0);
  dst_vertices[1] = Point(dst.cols, 0);
  dst_vertices[2] = Point(dst.cols, dst.rows);
  dst_vertices[3] = Point(0, dst.rows);

  Mat M = getPerspectiveTransform(src_vertices, dst_vertices);
  warpPerspective(src, dst, M, dst.size());

  line(src, Point(488 - top, maxHeight - height),
       Point(488 + top, maxHeight - height), Scalar(0, 255, 0), 1);
  line(src, Point(488 + top, maxHeight - height),
       Point(505 + bot, src.rows - 1), Scalar(0, 255, 0), 1);
  line(src, Point(505 + bot, src.rows - 1), Point(505 - bot, src.rows - 1),
       Scalar(0, 255, 0), 1);
  line(src, Point(505 - bot, src.rows - 1),
       Point(488 - top, maxHeight - height), Scalar(0, 255, 0), 1);

  Mat bin;
  dst.copyTo(bin);

  vector<Point2f> centers;
  cvtColor(bin, bin, COLOR_BGR2GRAY);
  threshold(bin, bin, 180, 255, THRESH_BINARY);
  findLines(bin, centers);

  Mat reverseM = getPerspectiveTransform(dst_vertices, src_vertices);
  perspectiveTransform(centers, centers, reverseM);

  for (auto it : centers) {
    circle(src, it, 3, Scalar(0, 255, 0), FILLED);
  }
}

int main() {
  VideoCapture cap("solidWhiteRight.mp4");
  int time = 0;

  namedWindow("source");

  createTrackbar("Top", "source", &top, maxTop);
  createTrackbar("Bottom", "source", &bot, maxBot);
  createTrackbar("Height", "source", &height, maxHeight);

  while (true) {
    auto begin = std::chrono::steady_clock::now();
    Mat src;
    cap >> src;

    if (src.empty()) {
      fprintf(stdout, "End of video\n");
      break;
    }

    Mat dst;
    src.copyTo(dst);
    warp(src, dst);

    putText(src, "FPS: 25     Time: " + std::to_string(time) + "ms",
            Point(10, 50), FONT_HERSHEY_DUPLEX, 1., CV_RGB(255, 0, 0), 2);

    // time for FPS
    auto end = std::chrono::steady_clock::now();
    auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    imshow("source", src);
    imshow("result", dst);

    if (waitKey(1000 / 25 - ms.count()) == 27) break;

    // time to display
    auto endPrint = std::chrono::steady_clock::now();
    auto msPrint =
        std::chrono::duration_cast<std::chrono::milliseconds>(endPrint - begin);
    time += msPrint.count();
  }

  cap.release();
  return 0;
}
