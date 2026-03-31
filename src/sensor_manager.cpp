#include "sensor_manager.h"
#include "hub_config.h"
#include "system_state.h"
#include <Adafruit_INA219.h>
#include <Wire.h>

Adafruit_INA219 ina219;
static float filteredTemp = 25.0;
const float TEMP_ALFA = 0.05;

void sensorsInit() {
    if (MODULE_INA219_PRESENT) {
        if (!ina219.begin()) {
            Serial.println("Eroare: Nu am gasit INA219");
        }
    }
}

void sensorsUpdate() {
    TelemetryState& t = getTelemetryState();
    ComponentState& ntc = getNtcState();
    ComponentState& ina = getIna219State();

    // --- LOGICA NTC ---
    if (MODULE_NTC_PRESENT) {
        int raw = analogRead(NTC_PIN);
        if (raw > 0 && raw < 4095) {
            float resistance = NTC_R_FIXED * (4095.0 / (float)raw - 1.0);
            float steinhart = log(resistance / NTC_R_NOMINAL) / NTC_B_COEF;
            steinhart += 1.0 / (NTC_T_NOMINAL + 273.15);
            float currentT = (1.0 / steinhart) - 273.15;

            filteredTemp = (TEMP_ALFA * currentT) + (1.0 - TEMP_ALFA) * filteredTemp;
            t.temperatureC = filteredTemp;
            ntc.status = "online";
        }
    }

    // --- LOGICA INA219 ---
    if (MODULE_INA219_PRESENT) {
        t.voltageV = ina219.getBusVoltage_V();
        t.currentmA = ina219.getCurrent_mA();
        
        // Calcul procent baterie (Calibrat pt Li-Ion)
        float pct = ((t.voltageV - BATTERY_VOLTAGE_MIN) / (BATTERY_VOLTAGE_MAX - BATTERY_VOLTAGE_MIN)) * 100.0f;
        t.batteryPercent = (int)constrain(pct, 0, 100);
        
        ina.status = "online";
    }
}

void sensorsSetCpuLoad(int cpuLoadPercent) {
    getTelemetryState().cpuLoadPercent = cpuLoadPercent;
}