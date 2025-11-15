#include "client/mainframe.hpp"
#include "client/message_model.hpp"
#include "client/CheckBoxPanel.h"

MainFrame::MainFrame(const wxString& title, std::shared_ptr<MessageModel> model,
    const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size), model_(model) {

    // Create text control for displaying messages
    messageDisplay_ = new wxTextCtrl(this, wxID_ANY, "",
        wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP);

    /*
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(messageDisplay_, 1, wxEXPAND | wxALL, 5);
    SetSizer(sizer);
    */

    // Register as observer
    model_->addObserver([this]() {
        // Use CallAfter to update GUI from non-GUI thread
        CallAfter(&MainFrame::updateMessageDisplay);
        });

    //added button
    //wxButton* button = new wxButton(messageDisplay_, wxID_ANY, "button", wxPoint(50, 100), wxSize(100, 35));

    wxArrayString sensors;
    sensors.Add("All");
    sensors.Add("Sensor 1");
    sensors.Add("Sensor 2");
    sensors.Add("Sensor 3");

    auto* checkboxPanel = new CheckBoxPanel(this, sensors);

    //left sidebar
    wxPanel* sidebar = new wxPanel(this, wxID_ANY);
    sidebar->SetBackgroundColour(*wxLIGHT_GREY);
    wxBoxSizer* sidebarSizer = new wxBoxSizer(wxVERTICAL);
    sidebarSizer->Add(checkboxPanel, 1, wxEXPAND | wxALL, 10);

    sidebarSizer->Add(new wxButton(sidebar, wxID_ANY, "Feature 1"), 0, wxALL, 5);
    sidebarSizer->Add(new wxButton(sidebar, wxID_ANY, "Feature 2"), 0, wxALL, 5);

    sidebar->SetSizer(sidebarSizer);

    // Main content area
    //wxPanel* content = new wxPanel(this, wxID_ANY);
    //content->SetBackgroundColour(*wxWHITE);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    mainSizer->Add(sidebar, 0, wxALIGN_BOTTOM | wxALL, 5);
    mainSizer->Add(checkboxPanel, 1, wxALIGN_CENTRE | wxALL, 5);
    mainSizer->Add(messageDisplay_, 3, wxEXPAND | wxALL, 5);
    SetSizer(mainSizer);

    /*
    //NEW STUFF
    wxArrayString sensors;
    sensors.Add("All");
    sensors.Add("Sensor 1");
    sensors.Add("Sensor 2");
    sensors.Add("Sensor 3");

    auto* checkboxPanel = new CheckBoxPanel(this, sensors);

    mainSizer->Add(checkboxPanel, 1, wxALIGN_TOP | wxALL, 10);
    SetSizer(mainSizer);
    */

    //BuildCheckboxes(sensors);

    //wxChoice* sensor = new wxChoice(messageDisplay_, wxID_ANY, wxPoint(150, 200), wxSize(100, -1), sensors);

    //wxCheckBox* checkBox = new wxCheckBox(messageDisplay_, wxID_ANY, "sensor 1", wxPoint(200, 50));

    //checkBox->Bind(wxEVT_CHECKBOX, &MainFrame::OnCheckBoxClicked, this);

    CreateStatusBar();
}

void MainFrame::updateMessageDisplay() {
    auto messages = model_->getMessages();
    wxString text;
    for (const auto& msg : messages) {
        text += wxString::FromUTF8(msg.c_str()) + "\n";
    }
    messageDisplay_->SetValue(text);

    // Scroll to bottom
    messageDisplay_->SetInsertionPointEnd();
}


//ME
/*
void MainFrame::BuildCheckboxes(const wxArrayString& labels) {
    // If rebuilding, clean up old controls
    for (auto* cb : checkboxes) {
        cb->Destroy();
    }
    checkboxes.clear();
    vbox->Clear(false); // keep sizer, just detach items

    // Create checkboxes dynamically
    for (const auto& label : labels) {
        auto* cb = new wxCheckBox(panel, wxID_ANY, label);
        vbox->Add(cb, 0, wxALL | wxEXPAND, 6);
        checkboxes.push_back(cb);

        // Bind each checkbox to a shared handler
        cb->Bind(wxEVT_CHECKBOX, &MainFrame::OnCheckBoxClicked, this);
    }

    // Layout after adding
    vbox->Layout();
    panel->Layout();
}

void MainFrame::OnCheckBoxClicked(wxCommandEvent& evt) {
    auto* cb = wxDynamicCast(event.GetEventObject(), wxCheckBox);
    if (!cb) return;

    // Requirement: show text when selected, clear when unselected
    if (cb->IsChecked()) {
        // Generic message. If you want the label too, use: "Selected: " + cb->GetLabel()
        SetStatusText("Sensor Selected");
    }
    else {
        SetStatusText(""); // clear
    }
}
*/