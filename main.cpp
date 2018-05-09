#include <windows.h>      
#include <time.h>       /* time_t, struct tm, time, localtime */
#include <stdio.h>		/* puts, printf */
#include <tchar.h>
#include <iostream>
#include <string>
#include <future>  // for Async functions
#include <chrono>

using namespace std;

// if minutes or seconds is <10, it does not automatically put a 0 in front.  It looks a bit odd
//     ^------------v
void dispTime(tm mytime, bool dispSet) {  // recieves the local time in a "tm struct", and outputs to console.
	int dispHour = mytime.tm_hour;
	string amPm = "AM and ";
	string dispMin = to_string(mytime.tm_min);
	if(dispMin.size() < 2){//add a zero for formatting, if minute is 0-9.
	dispMin = '0'+ dispMin;
	}
	if (dispHour == 0) {
		dispHour = 12;
	}
	if (dispHour > 12) {// if afternoon, modify for PM.
		dispHour -= 12; amPm = "PM and ";
	}
	if (dispSet) {
		cout << "The current time is " << dispHour << ":" << dispMin << amPm << mytime.tm_sec << " seconds" << endl;
	}
	else if (!dispSet) {
		cout << "Would you like to set the alarm for " << dispHour << ":" << dispMin << amPm << mytime.tm_sec << " seconds" << endl;
	}
}


void checkTimeNeg(struct tm *checktime, struct tm *currenttime) {
// Makes sure that the new time is not before the current time.  
// If it is before, this function will change the new time to tomorrow's date.

	if (difftime(mktime(checktime), mktime(currenttime)) < 0) {
		/*  if negative -->increment day & day of week (so it won't be negative any more)
		if last day of month, increment month
		if last day of year, increment year  */

	// first:  set day of week.  0 = Sunday.
		if (checktime->tm_wday < 6) {
			(checktime->tm_wday)++;
		}
		else { checktime->tm_wday = 0; }

	// Determine the final day of the month.
		int maxDay = 0;  int curMon = checktime->tm_mon;
		if (curMon == 1) {  //Feb
			maxDay = 27;  		}
		else if (curMon == 0 || curMon == 2 || curMon == 4 || curMon == 6 || curMon == 7 || curMon == 9 || curMon == 11) {
			maxDay = 30;  // Jan, March, May, July, Aug, Oct, Dec all have 31 days.
		}
		else {  // April, June, Sept, Nov all have 30 days.
			maxDay = 29;		}
		
		checktime->tm_yday++;  //increment the day of the year (0-365).  Below it will be set to 0, if a new year.
	// Next increment the day (and month/year as necessary)
		if (checktime->tm_mday == maxDay) {
			//increment month, set to first day
			checktime->tm_mday = 0;
			if (checktime->tm_mon < 11) {
				checktime->tm_mon++;			}
			else {//increment year, set to first month
				checktime->tm_mon = 0;
				checktime->tm_year++;
				checktime->tm_yday = 0; }
		}
		else {
			checktime->tm_mday++;  //increment day of month (we've already determined it was not already the last day)
		}
	}
}

bool wait_for_User() {  // User must press ENTER to progress.
	cout << endl<< "Press <ENTER> to cancel alarm.";
	return std::cin.get();
}

void sound_the_Alarm() {
	Beep(800, 250);  // Beeps at 800hz for a quarter of a second.
	Beep(1600, 250);
	Beep(600, 250);
}

int alarm_Menu() {
	int choice = 0;
	system("cls");
	while (true) {
		cout << endl << "--------------| Welcome to the ALARM CLOCK main menu |--------------" << endl;
		cout << endl << endl << "1) Set alarm to go off at a certain time." << endl;
		cout << "2) Set alarm to go off after an elapsed duration.";
		cout << endl << endl << "0) Quit." << endl;
		cin >> choice;
		if (choice == 1 || choice == 2 || choice == 0) {
			return(choice);
		}
		else {
			system("cls");
			cout << choice << " was invalid.  Please enter a valid selection." << endl << endl;
		}
	}//end while loop
}

