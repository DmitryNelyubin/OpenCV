#include <chrono>
#include <opencv2/opencv.hpp>

using namespace cv;

static int top{118}, bot{395}, height{167};
const int maxHeight{540}, maxBot{450}, maxTop{470};

void warp(const Mat &src, Mat &dst) {
  Point2f src_vertices[4];
  src_vertices[0] = Point(488 - top, maxHeight - height);
  src_vertices[1] = Point(488 + top, maxHeight - height);
  src_vertices[2] = Point(505 + bot, src.rows - 1);
  src_vertices[3] = Point(505 - bot, src.rows - 1);

  Point2f dst_vertices[4];
  dst_vertices[0] = Point(0, 0);
  dst_vertices[1] = Point(640, 0);
  dst_vertices[2] = Point(640, 480);
  dst_vertices[3] = Point(0, 480);

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

    Mat dst(480, 640, CV_8UC3);
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
