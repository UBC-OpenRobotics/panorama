#include "client/mainframe.hpp"
#include "client/message_model.hpp"
// #include "client/graph_panel.hpp"
#include "client/sensor_data_panel.h"
#include "client/sensor_manager.hpp"
#include <wx/dcbuffer.h>
#include <wx/sizer.h>

MainFrame::MainFrame(const wxString& title, std::shared_ptr<MessageModel> model,
    const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size), model_(model) {
    
    // Create splitter for layout
    wxSplitterWindow* mainSplitter = new wxSplitterWindow(this, wxID_ANY);
    wxSplitterWindow* topSplitter = new wxSplitterWindow(mainSplitter, wxID_ANY);
    wxSplitterWindow* rightSplitter = new wxSplitterWindow(topSplitter, wxID_ANY);
    // Create the four main panels
    // Sensor manager panel
    SensorManagerPanel* sensorPanel = new SensorManagerPanel(topSplitter);
    

    // Data view area - Sensor Data Panel
    wxArrayString sensorNames;
    sensorNames.Add("Sensor 1");
    sensorNames.Add("Sensor 2");
    sensorNames.Add("Sensor 3");
    sensorNames.Add("Sensor 4");
    sensorNames.Add("Sensor 5");
    sensorNames.Add("Another one");
    wxPanel* dataViewPanel = new wxPanel(rightSplitter);
    dataViewPanel->SetBackgroundColour(wxColour(240, 240, 240)); 

    SensorDataPanel* sensorDataGrid = new SensorDataPanel(dataViewPanel, sensorNames);
    
    wxBoxSizer* dataViewSizer = new wxBoxSizer(wxVERTICAL);
    dataViewSizer->Add(sensorDataGrid, 1, wxEXPAND);
    dataViewPanel->SetSizer(dataViewSizer);


    // Graph panel area
    wxPanel* graphPanel = new wxPanel(rightSplitter);
    graphPanel->SetBackgroundColour(wxColour(180, 180, 180));
    wxStaticText* graphLabel = new wxStaticText(graphPanel, wxID_ANY, "GRAPH PANEL AREA",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    wxBoxSizer* graphSizer = new wxBoxSizer(wxVERTICAL);
    graphSizer->AddStretchSpacer();
    graphSizer->Add(graphLabel, 0, wxALIGN_CENTER);
    graphSizer->AddStretchSpacer();
    graphPanel->SetSizer(graphSizer);


    // Create text control for displaying messages (Console)
    wxPanel* consolePanel = new wxPanel(mainSplitter);
    wxBoxSizer* consoleSizer = new wxBoxSizer(wxVERTICAL);
    messageDisplay_ = new wxTextCtrl(consolePanel, wxID_ANY, "",
                                     wxDefaultPosition, wxDefaultSize,
                                     wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP);
    messageDisplay_->SetBackgroundColour(wxColour(40, 40, 40));
    messageDisplay_->SetForegroundColour(wxColour(255, 255, 255));

    wxStaticText* consoleLabel = new wxStaticText(consolePanel, wxID_ANY, "Console");

    consoleSizer->Add(consoleLabel, 0, wxALL, 5);
    consoleSizer->Add(messageDisplay_, 1, wxEXPAND | wxALL, 5);
    consolePanel->SetSizer(consoleSizer);

    // Assemble splitters
    
    rightSplitter->SplitHorizontally(dataViewPanel, graphPanel, 300);
    rightSplitter->SetMinimumPaneSize(50);  
    
    topSplitter->SplitVertically(sensorPanel, rightSplitter, 200);
    topSplitter->SetMinimumPaneSize(100);

    int windowHeight = size.GetHeight();
    int consoleHeight = 150; // fixed height
    mainSplitter->SplitHorizontally(topSplitter, consolePanel, -150);
    mainSplitter->SetMinimumPaneSize(50);
    
    // Layout
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(mainSplitter, 1, wxEXPAND);
    SetSizer(mainSizer);

    // Register as observer
    model_->addObserver([this]() {
        // Use CallAfter to update GUI from non-GUI thread
        CallAfter(&MainFrame::updateMessageDisplay);
    });

    CreateStatusBar();
}

void MainFrame::updateMessageDisplay() {
    auto messages = model_->getMessages();
    wxString text;
    for (const auto& msg : messages) {
        text += wxString::FromUTF8(msg.c_str()) + "\n";
    }
    messageDisplay_->SetValue(text);
    messageDisplay_->SetInsertionPointEnd();
}