#pragma once
#include <wx/wx.h>
#include <mathplot.h>
#include <map>
#include <vector> 
#include <set>
#include <string>

class GraphPanel : public wxPanel {
public:
    GraphPanel(wxWindow* parent);

    void AddDataPoint(const std::string& sensorName, double value, double timestamp);
    void SetVisibleSensors(const std::set<std::string>& visisble);

private:
    mpWindow* m_plot;

    //store data for each sensor
    std::map<std::string, std::vector<std::pair<double, double>>> sensorData_;
    std::map<std::string, mpFXYVector*> sensorLayers_;

    std::set<std::string> visibleSensors_;

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void DrawBackground(wxDC& dc);
    void DrawGrid(wxDC& dc);
    void DrawAxes(wxDC& dc);
    void UpdateGraph();

    wxDECLARE_EVENT_TABLE();
};