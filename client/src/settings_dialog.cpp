#include "client/settings_dialog.hpp"
#include "client/config_manager.hpp"
#include <wx/dirdlg.h>
#include <wx/stattext.h>
#include <wx/sizer.h>

SettingsDialog::SettingsDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Settings", wxDefaultPosition, wxSize(500, 300)) {

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    //====================================
    //  Runtime Dir Section
    //====================================
    wxStaticBoxSizer* dirBox = new wxStaticBoxSizer(wxVERTICAL, this, "Runtime Directory");

    wxBoxSizer* dirSizer = new wxBoxSizer(wxHORIZONTAL);
    runtimeDirText_ = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    dirSizer->Add(runtimeDirText_, 1, wxEXPAND | wxRIGHT, 5);

    browseDirBtn_ = new wxButton(this, wxID_ANY, "Browse...");
    browseDirBtn_->Bind(wxEVT_BUTTON, &SettingsDialog::OnChangeDirectory, this);
    dirSizer->Add(browseDirBtn_, 0);

    dirBox->Add(dirSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(dirBox, 0, wxEXPAND | wxALL, 10);
    
    //====================================
    //  TCP Settings Section
    //====================================
    wxStaticBoxSizer* tcpBox = new wxStaticBoxSizer(wxVERTICAL, this, "TCP Connection Settings");

    wxFlexGridSizer* tcpGrid = new wxFlexGridSizer(2, 2, 10, 10);
    tcpGrid->AddGrowableCol(1);

    tcpGrid->Add(new wxStaticText(this, wxID_ANY, "Host:"), 0, wxALIGN_CENTER_VERTICAL);
    tcpHostText_ = new wxTextCtrl(this, wxID_ANY);
    tcpGrid->Add(tcpHostText_, 1, wxEXPAND);

    tcpGrid->Add(new wxStaticText(this, wxID_ANY, "Port:"), 0, wxALIGN_CENTER_VERTICAL);
    tcpPortSpin_ = new wxSpinCtrl(this, wxID_ANY);
    tcpPortSpin_->SetRange(1, 65535);
    tcpGrid->Add(tcpPortSpin_, 1, wxEXPAND);

    tcpBox->Add(tcpGrid, 0, wxEXPAND | wxALL, 5);

    autoReconnectCheck_ = new wxCheckBox(this, wxID_ANY, "Auto-reconnect on disconnect");
    tcpBox->Add(autoReconnectCheck_, 0, wxALL, 5);

    wxBoxSizer* delaySizer = new wxBoxSizer(wxHORIZONTAL);
    delaySizer->Add(new wxStaticText(this, wxID_ANY, "Reconnect delay (seconds):"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    reconnectDelaySpin_ = new wxSpinCtrl(this, wxID_ANY);
    reconnectDelaySpin_->SetRange(1, 60);
    delaySizer->Add(reconnectDelaySpin_, 0);
    tcpBox->Add(delaySizer, 0, wxALL, 5);

    mainSizer->Add(tcpBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    //====================================
    //  Buttons
    //====================================

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* saveBtn = new wxButton(this, wxID_OK, "Save");
    saveBtn->Bind(wxEVT_BUTTON, &SettingsDialog::OnSave, this);
    buttonSizer->Add(saveBtn, 0, wxRIGHT, 5);

    wxButton* cancelBtn = new wxButton(this, wxID_CANCEL, "Cancel");
    cancelBtn->Bind(wxEVT_BUTTON, &SettingsDialog::OnCancel, this);
    buttonSizer->Add(cancelBtn, 0);

    mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxALL, 10);

    SetSizer(mainSizer);

    loadSettings();
}

void SettingsDialog::loadSettings() {
    ConfigManager& config = ConfigManager::getInstance();

    runtimeDirText_->SetValue(config.getRuntimeDirectory());

    std::string host;
    int port;
    bool autoReconnect;
    int reconnectDelay;

    if (config.getTcpSettings(host, port, autoReconnect, reconnectDelay)) {
        tcpHostText_->SetValue(host);
        tcpPortSpin_->SetValue(port);
        autoReconnectCheck_->SetValue(autoReconnect);
        reconnectDelaySpin_->SetValue(reconnectDelay);
    } else {
        tcpHostText_->SetValue("127.0.0.1"); // default
        tcpPortSpin_->SetValue(3000);
        autoReconnectCheck_->SetValue(true);
        reconnectDelaySpin_->SetValue(5);
    }
}

void SettingsDialog::saveSettings() {
    ConfigManager& config = ConfigManager::getInstance();

    // Save TCP Settings
    std::string host = tcpHostText_->GetValue().ToStdString();
    int port = tcpPortSpin_->GetValue();
    bool autoReconnect = autoReconnectCheck_->GetValue();
    int reconnectDelay = reconnectDelaySpin_->GetValue();

    if (!config.saveTcpSettings(host, port, autoReconnect, reconnectDelay)) {
        wxMessageBox("Failed to save TCP settings.", "Error", wxOK | wxICON_ERROR);
    }

    // Note: Runtime directory changes are not persisted here
    // They require application restart to take effect
}

void SettingsDialog::OnSave(wxCommandEvent& event) {
    saveSettings();
    wxMessageBox("Settings saved successfully.\n\nNote: Some settings may require an application restart to take effect.",
                 "Settings Saved", wxOK | wxICON_INFORMATION);
    EndModal(wxID_OK);
}

void SettingsDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void SettingsDialog::OnChangeDirectory(wxCommandEvent& event) {
    ConfigManager& config = ConfigManager::getInstance();

    wxDirDialog dirDialog(this, "Select New Runtime Directory",
                         config.getRuntimeDirectory(),
                         wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    if (dirDialog.ShowModal() == wxID_OK) {
        std::string newDir = dirDialog.GetPath().ToStdString();
        runtimeDirText_->SetValue(newDir);

        if (config.setRuntimeDirectory(newDir)) {
            config.saveConfig("runtime_directory", newDir);
            wxMessageBox("Runtime directory updated. Please restart the application for changes to take full effect.",
                        "Directory Changed", wxOK | wxICON_INFORMATION);
        } else {
            wxMessageBox("Failed to set runtime directory. Please ensure the directory is writable.",
                        "Error", wxOK | wxICON_ERROR);
        }
    }
}
