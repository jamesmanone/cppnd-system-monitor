#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const { return cpuUtil_; }

// Calculate cpu utl (% of core, not % of total)
void Process::CalcCpu() {
  auto fields = LinuxParser::CpuUtilization(Pid());
  long hz = sysconf(_SC_CLK_TCK);

  long uptime = (fields["sysUp"] * hz) - fields["starttime"];
  long elapsed = (uptime - uptime_);
  uptime_ = uptime;
  long ttime = (long)(fields["utime"] + fields["stime"]);
  long ntime = ttime - time_;
  time_ = ttime;

  if (elapsed != 0)
    cpuUtil_ = (float)ntime / (float)elapsed;
  else
    cpuUtil_ = 0.0;
}

// Return the command that generated this process
string Process::Command() const { return command_; }

// Set command_
void Process::Command(std::string command) { command_ = command; }

// Return this process's memory utilization
string Process::Ram() {
  return LinuxParser::Ram(Pid()).substr(
      0, 7);  // ram should always be 6 digits and a decimal
}

// Return the username that generated this process
string Process::User() const { return user_; }

// set user_. Padded/truncated to fit 7 char display width
void Process::User(std::string user) {
  while (user.size() < 7) user += " ";
  if (user.size() > 7) user = user.substr(0, 7);
  user_ = user;
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_ / sysconf(_SC_CLK_TCK); }

// < overload for sort decending
bool Process::operator<(Process const& a) const {
  return (CpuUtilization() > a.CpuUtilization());
}

// == overload for find by pid
bool Process::operator==(int const& a) const { return Pid() == a; }

// < (int) overload for find/sort by pid
bool Process::operator<(int const& a) const { return Pid() < a; }