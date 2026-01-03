#include "ncurses_display.h"
#include "system.h"

/*START-DEBUG-Includes*/
#include "linux_parser.h"
#include <iostream>
#include <string>
#include <iomanip>

using std::cout;

/*END-DEBUG-Includes*/

int main() {
  /*START-DEBUG-Section*/
  float memUtil = LinuxParser::MemoryUtilization();
  cout << "Utilized Memory: " 
  << std::fixed << std::setprecision(2)
  << memUtil*100 << " %\n";
  
  std::string os = LinuxParser::OperatingSystem();
  cout << "Operating System: " << os << "\n";
  
  long uptime = LinuxParser::UpTime();
  cout << std::fixed <<"Uptime: " << uptime << "\n";

  long totaCpuTime = LinuxParser::Jiffies();
  cout << std::fixed <<"totaCpuTime: " << totaCpuTime << "\n";
  

  auto pids = LinuxParser::Pids();
  int indx = 0;
  for (auto pid : pids){
    cout<<"pid[" <<indx <<"]: " << pid << "\n";
    indx++;
  }
  /*END-DEBUG-Section*/
  
  System system;
  NCursesDisplay::Display(system);
}