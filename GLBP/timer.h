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
	double get_time_local();
	float get_fps();
	float get_delta();

private:
	long long time_old;
	long long time_new;
	long long time_rate;
	long long time_start;
	float fps;
	float time_local;
};

#endif