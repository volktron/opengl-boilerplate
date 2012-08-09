#include "timer.h"
#include <windows.h>

Timer::Timer()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&time_rate);
	fps = 99999999;
	time_new = 0;
	time_old = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)&time_start);
}

Timer::~Timer()
{

}

void Timer::update()
{
	time_old = time_new;
	QueryPerformanceCounter((LARGE_INTEGER*)&time_new);
	time_local = (long double)(time_new - time_start);
	fps = 1000.0 / ((double)(long double)(time_new - time_old) / (long double)(time_rate) * 1000.0);
}

long long Timer::get_time()
{
	return time_new / (time_rate / 1000);
}

long long Timer::get_time_local()
{
	return time_local;
}

double Timer::get_fps()
{
	return fps;
}
