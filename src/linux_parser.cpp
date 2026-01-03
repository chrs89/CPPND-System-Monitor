#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <cassert>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::cout;
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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  
  std::ifstream file(kProcDirectory + kMeminfoFilename);
  std::string line;
  std::regex memTotal_regex("^MemTotal:\\s+(\\d+)");
  std::regex memFree_regex("^MemFree:\\s+(\\d+)");
  std::smatch match;
  bool b_memTotal = false, b_memFree = false;

  float memTotal = 0.0f, memFree = 0.0f;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      if (std::regex_search(line, match, memTotal_regex)) {
        memTotal = std::stof(match[1]);
        b_memTotal = true;
      }
      if (std::regex_search(line, match, memFree_regex)) {
        memFree = std::stof(match[1]);
        b_memFree = true;
      }

      if (b_memTotal && b_memFree) {
        break;
      }
    }
  }
  return (memTotal - memFree) / memTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream file(kProcDirectory + kUptimeFilename);

  std::string line;
  long uptime = 0;

  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  std::ifstream file(kProcDirectory + kStatFilename);
  std::string line, cpu;

  long value = 0;
  long total = 0;

  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream linestream(line);

    linestream >> cpu;

    while (linestream >> value) {
      total += value;
    }
  }
  return total;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream file(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::string line;
  std::string stmp;
  vector<std::string> vec;

  std::regex tail("^(\\d+)\\s\\((.*)\\)\\s+(.*)$");
  std::smatch match;

  long uCpu = 0, kCpu = 0, cUCpu = 0, cKCpu = 0;

  if (file.is_open()) {
    std::getline(file, line);

    if (std::regex_match(line, match, tail)) {
      std::istringstream linestream(match[3]);
      while (linestream >> stmp) {
        vec.push_back(stmp);
      }
      if (vec.size() >= 15) {
        try {
          uCpu = std::stol(vec[11]);
          kCpu = std::stol(vec[12]);
          cUCpu = std::stol(vec[13]);
          cKCpu = std::stol(vec[14]);
        } catch (const std::exception& e) {
          std::cerr << e.what() << '\n';
        }
      }

      return uCpu + kCpu + cUCpu + cKCpu;
    }
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::ifstream file(kProcDirectory + kStatFilename);
  std::string line, cpu;
  long value = 0, active = 0;

  if (file.is_open() && std::getline(file, line)) {
    std::istringstream linestream(line);
    linestream >> cpu;

    vector<long> fields;
    while (linestream >> value) {
      fields.push_back(value);
    }

    if (fields.size() >= 8) {
      active = fields[0] + fields[1] + fields[2] +
      fields[5] + fields[6] + fields[7];
    }
  }

  return active;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::ifstream file(kProcDirectory + kStatFilename);
  std::string line, cpu;
  long value = 0, idle = 0;

  if (file.is_open() && std::getline(file, line)) {
    std::istringstream linestream(line);
    linestream >> cpu;

    vector<long> fields;
    while (linestream >> value) {
      fields.push_back(value);
    }

    if (fields.size() >= 8) {
      idle = fields[3] + fields[4];
      
    }
  }

  return idle;
}


// TODO: Read and return CPU utilization
vector<long> LinuxParser::CpuUtilization() {
  std::ifstream file(kProcDirectory + kStatFilename);
  vector<long> vec;
  string line, cpu;
  long value;

  if(file.is_open() && std::getline(file,line)){
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> value)
    {
      vec.push_back(value);
    } 
  }
  return vec;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  std::ifstream file(kProcDirectory + kStatFilename);
  int cntProc=0;
  string line, name, value;

  while(file.is_open() && std::getline(file,line)){
    std::istringstream linestream(line);
    linestream >> name;
    if (name == "processes"){
      if (linestream >> value)
      {
        cntProc = std::stoi(value);
        break;
      }
    }
  }
  return cntProc;
 }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream file(kProcDirectory + kStatFilename);
  int runProc=0;
  string line, name, value;

  while(file.is_open() && std::getline(file,line)){
    std::istringstream linestream(line);
    linestream >> name;
    if (name == "procs_running"){
      if (linestream >> value)
      {
        runProc = std::stoi(value);
        break;
      }
    }
  }
  return runProc;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  std::ifstream file(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line, cmd;

  while (file.is_open() && std::getline(file,line, '\0')){
    cmd += line;
  }
  
  return string(cmd); 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  std::ifstream file(kProcDirectory + to_string(pid) + kStatusFilename);
  string key , line;
  string vmSize = "0";
  
  if (file.is_open()){
    while (std::getline(file, line)){
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:"){
        linestream >> vmSize;
        break;
      }
    }
  }
  return vmSize;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
std::ifstream file(kProcDirectory + to_string(pid) + kStatusFilename);
  string key , line;
  string uid = "0";
  
  if (file.is_open()){
    while (std::getline(file, line)){
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:"){
        linestream >> uid;
        break;
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  std::ifstream file(kPasswordPath);
  string user, x, id, line;

  if (file.is_open()){
    while(std::getline(file, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      if (linestream >> user >> x >> id && id == uid){
        return user;
      }
    }
  }

  return "";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
std::ifstream file(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::string line;
  std::string stmp;
  vector<std::string> vec;

  std::regex tail("^(\\d+)\\s\\((.*)\\)\\s+(.*)$");
  std::smatch match;
  long long startTicks=0;
  long hz = sysconf(_SC_CLK_TCK);

  if (file.is_open()) {
    std::getline(file, line);

    if (std::regex_match(line, match, tail)) {
      std::istringstream linestream(match[3]);
      while (linestream >> stmp) {
        vec.push_back(stmp);
      }
      if (vec.size() >= 20) {
        startTicks = std::stoll(vec[19]);
      } else{
        return 0; // regex failed
      }
    } else{
      return 0; // file error
    }
  }

  long sysUptime = LinuxParser::UpTime(); // Unit: [s]
  long startSeconds = static_cast<long>(startTicks / hz); // Unit: [s]

  return  sysUptime-startSeconds;
}
