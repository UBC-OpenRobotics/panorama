#include "client/mainframe.hpp"
#include "client/message_model.hpp"
#include "client/DataBuffer.hpp"
#include "client/graph_panel.hpp"
#include "client/sensor_data_panel.h"
#include "client/sensor_manager.hpp"
#include <wx/dcbuffer.h>
#include <wx/sizer.h>

MainFrame::MainFrame(const wxString& title, std::shared_ptr<MessageModel> model,
    std::shared_ptr<DataBuffer> dataBuffer,
    const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size), model_(model), dataBuffer_(dataBuffer) {
    
    // Create splitter for layout
    wxSplitterWindow* mainSplitter = new wxSplitterWindow(this, wxID_ANY);
    wxSplitterWindow* topSplitter = new wxSplitterWindow(mainSplitter, wxID_ANY);
    wxSplitterWindow* rightSplitter = new wxSplitterWindow(topSplitter, wxID_ANY);
    // Create the four main panels
    // Sensor manager panel
    SensorManagerPanel* sensorPanel = new SensorManagerPanel(topSplitter);

    // Data view area - Sensor Data Panel
    wxArrayString sensorNames;
    sensorNames.Add("temperature");
    sensorNames.Add("humidity");
    sensorNames.Add("pressure");
    sensorNames.Add("light");

    wxPanel* dataViewPanel = new wxPanel(rightSplitter);
    dataViewPanel->SetBackgroundColour(wxColour(240, 240, 240)); 
    
    sensorDataGrid = new SensorDataFrame(dataViewPanel, sensorNames);
    
    wxBoxSizer* dataViewSizer = new wxBoxSizer(wxVERTICAL);
    dataViewSizer->Add(sensorDataGrid, 1, wxEXPAND);
    dataViewPanel->SetSizer(dataViewSizer);


    // Graph panel area
    GraphPanel* graphPanel = new GraphPanel(rightSplitter);

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
    rightSplitter->SplitHorizontally(dataViewPanel, graphPanel, 180);
    rightSplitter->SetMinimumPaneSize(100);
    rightSplitter->SetSashGravity(0.0); // keeps data panel a 180px, graph takes extra space
    
    topSplitter->SplitVertically(sensorPanel, rightSplitter, 200);
    topSplitter->SetMinimumPaneSize(100);
    topSplitter->SetSashGravity(0.0); 

    int windowHeight = size.GetHeight();
    int consoleHeight = 150; // fixed height
    mainSplitter->SplitHorizontally(topSplitter, consolePanel, -150);
    mainSplitter->SetMinimumPaneSize(50);
    mainSplitter->SetSashGravity(1.0); // keeps console constant at 150px
    
    // Layout
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(mainSplitter, 1, wxEXPAND);
    SetSizer(mainSizer);

    // Register as observer
    model_->addObserver([this]() {
        // Use CallAfter to update GUI from non-GUI thread
        CallAfter(&MainFrame::updateMessageDisplay);
        CallAfter(&MainFrame::updateDataPanel);
    });

    CreateStatusBar();
}

void MainFrame::updateMessageDisplay() {
    auto messages = model_->getMessages();
    wxString text;
    for (const auto& msg : messages) {
        text += wxString::FromUTF8(msg.c_str()) + "\n";
    }

    if (dataBuffer_->size() > 0) {
        //std::cout << dataBuffer_->toStringAll();
        text += "\n--- DataBuffer Contents ---\n";
        text += wxString::FromUTF8(dataBuffer_->toStringAll());
        text += "--- End of DataBuffer ---\n";
    }

    messageDisplay_->SetValue(text);
    messageDisplay_->SetInsertionPointEnd();
}

void MainFrame::updateDataPanel() {

    /*
    float data; // actual value
    std::time_t timestamp; // date recorded
    std::string dataunit; // e.g. "kPa", "mL"
    std::string datatype; // e.g. "temperature", "sound"
    */

    //if dataBuffer has data
    //dataBuffer_ is a list of type data_buffer_t, which has fields: datatype, data, dataunit, timestamp
    //when this function is called, update every sensors
    //currently we have temperature, humidity, pressure, and light

    //make a loop going through the list. call the updateReading function for each element


    if (dataBuffer_->size() > 0) {
        for (buffer_data_t latestData : dataBuffer_->readAll()) {

            sensorDataGrid->UpdateReading(latestData.datatype, (double)latestData.data, latestData.dataunit);
            
            std::cout << "Updated " << latestData.datatype << " with value: " << latestData.data << " " << latestData.dataunit << std::endl;
        }
    }
}

