#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, v;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> v >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float utilization = 0.0f;
  float total, free, val;
  string key;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "MemTotal:")
        total = val;
      else if (key == "MemFree:") {
        free = val;
        break;
      }
    }
    utilization = (total - free) / total;
  }
  return utilization;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  double uptime, idle;
  string raw;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, raw);
    std::istringstream linestream(raw);
    linestream >> uptime >> idle;
  }

  return (long)uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long tmp, jiffies = 0;
  string line, cpu;

  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    while (cpu != "cpu" && std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> cpu;
      if (cpu == "cpu") {
        int i = 0;
        while (++i <=
               8) {  // items 9 and 10 are already included in other numbers
          linestream >> tmp;
          jiffies += tmp;
        }
      }
    }
  }

  return jiffies;
}

std::unordered_map<LinuxParser::CPUStates, long> LinuxParser::GetJiffieMap() {
  std::unordered_map<CPUStates, long> counts;
  string cpu, line;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    linestream >> cpu >> counts[kUser_] >> counts[kNice_] >> counts[kSystem_] >>
        counts[kIdle_] >> counts[kIOwait_] >> counts[kIRQ_] >>
        counts[kSoftIRQ_] >> counts[kSteal_] >> counts[kGuest_] >>
        counts[kGuestNice_];
  }
  return counts;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::unordered_map<CPUStates, long> counts = GetJiffieMap();
  return counts[kUser_] + counts[kNice_] + counts[kSystem_] + counts[kIRQ_] +
         counts[kSoftIRQ_] + counts[kSteal_];
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::unordered_map<LinuxParser::CPUStates, long> counts = GetJiffieMap();
  return counts[kIdle_] + counts[kIOwait_];
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key, line;
  int processes;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (key != "processes" && std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> processes;
    }
  }
  return processes;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, line;
  int runningProcesses;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (key != "procs_running" && std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> runningProcesses;
    }
  }
  return runningProcesses;
}

// Read and return the command used to start the process
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  string line;
  if (stream.is_open()) std::getline(stream, line);
  return line;
}

// Read and return the memory used by the process
string LinuxParser::Ram(int pid) {
  string key, val, line;
  string memory{"0"};

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (key != "VmSize:" && std::getline(stream, line)) {
      // std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> val;
    }

    float MiB = std::stof(val) / 1024;

    memory = std::to_string(MiB);
  }
  return memory;
}

// Read and return the UID under which the program is running
string LinuxParser::Uid(int pid) {
  string key, val, line;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (key != "Uid:" && std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> val;
    }
  }
  return val;
}

// get username for pid
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string user, id, x, line;

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (id != uid && std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> id;
    }
  }
  return user;
}

// Read the numbers used to calc cpu utl <process> and return in unordered_map
std::unordered_map<string, long> LinuxParser::CpuUtilization(int pid) {
  std::unordered_map<string, long> fields{};

  fields["sysUp"] = UpTime();
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line;

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    string val;

    int n{0};
    while (++n) {
      if (linestream >> val) {
        switch (n) {
          case 14:
            fields["utime"] = std::stol(val);
            break;
          case 15:
            fields["stime"] = std::stol(val);
            break;
          case 16:
            fields["cutime"] = std::stol(val);
            break;
          case 17:
            fields["cstime"] = std::stol(val);
            break;
          case 22:
            fields["starttime"] = std::stol(val);
        }
      } else
        break;
    }
  }

  return fields;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  auto utlMap = CpuUtilization(pid);
  long hz = sysconf(_SC_CLK_TCK);
  return utlMap["sysUp"] - (long)(utlMap["starttime"] / hz);
}
