#pragma once 
#include <wx/wx.h>
#include <wx/datetime.h>
#include <string>

class Sensor {
    public:
        Sensor(const std::string& sensorName, const std::string& sensorUnit = "");

        //getter functions
        const std::string& GetName() const;
        const std::string& GetUnit() const;
        double GetValue() const;
        const wxDateTime& GetLastUpdated() const;
        bool checkEnabled() const;

        //setter functions
        void SetValue(double value);
        void SetUnit(const std::string& unit);
        void SetEnabled(bool enabled); 


    private:
    std::string sensorName_;
    std::string sensorUnit_;
    double sensorValue_;
    wxDateTime lastUpdated_;
    bool enabled_;
};

