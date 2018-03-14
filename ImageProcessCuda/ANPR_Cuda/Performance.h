#pragma once
#include "Windows.h"

/** class Performance
 * 
 * Klasa Performance wspiera proces mierzenia czasu trwania pojedynczych
 * instrukcji kodu lub ca³ych fragmentów kodu.
 *
 * Sposób u¿ycia
 * Performance p;
 * p.start(); //tu zaczyna pomiar czasu
 * ...; //tu Twój kod
 * ...; //tu Twój kod
 * ...; //tu Twój kod
 * p.stop(); // koniec mierzenia czasu
 *
 * double ileTrwalo = p.getMicroseconds(); //funkcja zwraca ostatni pomiar (miêdzy ostatnim wywo³aniem stop() a ostatnim wywo³aniem start()) w mikrosekundach!!!!
 *
 * UWAGA! Mo¿esz raz w³¹czyæ "stoper" funkcj¹ p.start(),
 * a potem wo³aæ w dowolnych miejscach p.stop() i mierzyæ czas jaki up³yn¹³ od ostatniego p.start().
 */

class Performance
{
public:
	Performance();
	void start();
	void stop();
	/** returns microseconds between last start() and stop() */
	double getMicroseconds();
	/** returns milliseconds between last start() and stop() */
	double getMilliseconds();
	/** returns microseconds between last start() and now */
	double getMicrosecondsFromStart();
	/** returns milliseconds between last start() and now */
	double getMillisecondsFromStart();
	/** returns current tick */
	static long long getCurTick();
	/** returns microseconds between startTick and now */
	static double getMicroseconds(long long startTick);
	/** returns milliseconds between startTick and now */
	static double getMilliseconds(long long startTick);
	/** returns microseconds between startTick and  stopTick*/
	static double getMicroseconds(long long startTick, long long endTick);
	/** returns milliseconds between startTick and  stopTick*/
	static double getMilliseconds(long long startTick, long long endTick);

	static long long getTicksPerSecond();

protected:
	LARGE_INTEGER tick_START;
	LARGE_INTEGER tick_END;
	LARGE_INTEGER ticks_PerSecond;

	//podstawowa funkcja ze wzorem na liczenie czasu miêdzy startem a endem
	static double calculateMicroseconds(long long start, long long end, long long tps);
};
