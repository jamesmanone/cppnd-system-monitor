#include <unistd.h>
#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Find element in sorted vector<T>. Must be able to compare T to S by ++ and <
template <typename T, typename S>
long findSorted(vector<T> v, S q) {
  long i, index{-1}, first{0}, last{(long)v.size() - 1};

  while (first <= last) {
    i = (long)((last - first) / 2) + first;
    if (v[i] == q) {
      index = i;
      break;
    } else if (v[i] < q) {
      first = i + 1;
    } else {
      last = i - 1;
    }
  }
  return index;
}

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

int System::findProcess(int pid) {
  // sort by pid
  if (sortP_ != sortOrder::kPid) {
    std::sort(processes_.begin(), processes_.end(),
              [](Process a, Process b) { return a.Pid() < b.Pid(); });
    sortP_ = sortOrder::kPid;
  }

  return findSorted<Process, int>(processes_, pid);
}

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  auto pids = LinuxParser::Pids();
  std::sort(pids.begin(), pids.end());
  for (auto pid : pids) {
    int index = findProcess(pid);
    if (index == -1) {
      processes_.emplace_back(Process(pid));
      processes_.back().User(LinuxParser::User(pid));
      string command = LinuxParser::Command(pid);
      processes_.back().Command(command);

      sortP_ = sortOrder::kNone;
      index = processes_.size() - 1;
    }
    processes_[index].CalcCpu();
  }

  // Remove dead processes
  vector<int> dead;
  for (Process p : processes_) {
    if (findSorted<int, int>(pids, p.Pid()) == -1) dead.emplace_back(p.Pid());
  }
  for (int i : dead) processes_.erase(processes_.begin() + findProcess(i));

  std::sort(processes_.begin(), processes_.end());  // sort by cpu util
  sortP_ = sortOrder::kCpu;
  return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() {
  if (kernel_ == "") kernel_ = LinuxParser::Kernel();

  return kernel_;
}

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() {
  if (os_ == "") os_ = LinuxParser::OperatingSystem();

  return os_;
}

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }