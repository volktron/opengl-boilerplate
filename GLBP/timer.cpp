#include "timer.h"
#include <sstream>
#include <windows.h>

Timer::Timer()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&time_rate);
	fps = 99999999.0f;
	time_new = 0;
	time_old = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)&time_start);

	this->update();
}

Timer::~Timer()
{
	std::ostringstream ss;
	ss << "Time: " << time_local << "\nfps: " << fps << "\ndelta: " << this->get_delta() << std::endl;
	std::string s(ss.str());
	OutputDebugString(s.c_str());
}

void Timer::update()
{
	time_old = time_new;
	QueryPerformanceCounter((LARGE_INTEGER*)&time_new);
	time_local = 1.0 * (double)(time_new - time_start)/(time_rate * 1.0);
	fps = 1.0 / ((double)(time_new - time_old)  /(time_rate));
}

long long Timer::get_time()
{
	return time_new / (time_rate / 1000);
}

double Timer::get_time_local()
{
	return time_local;
}

double Timer::get_fps()
{
	return fps;
}

double Timer::get_delta()
{
	return 1.0f/fps;
}
