// c++11
#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <sstream>
#include <cmath>
#ifndef TIMER_H
#define TIMER_H
class Timer {
public:
	void start();	
	long long getNanoTime();
	long double getTime();
	long double getMillis();
private:
	std::chrono::high_resolution_clock::time_point begin;
};

#endif /* TIMER_H */
