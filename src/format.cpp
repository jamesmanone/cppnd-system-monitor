#include <sstream>
#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  string hh, mm, ss;
  int hours, minutes;
  hours = (long)(seconds / 3600);
  seconds -= (hours * 3600);
  minutes = (long)(seconds / 60);
  seconds -= (minutes * 60);
  hh = hours > 9 ? std::to_string(hours) : "0" + std::to_string(hours);
  mm = minutes > 9 ? std::to_string(minutes) : "0" + std::to_string(minutes);
  ss = seconds > 9 ? std::to_string(seconds) : "0" + std::to_string(seconds);
  return hh + ":" + mm + ":" + ss;
}