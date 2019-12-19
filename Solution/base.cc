#include"Solution/base.h"
#include"Solution/type_extension.h"

#include<opencv2/imgproc.hpp>

#include<random>
#include<vector>



namespace detail {

static std::default_random_engine __global_eng;
static std::normal_distribution<> __global_dis(1, 1);
//brief:返回服从正态分布的随机数，E = 1 , D = 1
//becare:不是线程安全的
//      也可以采用RNG对象生成
double randNorm() {
	return __global_dis(__global_eng);
}

//breif:计算阶乘
//FIXME:尾递归实现
int factorial(int i) {
	assert(i > -1);

	if (i == 0 || i == 1)
		return 1;
	return i * factorial(i - 1);
}


//brief:以二项式展开式作为非归一化高斯系数的近似
//return:差分算子结果为列向量
cv::Mat getSmoothKernel(int n)
{
	cv::Mat kernel = cv::Mat::zeros(cv::Size(n, 1), CV_32FC1);
	auto cur = kernel.ptr<float>(0, 0);
	for (int i = 0; i < n; i++){
		//利用二项式展开式作为高斯平滑的近似
		*cur++ = static_cast<float>(detail::factorial(n - 1) / (detail::factorial(i) * detail::factorial(n - 1 - i)));
	}
	return kernel;
}

//brief:得到soble差分算子,
//return:差分算子结果为列向量
cv::Mat getSobelDifference(int n)
{
	cv::Mat diff = cv::Mat::zeros(cv::Size(n, 1), CV_32FC1);
	//差分算子的前身是n-2阶的二项式展开式，与平滑算子相差1
	cv::Mat prev = getSmoothKernel(n - 1);
	//利用差分算子的获得过程，改变Mat
	auto cur = diff.ptr<float>(0, 0);
	auto iter = prev.ptr<float>(0, 0);
	*cur++ = -1;
	for (int i = 1; i < n - 1; i++)
	{
		*cur++ = (*iter - *(iter + 1));
		++iter;
	}
	*cur = 1;
	return diff;
}

//brief:二维卷积
void convolution2D(const cv::Mat& src, 
	cv::Mat& dst, 
	int ddepth, 
	const cv::Mat& kernel, 
	cv::Point p , 
	int bordertype) 
{
	assert(!src.empty());
	cv::Mat flip_k;
	cv::flip(kernel, flip_k, -1);

	//fort test
	//std::vector<float> vk = std::vector<float>(flip_k);

	cv::filter2D(src, dst, ddepth, flip_k, p, 0.0, bordertype);
}

//brief:分离的二维卷积
void sepConvolution2D(const cv::Mat& src, 
	cv::Mat& dst, 
	int ddepth,
	const cv::Mat& kernelx,
	const cv::Mat& kernely, 
	cv::Point p , 
	int bordertype) 
{
	assert(!src.empty());
	cv::Mat flip_kx;
	cv::Mat flip_ky;
	cv::flip(kernelx, flip_kx, -1);
	cv::flip(kernely, flip_ky, -1);

	//fort test
	//std::vector<float> vx = std::vector<float>(kernelx);
	//std::vector<float> vy = std::vector<float>(flip_ky);

	cv::sepFilter2D(src, dst, ddepth, flip_kx, flip_ky, p, 0.0, bordertype);
}

//brief:d
void Sobel(cv::Mat& src, cv::Mat& dst, int ddepth, int dx, int dy, int win) {
	assert((win & 0x1) == 0x1);

	cv::Mat smooth_kernel;
	cv::flip(detail::getSmoothKernel(win), smooth_kernel, -1);
	//利用函数得到差分算子
	cv::Mat diff_kernel;
	cv::flip(detail::getSobelDifference(win), diff_kernel, -1);

#ifndef NDEBUG
	//for test
	std::vector<int> v = std::vector<int>(smooth_kernel);
	std::vector<int> v2 = std::vector<int>(diff_kernel);
#endif

	if(dx)
		cv::sepFilter2D(src, dst, CV_16S, smooth_kernel, diff_kernel.t());

	if(dx==0 && dy)
		cv::sepFilter2D(src, dst, CV_16S, diff_kernel, smooth_kernel.t());

}



}//!namespace detail