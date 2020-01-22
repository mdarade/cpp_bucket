#pragma once

#include <iostream>
#include <memory>
#include <atomic>
#include <mutex>


#include "CommonMacros.h"


using namespace std;
//------------------------------------------------
// BYTE_MSB[n] returns the position of the most
// significant bit. If no bits are set (n = 0) it
// returns 0. Otherwise the positions are 1 ... 8
// from low to high, so e.g. n = 13 returns 4:
//
//      bits:       0  0  0  0  1  1  0  1
//      position:   8  7  6  5 [4] 3  2  1
//
static const char BYTE_MSB[] = {
        0, 1, 2, 2, 3, 3, 3, 3,  4, 4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5,  5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6,  6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6,  6, 6, 6, 6, 6, 6, 6, 6,

        7, 7, 7, 7, 7, 7, 7, 7,  7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7,  7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7,  7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7,  7, 7, 7, 7, 7, 7, 7, 7,

        8, 8, 8, 8, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8, 8,

        8, 8, 8, 8, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8, 8
};


class Bucket {
	public:
		Bucket(std::string name, uint32_t start, uint32_t max, uint32_t mf) :
			name_(name), start_(start), max_(max), multiplication_factor_(mf) {
				Init();
			}
		void Init();
		bool Insert(uint64_t number);
		bool Insert(uint64_t number, int req_cnt, int req_size);
		bool Clear();
		bool Dump();
		void SetLogFrequencyWithReqCount(uint64_t req_cnt);
		std::string GetName() { return name_; }

	private:
		std::string name_;
		uint32_t start_{0};
		uint32_t max_;
		uint32_t multiplication_factor_{2};
		vector<vector<int>> bucket_;
		uint64_t log_frequency_{512};
		std::atomic<uint64_t> req_cnt_{0};
		std::atomic<uint64_t> req_size_{0};
		std::mutex mutex_;
		uint32_t max_batch_size_{128};
		//unit milliseconds
};

