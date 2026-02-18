#include <wx/wx.h>
#include <client/sensor_manager.hpp>
#include "client/settings_dialog.hpp"

SensorManagerPanel::SensorManagerPanel(wxWindow* parent)
	: wxPanel(parent, wxID_ANY)
{
	SetBackgroundColour(wxColour(150, 150, 150));

	sizer_ = new wxBoxSizer(wxVERTICAL);

	searchBox_ = new wxTextCtrl(this, wxID_ANY, "Search...", wxDefaultPosition, wxDefaultSize);
	sizer_->Add(searchBox_, 0, wxEXPAND | wxALL, 5);

	// Sensors will be added dynamically via AddSensor()

	sizer_->AddStretchSpacer();

	addDataBtn_ = new wxButton(this, wxID_ANY, "Add Data");
	sizer_->Add(addDataBtn_, 0, wxEXPAND | wxALL, 5);
	addDataBtn_->Bind(wxEVT_BUTTON, &SensorManagerPanel::OnAddData, this);

	settingsBtn_ = new wxButton(this, wxID_ANY, "Settings");
	sizer_->Add(settingsBtn_, 0, wxEXPAND | wxALL, 5);
	settingsBtn_->Bind(wxEVT_BUTTON, &SensorManagerPanel::OnSettings, this);

	SetSizer(sizer_);
}

void SensorManagerPanel::AddSensor(std::shared_ptr<Sensor> sensor) {

	wxCheckBox* checkbox = new wxCheckBox(this, wxID_ANY, sensor->GetName());
	checkbox->SetValue(sensor->checkEnabled());

	sizer_->Insert(sizer_->GetItemCount() - 3, checkbox, 0, wxALL, 5);

	checkboxToSensor_[checkbox] = sensor;

	checkbox->Bind(wxEVT_CHECKBOX, &SensorManagerPanel::OnSensorCheckbox, this);

	Layout();
}

void SensorManagerPanel::OnSensorCheckbox(wxCommandEvent& event) {
	wxCheckBox* checkbox = static_cast<wxCheckBox*>(event.GetEventObject());

	auto it = checkboxToSensor_.find(checkbox);
	
	if (it != checkboxToSensor_.end()) {
		it->second->SetEnabled(checkbox->IsChecked());

		wxString msg = wxString::Format("%s is now %s",
			it->second->GetName(),
			checkbox->IsChecked() ? "enabled" : "disabled");
		wxLogMessage(msg);
	}
}

void SensorManagerPanel::OnAddData(wxCommandEvent& event) {
	wxLogMessage("Add Data button clicked!");
}

void SensorManagerPanel::OnSettings(wxCommandEvent& event) {
	SettingsDialog dialog(this);
	dialog.ShowModal();
}