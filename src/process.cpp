#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_{pid} {}
// TODO: Return this process's ID
int Process::Pid() const{
    return pid_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const{
    long totalTimeTicks = LinuxParser::ActiveJiffies(pid_);
    long hz = sysconf(_SC_CLK_TCK);

    long sysUp = LinuxParser::UpTime();

    float totalTimesec = totalTimeTicks / static_cast<float>(hz);
    return totalTimesec / static_cast<float>(sysUp);
}

// TODO: Return the command that generated this process
string Process::Command() const{
    return LinuxParser::Command(pid_);
}

// TODO: Return this process's memory utilization
string Process::Ram() const{ 
    return LinuxParser::Ram(pid_);
 }

// TODO: Return the user (name) that generated this process
string Process::User() const{
    return LinuxParser::User(pid_);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const{
    return LinuxParser::UpTime(pid_);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const {
    return CpuUtilization() > a.CpuUtilization();
}