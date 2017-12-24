#include "Timer.h"

void Timer::start() {
	this->begin = std::chrono::high_resolution_clock::now();
}

long long Timer::getNanoTime(){
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	std::ostringstream ss;
	ss << std::chrono::duration_cast<std::chrono::nanoseconds>(end-this->begin).count();
	std::string timeStr = ss.str();
	return stoll(timeStr);
}

long double Timer::getTime() {
	long double fact = pow(10,9);
	return this->getNanoTime() / fact;
}

long double Timer::getMillis() {
	long double fact = pow(10,6);
	return this->getNanoTime() / fact;
}
