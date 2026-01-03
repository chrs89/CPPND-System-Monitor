#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    std::vector<long> cpuUtil = LinuxParser::CpuUtilization();
    
    long idle = cpuUtil[LinuxParser::kIdle_] + cpuUtil[LinuxParser::kIOwait_];

    long running = cpuUtil[LinuxParser::kUser_] +
      cpuUtil[LinuxParser::kNice_] +
      cpuUtil[LinuxParser::kSystem_] +
      cpuUtil[LinuxParser::kIRQ_] +
      cpuUtil[LinuxParser::kSoftIRQ_] +
      cpuUtil[LinuxParser::kSteal_];

    long total = idle + running;

    long dTotal = total - prevTotal_;
    long dIdle = idle - prevIdle_;

    prevIdle_ = idle;
    prevTotal_ = total;

    if (dTotal == 0) {
    return 0.0;
}

    return static_cast<float>(dTotal-dIdle)/dTotal;

}