#pragma once
#include <wx/wx.h>
#include <wx/grid.h>
#include <vector>
#include <string>

class SensorDataFrame : public wxPanel {
public:
    SensorDataFrame(wxWindow* parent, const wxArrayString& sensorNames);
    
    void AddSensorRow(const std::string& sensorName);
    void UpdateReading(const std::string& sensorName, double value, const std::string& unit = "");
    void ClearReadings();
    void SetActiveSensors(const std::vector<std::string>& sensors);
    
private:
    void InitializeGrid();
    int FindSensorRow(const std::string& sensorName);
    void OnSize(wxSizeEvent& event);
    
    wxGrid* grid_;
    wxArrayString sensorNames_;
};
