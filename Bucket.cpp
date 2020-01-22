#include <vector>
#include <iostream>

#include <gtest/gtest.h>
#include <glog/logging.h>

#include "Bucket.h"

using namespace std;


int msb(uint64_t n)
{
	int shift = 0;

	while (true) {
		uint64_t n_div_256 = n >> 8;

		if (n_div_256 == 0) {
			return shift + (int)BYTE_MSB[n];
		}

		n = n_div_256;
		shift += 8;
	}
	//should not be here
	log_assert(0);
	return -1;
}

bool Bucket::Insert(uint64_t number) {
	uint32_t index = msb(number);
	uint32_t max = max_ - 1;
	if (index > max) {
		bucket_[max][0]++;
	} else {
		bucket_[index][0]++;
	}
	return true;
}


bool Bucket::Insert(uint64_t number, int req_cnt, int req_size) {

	if (req_cnt <= 0) {
		LOG(ERROR) << "request count should be at least 1";
		//log_assert(0);
		return false;
	}

	std::lock_guard<std::mutex> lock(mutex_);

	req_cnt_ += req_cnt;
	req_size_ += req_size;

	auto latency_index = msb(number);
	int max = max_ - 1;

#if 0
	LOG(ERROR) << "name:" << name_
		<< " req_cnt:" << req_cnt
		<< " req_cnt_:" << req_cnt_
		<< " latency:" << number
		<< " index: " << latency_index
		<< " req_size:" << req_size
		<< " req_size_:" << req_size_;
#endif
	if (latency_index > max) {
		bucket_[max][req_cnt]++;
	} else {
		bucket_[latency_index][req_cnt]++;
	}

	if (req_cnt_ >= log_frequency_) {
		Dump();
		Clear();
	}
	return true;
}


void Bucket::SetLogFrequencyWithReqCount(uint64_t req_cnt) {
	log_frequency_ = req_cnt;
	LOG(ERROR) << "New log frequency is " << log_frequency_;

}


void Bucket::Init() {
	bucket_.reserve(max_);
	bucket_.resize(max_, std::vector<int>(max_batch_size_));
	for (auto & batch : bucket_) {
		batch.reserve(max_batch_size_);
		std::fill(batch.begin(), batch.end(), 0);
	}
}


bool Bucket::Clear() {
	req_cnt_ = 0;
	req_size_ = 0;
	for (auto & batch : bucket_) {
		std::fill(batch.begin(), batch.end(), 0);
	}
	return true;
}


bool Bucket::Dump() {
	std::stringstream header;
	header << "name:" << name_ 
			<< ",bucket size:" << bucket_.size()
			<< ",io count:" << req_cnt_
			<< ",io size:" << req_size_ ;

	LOG(ERROR) << header.str();

	uint32_t lat_index=0;

	for (auto latency_elem : bucket_) {
		bool should_log = false;
		std::stringstream batch_stream;
		batch_stream << "[" << lat_index << "]" << " ";

		uint32_t batch_index=0;
		for (auto batch_elem : latency_elem) {
			if (batch_elem != 0) {
				should_log = true;
				batch_stream << batch_index << ":" << batch_elem << ",";
			}
			batch_index++;
		}

		if (lat_index == 0 || lat_index == 1) {
			lat_index++;
		} else {
			lat_index *= multiplication_factor_;
		}
		should_log && LOG(ERROR) << batch_stream.str();
	}
	return true;
}