int main()
{ // -------------------------------------- MAIN PROGRAM--------------------------------------
	int nicksHour = 0;
	int nicksMin = 0;
	int nicksSec = 0;
	double timedif = 0;

	time_t rawtime;
	struct tm timeinfo;
	char buffer[256];

	time(&rawtime);  //gets the time
	localtime_s(&timeinfo, &rawtime);  //converts the time to local time (military time) 
	asctime_s(buffer, 256, &timeinfo);  //converts local time to a string format, puts it in "buffer".
										// format:  DayOfWeek Month Day MilitaryTime Year
	printf("Current local time and date: %s", buffer);
	struct tm anotherTime = timeinfo;

	// ------------Initialization complete --------------------

	int menu_pick = menu_pick = alarm_Menu();
	cout << "you selected: " << menu_pick << endl;

while(menu_pick){
if (menu_pick == 1) {
	time(&rawtime);  //gets the time
	localtime_s(&timeinfo, &rawtime);  //converts the time to local time (military time) 
	dispTime(timeinfo, true);

	cout << "Enter a time to set the alarm to (Military Time hh:mm:ss): " << endl;
	cout << "hour(0-23): ";  cin >> nicksHour;
	cout << "minutes(0-59): ";  cin >> nicksMin;
	cout << "seconds(0-59): ";  cin >> nicksSec;
	cout << endl;

	time(&rawtime);  //gets the time
	localtime_s(&timeinfo, &rawtime);  //converts the time to local time (military time) 
	dispTime(timeinfo, true);

	anotherTime = timeinfo;
	anotherTime.tm_hour = nicksHour;
	anotherTime.tm_min = nicksMin;
	anotherTime.tm_sec = nicksSec;

	checkTimeNeg(&anotherTime, &timeinfo);  // make sure the time entered has not already occurred.  
	dispTime(anotherTime, false);
	timedif = difftime(mktime(&anotherTime), mktime(&timeinfo));

	cout << "A total of " << timedif << " seconds? (y/n)" << endl;
}//end if menu_pick == 1.
else if(menu_pick == 2) {
		cout << "How long until the alarm goes off? (Military Time hh:mm:ss): " << endl;
		cout << "hours: ";  cin >> nicksHour;
		cout << "minutes: ";  cin >> nicksMin;
		cout << "seconds: ";  cin >> nicksSec;
		cout << endl;

		time(&rawtime);  //gets the time
		localtime_s(&timeinfo, &rawtime);  //converts the time to local time (military time) 
		dispTime(timeinfo, true);

		nicksMin = nicksMin + (nicksHour * 60);
		nicksSec = nicksSec + (nicksMin * 60);

		cout << "Would you like the alarm in " << nicksSec << " seconds? (y/n)" << endl;
}//end if menu_pick == 2.

		char sleepanswer = 'n';
		cin >> sleepanswer;
		int sleeptime = 0;
		if (sleepanswer == 'y' || sleepanswer == 'Y') {
			if (menu_pick == 1) {
				time(&rawtime);  //gets the time
				localtime_s(&timeinfo, &rawtime);  //converts the time to local time (military time) 
				sleeptime = difftime(mktime(&anotherTime), mktime(&timeinfo));  //update the alarm time.
			}//end case 1
			if (menu_pick == 2) {
				sleeptime = nicksSec;
			}//end case 2

			system("cls");
			time(&rawtime);  //gets the time
			localtime_s(&timeinfo, &rawtime);  //converts the time to local time (military time) 
			dispTime(timeinfo, true);

			cout << "Snooze for " << sleeptime << " seconds....";
			sleeptime = (sleeptime * 1000);
			Sleep(sleeptime);
			cout << "...Awake!" << endl;

		time(&rawtime);  //gets the time
		localtime_s(&timeinfo, &rawtime);  //converts the time to local time (military time) 
		dispTime(timeinfo, true);
		
		cin.ignore(1);  //clears the last ENTER press out of buffer.


		// Now to make the taskbar icon flash until they click the console window
		//typedef struct {
		//	UINT  cbSize;
		//	HWND  hwnd = GetConsoleWindow();
		//	DWORD dwFlags;
		//	UINT  uCount;
		//	DWORD dwTimeout;
		//} FLASHWINFO, *PFLASHWINFO;
		FLASHWINFO fi;
		fi.cbSize = sizeof(FLASHWINFO);  //size of structure (bytes)
		fi.hwnd = GetConsoleWindow();  //function returns the handle for current console window.
		fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;  // 1st flag flashes window & taskbar.  2nd flashes until window is in foreground.
		fi.uCount = 0;  // number of times to flash
		fi.dwTimeout = 0;  // rate (miliseconds) to flashing.  0 is system default rate.

		const auto future = std::async(std::launch::async, wait_for_User);
		int wait_count = 0;
		do {
			//cout << wait_count++ << flush;
			FlashWindowEx(&fi);  // Function that flashes the window
			sound_the_Alarm();
		} while (future.wait_for(chrono::seconds(1)) == future_status::timeout);  //waits for 1 second, or until result becomes available.
												// if result becomes available, it changes the ".wait_for" future_status to "ready", instead of "timeout"
		// The button has been pressed.

	}//end--"if confirmation of alarm set"

	menu_pick = alarm_Menu();
	cout << "you selected: " << menu_pick << endl;
}  // quit was selected.

	return 0;
	// Microsoft Example - https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/localtime-s-localtime32-s-localtime64-s
}
