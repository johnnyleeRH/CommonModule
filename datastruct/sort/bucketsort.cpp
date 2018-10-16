// 桶排序，解决计数排序只能处理整数的问题，ref: http://t.cn/E7FpAaR by程序员小灰
// 适用于桶内元素均衡的场景
#include <iostream>
#include <vector>
using namespace std;

void bubblesort(vector<double>& src) {
	bool swap = false;
	double tmp;
	if (src.size() < 2)
		return;
	for (int i = 0; i < src.size() - 1; ++i) {
		for (int j = 0; j < src.size() - i - 1; ++j) {
			if (src[j] > src[j + 1]) {
				tmp = src[j];
				src[j] = src[j + 1];
				src[j + 1] = tmp;
				swap = true;
			}
		}
		if (!swap)
			break;
		swap = false;
	}
}

double* bucketsort(const double* array, const int len) {
	//1.得到数列的最大值和最小值，并算出差值d
	double max = array[0];
	double min = array[0];
	for (int i = 1; i < len; ++i) {
		if (array[i] > max)
			max = array[i];
		if (array[i] < min)
			min = array[i];
	}
	double d = max - min;

	//2.初始化桶
	vector<vector<double> > bucketvec;
	for (int i = 0; i < len; ++i)
		bucketvec.push_back(vector<double>());
	//3.遍历原始数组，将每个元素放入桶中，桶元素区间跨度(最大值-最小值)/(桶的数量-1)
	//元素按比例定位：(array[i] - min) * (bucketnum - 1)/d
	for (int i = 0; i < len; ++i)
		bucketvec[(array[i] - min) * (len - 1) / d].push_back(array[i]);

	//4.对每个通内部进行排序, 这里用冒泡实现，可以使用O(nlogn)的排序（快速/归并等）实现
	for (int i = 0; i < len; ++i)
		bubblesort(bucketvec[i]);

	//5.输出全部元素
	double *dst = new double[len];
	int id = 0;
	for (int i = 0; i < len; ++i)
		for (int j = 0; j < bucketvec[i].size(); ++j)
			dst[id++] = bucketvec[i][j];
	return dst;
}

int main() {
	double src[] = {4.12, 6.421, 0.0023, 3.0, 2.123, 8.122, 4.12, 10.09};
	double* dst = bucketsort(src, sizeof(src)/sizeof(src[0]));
	for (int i = 0; i < sizeof(src)/sizeof(src[0]); ++i)
		cout << dst[i] << " ";
	cout << endl;
	delete [] dst;

}