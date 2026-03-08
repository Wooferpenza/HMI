#ifndef MODBUSSETTINGS_H
#define MODBUSSETTINGS_H

#include <QString>

class QSettings;

struct ModbusSettings {
    QString ip;
    int port = 502;
    int unitId = 1;
    int pollIntervalMs = 1000;

    static ModbusSettings load(QSettings &settings);
    void save(QSettings &settings) const;
};

#endif // MODBUSSETTINGS_H
