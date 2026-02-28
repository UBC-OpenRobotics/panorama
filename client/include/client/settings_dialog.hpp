#pragma once

#include <wx/wx.h>
#include <wx/spinctrl.h>

class SettingsDialog : public wxDialog {
public:
    SettingsDialog(wxWindow* parent);

private:
    void OnSave(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnChangeDirectory(wxCommandEvent& event);
    void loadSettings();
    void saveSettings();

    // UI controls
    wxTextCtrl* runtimeDirText_;
    wxButton* browseDirBtn_;
    wxTextCtrl* tcpHostText_;
    wxSpinCtrl* tcpPortSpin_;
    wxCheckBox* autoReconnectCheck_;
    wxSpinCtrl* reconnectDelaySpin_;
};
