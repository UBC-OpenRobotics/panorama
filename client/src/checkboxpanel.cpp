#include "client/CheckBoxPanel.h"

CheckBoxPanel::CheckBoxPanel(wxWindow* parent, const wxArrayString& labels)
    : wxPanel(parent, wxID_ANY)
{
    vbox = new wxBoxSizer(wxVERTICAL);
    SetSizer(vbox);

    for (const auto& label : labels) {
        auto* cb = new wxCheckBox(this, wxID_ANY, label);
        vbox->Add(cb, 0, wxALL | wxEXPAND, 5);
        checkboxes.push_back(cb);

        // Bind each checkbox to this handler
        cb->Bind(wxEVT_CHECKBOX, &CheckBoxPanel::OnCheckBox, this);
    }
}

void CheckBoxPanel::OnCheckBox(wxCommandEvent& event) {
    auto* cb = wxDynamicCast(event.GetEventObject(), wxCheckBox);
    if (!cb) return;

    if (cb->IsChecked())
        wxLogStatus("Sensor selected.");
    else
        wxLogStatus("");
}