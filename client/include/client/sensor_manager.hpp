#pragma once
#include <wx/wx.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <vector>
#include <map>
#include <memory>
#include "client/sensor.hpp"
#include <functional>

class SensorManagerPanel : public wxPanel {
public: 
	SensorManagerPanel(wxWindow* parent);

	// Dynamically add a sensor and create its checkbox in the snesor manager
	void AddSensor(std::shared_ptr<Sensor> sensor);

	void SetOnSensorToggled(std::function<void()> callback);
	// Returns names of all currently enabled sensors
	std::vector<std::string> GetEnabledSensorNames() const;

private:
	
	void OnSensorCheckbox(wxCommandEvent& event);
	void OnAddData(wxCommandEvent& event);
	void OnSettings(wxCommandEvent& event);

	wxBoxSizer* sizer_;
	wxTextCtrl* searchBox_;
	wxButton* addDataBtn_;
	wxButton* settingsBtn_;

	std::function<void()> onSensorToggled_;

	// Maps each checkbox to its corresponding Sensor object
	std::map<wxCheckBox*, std::shared_ptr<Sensor>> checkboxToSensor_;
};