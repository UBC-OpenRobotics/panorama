#pragma once
#include <wx/wx.h>
#include <mathplot.h>
#include <map>
#include <vector> 
#include <set>
#include <string>
#include <memory>
#include "client/post_processing.hpp"

class GraphPanel : public wxPanel {
public:
    GraphPanel(wxWindow* parent, std::shared_ptr<PostProcessing> postProcessor);

    void AddDataPoint(const std::string& sensorName, double value, double timestamp);
    void SetVisibleSensors(const std::set<std::string>& visisble);

private:
    mpWindow* m_plot;

    //store data for each sensor
    std::map<std::string, std::vector<std::pair<double, double>>> sensorData_;
    std::map<std::string, mpFXYVector*> sensorLayers_;

    std::set<std::string> visibleSensors_;

    void UpdateGraph();

    std::shared_ptr<PostProcessing> postProcessor_;
};