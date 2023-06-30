#include "system_module.hpp"

#include <sys/statfs.h>  // for statfs   (disk info)
#include <sys/sysinfo.h> // for sysinfo  (CPU, RAM)

#include <ctime>   // for time_t   (time)
#include <fstream> // for ifstream (CPU temp)
#include <sstream> // for stringstream (time)

static modules::MetaData metadata("system_module",
                                  node::Identification::SYSTEM_MODULE);

modules::SystemModule::SystemModule(data::SharedData &shared_data,
                                    cfg::Configuration &config)
    : modules::Module(metadata, shared_data, config) {
}

modules::SystemModule::~SystemModule() {
}

void modules::SystemModule::startup() {
}

void modules::SystemModule::loop() {
  updateCpuAndMemoryInfo();
  updateCpuTemp();
  updateDiskInfo();
  updateNetworkInfo();

  shared_data_.blocks.system_info.set(data_);
}

void modules::SystemModule::shutdown() {
}

void modules::SystemModule::processCommand(const cmd::Command &command) {
  (void)command;
}

void modules::SystemModule::updateCpuAndMemoryInfo() {
  struct sysinfo sys_info; // from sys/sysinfo.h

  if (sysinfo(&sys_info) != 0) {
    error(data::LogId::SYSTEM_MODULE_systemInfoReadFail);
    return;
  }

  data_.uptime_hours = sys_info.uptime / 3600.0;
  data_.cpu_load_avg_1 = sys_info.loads[0] / 65536.0;
  data_.cpu_load_avg_5 = sys_info.loads[1] / 65536.0;
  data_.cpu_load_avg_15 = sys_info.loads[2] / 65536.0;

  data_.mem_total_gb = sys_info.totalram / 1024.0 / 1024.0 / 1024.0;
  data_.mem_used_percent =
      (sys_info.totalram - sys_info.freeram) / (float)sys_info.totalram * 100.0;
  data_.mem_free_gb = sys_info.freeram / 1024.0 / 1024.0 / 1024.0;

  data_.swap_total_gb = sys_info.totalswap / 1024.0 / 1024.0 / 1024.0;
  data_.swap_free_gb = sys_info.freeswap / 1024.0 / 1024.0 / 1024.0;
}

void modules::SystemModule::updateCpuTemp() {
  std::ifstream temp_file("/sys/class/thermal/thermal_zone0/temp");

  try {
    if (temp_file.is_open()) {
      std::string str;
      std::getline(temp_file, str);
      temp_file.close();
      data_.cpu_temp_c = std::stof(str) / 1000.0;
    }
  } catch (std::runtime_error &e) {
    error(data::LogId::SYSTEM_MODULE_cpuTempReadFail, e.what());
  }
}

void modules::SystemModule::updateDiskInfo() {
  struct statfs disk_stat; // from sys/statfs.h

  if (statfs("/", &disk_stat) != 0) {
    error(data::LogId::SYSTEM_MODULE_diskInfoReadFail);
    return;
  }

  data_.disk_total_gb = (unsigned long long)(disk_stat.f_blocks) *
                        (unsigned long long)(disk_stat.f_bsize) / 1024.0 /
                        1024.0 / 1024.0;

  data_.disk_free_gb = (unsigned long long)(disk_stat.f_bavail) *
                       (unsigned long long)(disk_stat.f_bsize) / 1024.0 /
                       1024.0 / 1024.0;

  data_.disk_used_percent =
      (data_.disk_total_gb - data_.disk_free_gb) / data_.disk_total_gb * 100.0;
}

void modules::SystemModule::updateNetworkInfo() {
}