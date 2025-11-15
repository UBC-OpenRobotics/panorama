#pragma once
#include <wx/wx.h>
#include <vector>

class CheckBoxPanel : public wxPanel {
public:
    CheckBoxPanel(wxWindow* parent, const wxArrayString& labels);

private:
    void OnCheckBox(wxCommandEvent& event);

    wxBoxSizer* vbox = nullptr;
    std::vector<wxCheckBox*> checkboxes;
};