#pragma once
#include "Windows.h"

/** class Performance
 * 
 * Klasa Performance wspiera proces mierzenia czasu trwania pojedynczych
 * instrukcji kodu lub ca�ych fragment�w kodu.
 *
 * Spos�b u�ycia
 * Performance p;
 * p.start(); //tu zaczyna pomiar czasu
 * ...; //tu Tw�j kod
 * ...; //tu Tw�j kod
 * ...; //tu Tw�j kod
 * p.stop(); // koniec mierzenia czasu
 *
 * double ileTrwalo = p.getMicroseconds(); //funkcja zwraca ostatni pomiar (mi�dzy ostatnim wywo�aniem stop() a ostatnim wywo�aniem start()) w mikrosekundach!!!!
 *
 * UWAGA! Mo�esz raz w��czy� "stoper" funkcj� p.start(),
 * a potem wo�a� w dowolnych miejscach p.stop() i mierzy� czas jaki up�yn�� od ostatniego p.start().
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

	//podstawowa funkcja ze wzorem na liczenie czasu mi�dzy startem a endem
	static double calculateMicroseconds(long long start, long long end, long long tps);
};
