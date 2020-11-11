//
// Created by Andrea Petreti on 06/11/2020.
//

#ifndef SWARM_GEN_FOOTBOTUTILS_H
#define SWARM_GEN_FOOTBOTUTILS_H

#include <vector>
#include <fplus/fplus.hpp>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_motor_ground_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h>

using namespace std;
using namespace fplus;
using namespace argos;

namespace footbotutils {

    /*
     * functional variant is less efficient than actual version
     */
    vector<double> readProximityValues(const CCI_FootBotProximitySensor& proximitySensor, int groupSensor) {
        auto readings = proximitySensor.GetReadings();
        vector<double> groupedValued;
        groupedValued.reserve(groupSensor);
        int groupRange = readings.size() / groupSensor;
        for(int i = 0; i < groupSensor; i++) {
            // sum policy
            double sum = 0;
            for(int j = 0; j < groupRange; j++) {
                sum += readings[(i * groupRange) + j].Value;
            }
            groupedValued.push_back(sum);
        }
        return groupedValued;
    }

    /*vector<double> readProximityValues(const CCI_FootBotProximitySensor& proximitySensor, int groupSensor) {
        auto readings = proximitySensor.GetReadings();
        vector<double> groupedValued;
        groupedValued.reserve(groupSensor);
        int groupRange = readings.size() / groupSensor;
        for(int i = 0; i < groupSensor; i++) {
            // group by max policy
            double max = -1;
            for(int j = 0; j < groupRange; j++) {
                double read = readings[(i * groupRange) + j].Value;
                if(read > max) { max = read; }
            }
            groupedValued.push_back(max);
        }

        return groupedValued;
    }*/

    vector<double> readMotorGroundValues(const CCI_FootBotMotorGroundSensor& groundSensor) {
        return fwd::apply(groundSensor.GetReadings(),
                          fwd::transform([](CCI_FootBotMotorGroundSensor::SReading reading) { return reading.Value; }));
    }

    vector<bool> sensorValuesToBooleans(const std::vector<double>& values, double threshold, bool invert = false) {
        return transform([invert, threshold](double value) {
            return (invert) ? value <= threshold : value > threshold;
        }, values);
    }

}

#endif //SWARM_GEN_FOOTBOTUTILS_H
