/**
 * @file sysinfo.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief An extension to get system information (CPU, RAM, Disk, etc.)
 * 
 * @date 2023-01-01
 * @copyright Copyright (c) 2023
 * @version 0.1
 */

#include <sys/sysinfo.h> // for sysinfo  (CPU, RAM)
#include <sys/statfs.h>  // for statfs   (disk info)
#include <fstream>       // for ifstream (CPU temp)

#include "extensions.h"

namespace ex_internal {
    struct cpu_mem_info {
        // CPU
        float uptime_hours = 0.0;
        float cpu_load_avg_1 = 0.0;
        float cpu_load_avg_5 = 0.0;
        float cpu_load_avg_15 = 0.0;
        // RAM
        float ram_total_gb = 0.0;
        float ram_used_percent = 0.0;
        float ram_free_gb = 0.0;
    };

    struct disk_info {
        float total_gb = 0.0;
        float free_space_gb = 0.0;
        float used_percent = 0.0;
    };

    bool getCpuAndMemInfo(cpu_mem_info &info) {
        struct sysinfo sysinf; // from sys/sysinfo.h
        
        if (sysinfo(&sysinf) != 0) {
            return false;
        }

        info.uptime_hours = sysinf.uptime / 3600.0;
        info.cpu_load_avg_1 = sysinf.loads[0] / 65536.0;
        info.cpu_load_avg_5 = sysinf.loads[1] / 65536.0;
        info.cpu_load_avg_15 = sysinf.loads[2] / 65536.0;

        info.ram_total_gb = sysinf.totalram / 1024.0 / 1024.0 / 1024.0;
        info.ram_used_percent = 
            (sysinf.totalram - sysinf.freeram) / (float)sysinf.totalram * 100.0;
        info.ram_free_gb = sysinf.freeram / 1024.0 / 1024.0 / 1024.0;

        return true;
    }

    int getCpuTemp() {
        std::ifstream temp_file("/sys/class/thermal/thermal_zone0/temp");

        if (temp_file.is_open()) {
            std::string str;
            std::getline(temp_file, str);
            temp_file.close();
            return std::stoi(str) / 1000;
        }

        return -1;
    }

    bool getDiskInfo(disk_info &info) {
        struct statfs disk_stat; // from sys/statfs.h

        if (statfs("/", &disk_stat) != 0) {
            return false;
        }

        info.total_gb = 
            (unsigned long long) (disk_stat.f_blocks) 
            * (unsigned long long) (disk_stat.f_bsize) 
            / 1024.0 / 1024.0 / 1024.0;

        info.free_space_gb = 
            (unsigned long long) (disk_stat.f_bavail) 
            * (unsigned long long) (disk_stat.f_bsize) 
            / 1024.0 / 1024.0 / 1024.0;

        info.used_percent =
            (info.total_gb - info.free_space_gb) / info.total_gb * 100.0;
        return true;
    }
}

extension::SYSINFO::SYSINFO(
        DataStream *p_data_stream, 
        ExtensionMetadata extension_metadata) : 
        Extension(p_data_stream, extension_metadata) {
}

extension::SYSINFO::~SYSINFO() {

}

int extension::SYSINFO::runner() {
    ex_internal::cpu_mem_info system_info;
    ex_internal::disk_info disk_info;
    float cpu_temp = 0.0;

    setStatus(ExtensionStatus::RUNNING);
    while (!stop_flag_) {

        if (ex_internal::getCpuAndMemInfo(system_info)) {
            sendData("uptime_hours", system_info.uptime_hours);
            sendData("load_avg_1", system_info.cpu_load_avg_1);
            sendData("load_avg_5", system_info.cpu_load_avg_5);
            sendData("load_avg_15", system_info.cpu_load_avg_15);
            sendData("ram_total_gb", system_info.ram_total_gb);
            sendData("ram_used_percent", system_info.ram_used_percent);
            sendData("ram_free_gb", system_info.ram_free_gb);
        } else {
            error("CM");
        }

        cpu_temp = ex_internal::getCpuTemp();
        if (cpu_temp < 0) {
            error("CT");
        } else {
            sendData("cpu_temp", cpu_temp);
        }

        if (ex_internal::getDiskInfo(disk_info)) {
            sendData("disk_total_gb", disk_info.total_gb);
            sendData("disk_available_gb", disk_info.free_space_gb);
            sendData("disk_used_percent", disk_info.used_percent);
        } else {
            error("DI");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(getUpdateInterval()));
    }
    setStatus(ExtensionStatus::STOPPED);
    return 0;
}
