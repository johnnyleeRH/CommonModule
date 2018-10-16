// 计数排序，ref: http://t.cn/E7D8V9R  by程序员小灰
// 适用场景：有限范围内的整数排序
// 不足：数列最大最小值差距过大时不适用；数列元素非整数不适用，局限性可以考虑桶排序
#include <iostream>
using namespace std;

int* countSort(const int* array, const int len) {
	//1.得到数列的最大值和最小值，并算出差值d
	int max = array[0];
	int min = array[0];
	for (int i = 1; i < len; ++i) {
		if (array[i] > max)
			max = array[i];
		if (array[i] < min)
			min = array[i];
	}
	int d = max - min;

	//2.创建统计数组并统计对应元素个数
	int* countarray = new int[d + 1];
	for (int i = 0; i < len; ++i)
		countarray[array[i] - min]++;

	//3.统计数组做变形，后面的元素等于前面的元素之和，为了保证顺序稳定
	int sum = 0;
	for (int i = 0; i < d + 1; ++i) {
		sum += countarray[i];
		countarray[i] = sum;
	}

	//4.倒序遍历原始数列，从统计数组找到正确位置，输出到结果数组
	int *sorted = new int[len];
	for (int i = len - 1; i >= 0; --i) {
		sorted[countarray[array[i] - min] - 1] = array[i]; 
		--countarray[array[i] - min];
	}
	delete [] countarray;
	return sorted;
}

int main() {
	int src[] = {95, 94, 91, 98, 99, 90, 99, 93, 91, 92};
	int* dst = countSort(src, sizeof(src)/sizeof(int));
	for (int i = 0; i < sizeof(src)/sizeof(int); ++i)
		cout << dst[i] << " ";
	cout << endl;
	delete [] dst;
}