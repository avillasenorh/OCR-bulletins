#include <stdio.h>

/************************************************************************
* Function: iaddsec1
*
* Purpose: adds an integer number of seconds to a valid time expressed
* as (year, month, hr, day, min, sec).  Increases minutes, hours, and
* days if neccessary, but not months!  The reason for this is that these
* times are used to write in ISC 96-byte format, which does not allow
* phase arrival times with different reference months (for a given event)
*
* This function does not account for leap seconds
*
* Author: Antonio Villasenor, USGS, Golden
* Date:   17-FEB-1999
************************************************************************/

struct itm { 
	int     year; 
	int     month; 
	int     day; 
	int     hour; 
	int     minute; 
	int     second;
	int     millisecond; 
};

struct itm iaddsec1(struct itm RefTime, int dt)
{
	struct itm NewTime;
	struct itm nerr = {0, 0, 0, 0, 0, 0, 0};

	/* First check that RefTime is a valid time */

	if (RefTime.month < 1 || RefTime.month > 12) {
		fprintf(stderr, "Invalid month: %d\n", RefTime.month);
		return nerr;
	}
	/* Here calculate number of days in this month and year */
/*
	dom = days_in_month(RefTime.year, RefTime.month);
	if (RefTime.day < 0 || RefTime.hour > dom) {
		fprintf(stderr, "Invalid day: %d\n", RefTime.day);
		return nerr;
	}
*/
	if (RefTime.hour < 0 || RefTime.hour >= 24) {
		fprintf(stderr, "Invalid hour: %d\n", RefTime.hour);
		return nerr;
	}
	if (RefTime.minute < 0 || RefTime.minute >= 60) {
		fprintf(stderr, "Invalid minute: %d\n", RefTime.minute);
		return nerr;
	}
	if (RefTime.second < 0 || RefTime.second >= 60) {
		fprintf(stderr, "Invalid second: %d\n", RefTime.second);
		return nerr;
	}
	if (RefTime.millisecond < 0 || RefTime.millisecond >= 999) {
		fprintf(stderr, "Invalid millisecond: %d\n", RefTime.millisecond);
		return nerr;
	}

	NewTime = RefTime;

	NewTime.second += dt;

	/* correct seconds (does not correct for leap seconds) */

	if (NewTime.second >= 60) 
		do {
			NewTime.minute++;
			NewTime.second -= 60;
		} while (NewTime.second >= 60);
	else if (NewTime.second < 0)
		do {
			NewTime.minute --;
			NewTime.second += 60;
		} while (NewTime.second < 0);
	else
		return NewTime;

	/* correct minutes */

	if (NewTime.minute >= 60)
		do {
			NewTime.hour++;
			NewTime.minute -= 60;
		} while (NewTime.minute >= 60);
	else if (NewTime.minute < 0)
		do {
			NewTime.hour--;
			NewTime.minute += 60;
		} while (NewTime.minute < 0);
	else
		return NewTime;

	/* correct hours */

	if (NewTime.hour >= 24)
		do {
			NewTime.day++;
			NewTime.hour -= 24;
		} while (NewTime.hour >= 24);
	else if (NewTime.hour < 0)
		do {
			NewTime.day--;
			NewTime.hour += 24;
		} while (NewTime.hour < 0);
	else
		return NewTime;

	/* iaddsec1 does not correct days that fall outside a month
	(day <= 0, or day > number of days in the month)
	because in ISS 96-byte format files, the reference month
	for one event cannot be changed */

	return NewTime;
}
