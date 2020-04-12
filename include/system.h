#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();
  std::vector<Process>& Processes();
  float MemoryUtilization();
  long UpTime();
  int TotalProcesses();
  int RunningProcesses();
  std::string Kernel();
  std::string OperatingSystem();

 private:
  enum sortOrder { kPid, kCpu, kNone };
  sortOrder sortP_{kNone};
  int findProcess(int pid);
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  std::string kernel_{""};
  std::string os_{""};
};

#endif