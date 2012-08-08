#ifndef TIMER_H
#define TIMER_H

class Timer
{
public:
	~Timer();
	Timer();

public:
	void update();
	long long getTime();
	double get_fps();

private:
	long long time_old;
	long long time_new;
	long long time_rate;
	double fps;
};

#endif