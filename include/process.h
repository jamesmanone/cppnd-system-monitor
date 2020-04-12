#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid) : pid_(pid) {}
  int Pid() const;           // TODO: See src/process.cpp
  std::string User() const;  // TODO: See src/process.cpp
  void User(std::string user);
  std::string Command() const;  // TODO: See src/process.cpp
  void Command(std::string command);
  float CpuUtilization() const;  // TODO: See src/process.cpp
  void CalcCpu();
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp
  bool operator==(int const& a) const;
  bool operator<(int const& a) const;

  // TODO: Declare any necessary private members
 private:
  int pid_{0};
  std::string user_;
  std::string command_;
  long time_{0};
  long uptime_{0};
  float cpuUtil_{0.0};
};

#endif