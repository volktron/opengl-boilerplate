#ifndef TIMER_H
#define TIMER_H

class Timer
{
public:
	~Timer();
	Timer();

public:
	void update();
	long long get_time();
	long long get_time_local();
	double get_fps();
	

private:
	long long time_old;
	long long time_new;
	long long time_rate;
	long long time_start;
	double fps;
	double time_local;
};

#endif