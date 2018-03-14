//---------------------------------------------------------------------------
#include "Performance.h"
#include "Windows.h"
//---------------------------------------------------------------------------

//non-static
Performance::Performance() {
	QueryPerformanceFrequency(&ticks_PerSecond);
	start();
}

//non-static
void Performance::start() {
	QueryPerformanceCounter(&tick_START);
}

//non-static
void Performance::stop() {
	QueryPerformanceCounter(&tick_END);
}

//non-static
double Performance::getMicroseconds() {
	return calculateMicroseconds(tick_START.QuadPart, tick_END.QuadPart, ticks_PerSecond.QuadPart);
}

//non-static
double Performance::getMilliseconds() {
	return getMicroseconds() / 1000.0;
}

//non-static
double Performance::getMicrosecondsFromStart() {
	return calculateMicroseconds(tick_START.QuadPart, getCurTick(), ticks_PerSecond.QuadPart);
}

//non-static
double Performance::getMillisecondsFromStart() {
	return getMicrosecondsFromStart() / 1000.0;
}

//static
long long Performance::getCurTick() {
	LARGE_INTEGER tick;
	QueryPerformanceCounter(&tick);
	return tick.QuadPart;
}

//static
double Performance::getMicroseconds(long long startTick) {
	return calculateMicroseconds(startTick, getCurTick(), getTicksPerSecond());
}

//static
double Performance::getMilliseconds(long long startTick) {
	return getMicroseconds(startTick) / 1000.0;
}

//static
double Performance::getMicroseconds(long long startTick, long long endTick) {
	return calculateMicroseconds(startTick, endTick, getTicksPerSecond());
}

//static
double Performance::getMilliseconds(long long startTick, long long endTick) {
	return getMicroseconds(startTick, endTick) / 1000.0;
}

//static
long long Performance::getTicksPerSecond() {
	LARGE_INTEGER tmp_TicksPerSecond;
	QueryPerformanceFrequency(&tmp_TicksPerSecond);
	return tmp_TicksPerSecond.QuadPart;
}

//static
double Performance::calculateMicroseconds(long long start, long long end, long long tps) {
	return (1000000.0*((double)(end - start) / (double)tps));
}
