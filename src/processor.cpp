#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  long total = LinuxParser::Jiffies();
  long active = LinuxParser::ActiveJiffies();
  float currentActive = (float)(active - prevActive_);
  float currentTotal = (float)(total - prevJiffies_);
  float utl = currentActive / currentTotal;
  prevJiffies_ = total;
  prevActive_ = active;
  return utl;
}