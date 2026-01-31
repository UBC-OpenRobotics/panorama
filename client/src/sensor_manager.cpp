#include <wx/wx.h>
#include <client/sensor_manager.hpp>
#include "client/settings_dialog.hpp"

SensorManagerPanel::SensorManagerPanel(wxWindow* parent)
	: wxPanel(parent, wxID_ANY)
{
	SetBackgroundColour(wxColour(150, 150, 150));

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	searchBox_ = new wxTextCtrl(this, wxID_ANY, "Search...", wxDefaultPosition, wxDefaultSize);

	sizer->Add(searchBox_, 0, wxEXPAND | wxALL, 5);

	//create checkboxes
	for (int i = 0; i < 5; i++) {
		wxString label = wxString::Format("sensor %d", i + 1);
		wxCheckBox* checkbox = new wxCheckBox(this, wxID_ANY, label);

		sensorCheckboxes_.push_back(checkbox);

		sizer->Add(checkbox, 0, wxALL, 5);

		checkbox->Bind(wxEVT_CHECKBOX, &SensorManagerPanel::OnSensorCheckbox, this);
	}

	sizer->AddStretchSpacer();

	addDataBtn_ = new wxButton(this, wxID_ANY, "Add Data");
	sizer->Add(addDataBtn_, 0, wxEXPAND | wxALL, 5);
	addDataBtn_->Bind(wxEVT_BUTTON, &SensorManagerPanel::OnAddData, this);

	settingsBtn_ = new wxButton(this, wxID_ANY, "Settings");
	sizer->Add(settingsBtn_, 0, wxEXPAND | wxALL, 5);
	settingsBtn_->Bind(wxEVT_BUTTON, &SensorManagerPanel::OnSettings, this);

	SetSizer(sizer);

}

void SensorManagerPanel::OnSensorCheckbox(wxCommandEvent& event) {
	// Find which checkbox was clicked
	for (size_t i = 0; i < sensorCheckboxes_.size(); i++) {
		if (event.GetEventObject() == sensorCheckboxes_[i]) {
			wxString msg = wxString::Format("Sensor %d is now %s",
				i + 1,
				sensorCheckboxes_[i]->IsChecked() ? "checked" : "unchecked");
			wxLogMessage(msg);  // Prints to console/debug output
			break;
		}
	}
}

void SensorManagerPanel::OnAddData(wxCommandEvent& event) {
	wxLogMessage("Add Data button clicked!");
}

void SensorManagerPanel::OnSettings(wxCommandEvent& event) {
	SettingsDialog dialog(this);
	dialog.ShowModal();
}