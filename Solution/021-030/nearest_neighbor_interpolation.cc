#include"Solution/021-030/nearest_neighbor_interpolation.h"
#include"Solution/geometric_transform.h"

#include<opencv2/highgui.hpp>

namespace {

const double kScale = 1.5;

}
namespace digital {

//brief:
//becare:具体实现位于geometric_transform.[h/cc]中
void NearestNeighborInterpolation::operator()() {
	cv::Mat data = cv::imread(getPath());
	if (data.empty()) {
		dealException(kFileError);
		return;
	}

	cv::Mat img;
	
	cv::Size size = data.size();
	size.width = static_cast<int>(kScale * size.width);
	size.height = static_cast<int>(kScale * size.height);

#ifndef USE_OPENCVLIB

	cv::Mat M = cv::getRotationMatrix2D(cv::Point(0, 0), 0, kScale);
	cv::warpAffine(data, img, M, size, cv::INTER_NEAREST);
	
#else

	//获得仿射变换矩阵
	cv::Mat M = detail::getRotationMatrix2D(cv::Point(0, 0), 0, kScale, kScale);
	//进行仿射变换
	detail::warpAffine(data, img, M, size, cv::INTER_NEAREST, cv::Scalar::all(128));

#endif

	//cv::Mat test;
	//cv::warpAffine(data, test, dd, size, cv::INTER_NEAREST);
	//show(&data, &test);

	if (needShowOriginal())
		show(&data, &img);
	else
		show(&img);

}


}