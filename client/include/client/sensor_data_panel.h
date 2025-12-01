#pragma once
#include <wx/wx.h>
#include <wx/grid.h>
#include <vector>
#include <string>

class SensorDataPanel : public wxPanel {
public:
    SensorDataPanel(wxWindow* parent, const wxArrayString& sensorNames);
    
    void UpdateReading(const std::string& sensorName, double value, const std::string& unit = "");
    void ClearReadings();
    void SetActiveSensors(const std::vector<std::string>& sensors);
    
    // Load test data from JSON file
    bool LoadTestDataFromJSON(const wxString& jsonFilePath);
    
private:
    void InitializeGrid();
    void ResetRow(int row);
    int FindSensorRow(const std::string& sensorName);
    void OnSize(wxSizeEvent& event);
    
    wxGrid* grid_;
    wxArrayString sensorNames_;
};
