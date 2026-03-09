#include "modbussettings.h"
#include <QSettings>

namespace {

constexpr const char kModbusIp[] = "modbus/ip";
constexpr const char kModbusPort[] = "modbus/port";
constexpr const char kModbusUnitId[] = "modbus/unitId";
constexpr const char kModbusPollIntervalMs[] = "modbus/pollIntervalMs";
constexpr const char kDefaultIp[] = "192.168.1.5";
constexpr int kDefaultPort = 502;
constexpr int kDefaultUnitId = 1;
constexpr int kDefaultPollIntervalMs = 1000;

} // namespace

ModbusSettings ModbusSettings::load(QSettings &s)
{
    ModbusSettings out;
    out.ip = s.value(kModbusIp, kDefaultIp).toString();
    out.port = s.value(kModbusPort, kDefaultPort).toInt();
    out.unitId = s.value(kModbusUnitId, kDefaultUnitId).toInt();
    out.pollIntervalMs = s.value(kModbusPollIntervalMs, kDefaultPollIntervalMs).toInt();
    return out;
}

void ModbusSettings::save(QSettings &s) const
{
    s.setValue(kModbusIp, ip);
    s.setValue(kModbusPort, port);
    s.setValue(kModbusUnitId, unitId);
    s.setValue(kModbusPollIntervalMs, pollIntervalMs);
}
