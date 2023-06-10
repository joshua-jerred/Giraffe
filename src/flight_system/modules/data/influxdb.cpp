#include "influxdb.hpp"

using namespace data_middleware;

void InfluxDb::logDataPacket(const data::DataPacket &packet) { (void)packet; }

void InfluxDb::logLogPacket(const data::LogPacket &packet) { (void)packet; }