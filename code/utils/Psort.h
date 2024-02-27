/*****************************************************************************
* @brief : 多线程排序算法，内置多线程，使用方法同单线程排序
* 经测试,和c++17的并行排序速度基本在同一量级，但是整体速度略快
* @author : acedtang
* @date : 2021/7/25
* @version : ver 1.0
* @inparam :
* @outparam :
*  https://github.com/wiltchamberian/Algorithms/blob/main/PSort.h
*****************************************************************************/

#ifndef __PSORT_H 
#define __PSORT_H

#include <thread>
#include <future>
#include <algorithm>
#include <cassert>
#include <cmath>

namespace Sun {
//这两个参数可以适当调整 ，主要用于数据量少时直接在当前线程排序
#define MINIMUM_FOR_MULTITHREAD 64  //must not be too small!  支持多线程的最小元素个数

#define MINIMUM_FOR_ONE_FRAGMENT 2   //单个线程片段最小处理个数（否则会采用单线程)

	inline int find_neareset_higher_which_is_power_of_two(int val) {
		int n = val - 1;
		n = n | (n >> 1);
		n = n | (n >> 2);
		n = n | (n >> 4);
		n = n | (n >> 8);
		n = n | (n >> 16);
		return n + 1;
	}

	//threadNum为0，则会自动计算最合适的线程数，否则会用指定的线程数
	template<class _RandomIt,class _Compare>
	void parralled_sort(_RandomIt begin, _RandomIt end, _Compare compare,unsigned int threadNum = 0) {
		static int numCore = std::thread::hardware_concurrency();	
		int siz = std::distance(begin, end);
		if (threadNum ==1 || numCore <=1||siz< MINIMUM_FOR_MULTITHREAD) {
			std::sort(begin, end, compare);
			return;
		}
		int numThread = numCore;
		if (threadNum > 0) {
			numThread = threadNum;
		}
		else {
			bool ok = !(numCore&(numCore-1));
			if (!ok) {
				numThread = find_neareset_higher_which_is_power_of_two(numCore);
			}
			numThread *= 2;//this cof is a experience 
		}
		
		//将siz均分为numThread个部分，分别排序
		int basicFragSiz = siz / numThread;
		if (basicFragSiz < MINIMUM_FOR_ONE_FRAGMENT) {
			std::sort(begin, end, compare); return;
		}
		//如果resiude>0,则把它们平摊到前residue个线程上
		int residue = siz - basicFragSiz * numThread;
		
		//确定开始多线程处理
		//1 分片;
		std::vector<std::pair<_RandomIt, _RandomIt>> prs(numThread);
		auto iter = begin;
		for (int i = 0; i < numThread -1 ; ++i) {
			prs[i].first = iter;
			int fragSiz = basicFragSiz + (residue > 0 ? 1 : 0);
			prs[i].second = iter + fragSiz;
			iter += fragSiz;
			residue--;
		}
		prs.back().first = iter;
		prs.back().second = end;

		//2，异步处理
		std::vector<std::future<void>> futures(numThread);
		for (int i = 0; i < numThread; ++i) {
			futures[i] = std::async(std::launch::async, std::sort<_RandomIt, _Compare>, prs[i].first, prs[i].second,compare);
		}

		//等待片段结束
		for (int i = 0; i < numThread; ++i) {
			futures[i].get();
		}

		//开始多线程归并
		int k = log2(numThread);
		int d = 2;
		for (int i = 0; i < k; ++i) {
			std::vector<std::future<void>> tmpfutures;
			for (int j = 0; j < numThread; j +=d) {
				auto start = prs[j].first;
				auto end = prs[j + d - 1].second;
				auto mid = prs[j + (d>>1)].first;
				auto fr = std::async(std::launch::async,std::inplace_merge<_RandomIt, _Compare>,start, mid, end, compare);
				tmpfutures.push_back(std::move(fr));
			}
			for (int j = 0; j < numThread / d; ++j) {
				tmpfutures[j].get();
			}
			d = d << 1;
		}
		
		return;
	}



}

#endif