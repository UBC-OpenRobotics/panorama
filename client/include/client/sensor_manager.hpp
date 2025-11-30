#pragma once
#include <wx/wx.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <vector>

class SensorManagerPanel : public wxPanel {
public: 
	SensorManagerPanel(wxWindow* parent);
private:
	
	void OnSensorCheckbox(wxCommandEvent& event);
	void OnAddData(wxCommandEvent& event);
	void OnSettings(wxCommandEvent& event);

	wxTextCtrl* searchBox_;
	std::vector<wxCheckBox*> sensorCheckboxes_;
	wxButton* addDataBtn_;
	wxButton* settingsBtn_;
};