#include "client/mainframe.hpp"
#include "client/message_model.hpp"
#include "client/DataBuffer.hpp"
#include "client/graph_panel.hpp"
#include "client/sensor_data_panel.h"
#include "client/sensor_manager.hpp"
#include "client/sensor.hpp"
#include "client/settings_dialog.hpp"
#include "common/panorama_utils.hpp"
#include "common/panorama_colours.hpp"
#include "client/tcp_client.hpp"
#include "client/config_manager.hpp"
#include "client/esp32_scanner.hpp"
#include <wx/dcbuffer.h>
#include <wx/sizer.h>
#include <functional>

MainFrame::MainFrame(const wxString& title, std::shared_ptr<MessageModel> model,
    std::shared_ptr<DataBuffer> dataBuffer,
    TcpClient* tcpClient,
    const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size), model_(model), dataBuffer_(dataBuffer), tcpClient_(tcpClient) {

    CreateMenuBar();

    updateTimer_.Bind(wxEVT_TIMER, &MainFrame::OnUpdateTimer, this);
    updateTimer_.Start(10); // milliseconds between GUI refreshes

    // Control bar with start and stop btns
    wxPanel* controlBar = new wxPanel(this);
    wxBoxSizer* controlSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* startBtn = new wxButton(controlBar, ID_BTN_START, wxString::FromUTF8("\u25B6 Start"));
    startBtn->SetBackgroundColour(PCOLOUR_GREEN);
    startBtn->SetForegroundColour(*wxWHITE);
    wxButton* stopBtn = new wxButton(controlBar, ID_BTN_STOP, wxString::FromUTF8("\u25A0 Stop"));
    stopBtn->SetBackgroundColour(PCOLOUR_RED);
    stopBtn->SetForegroundColour(*wxWHITE);
    controlSizer->Add(startBtn, 0, wxALL, 4);
    controlSizer->Add(stopBtn, 0, wxALL, 4);
    controlBar->SetSizer(controlSizer);

    Bind(wxEVT_BUTTON, &MainFrame::OnStartStream, this, ID_BTN_START);
    Bind(wxEVT_BUTTON, &MainFrame::OnStopStream, this, ID_BTN_STOP);

    // ESP32 notification banner (hidden by default)
    esp32Banner_ = new wxPanel(this);
    esp32Banner_->SetBackgroundColour(PCOLOUR_BLUE);
    wxBoxSizer* bannerSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* bannerLabel = new wxStaticText(esp32Banner_, wxID_ANY,
        wxString::FromUTF8("  ESP32 device detected on network (192.168.4.1:9000)"));
    bannerLabel->SetForegroundColour(*wxWHITE);
    wxButton* autostartBtn = new wxButton(esp32Banner_, ID_ESP32_AUTOSTART, "Connect && Autostart");
    wxButton* connectBtn = new wxButton(esp32Banner_, ID_ESP32_CONNECT, "Connect");
    wxButton* dismissBtn = new wxButton(esp32Banner_, ID_ESP32_DISMISS, "Dismiss");
    bannerSizer->Add(bannerLabel, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    bannerSizer->Add(autostartBtn, 0, wxALL, 3);
    bannerSizer->Add(connectBtn, 0, wxALL, 3);
    bannerSizer->Add(dismissBtn, 0, wxALL, 3);
    esp32Banner_->SetSizer(bannerSizer);
    esp32Banner_->Hide();

    Bind(wxEVT_BUTTON, &MainFrame::OnEsp32Autostart, this, ID_ESP32_AUTOSTART);
    Bind(wxEVT_BUTTON, &MainFrame::OnEsp32Connect, this, ID_ESP32_CONNECT);
    Bind(wxEVT_BUTTON, &MainFrame::OnEsp32Dismiss, this, ID_ESP32_DISMISS);

    // Create splitter for layout
    wxSplitterWindow* mainSplitter = new wxSplitterWindow(this, wxID_ANY);
    wxSplitterWindow* topSplitter = new wxSplitterWindow(mainSplitter, wxID_ANY);
    wxSplitterWindow* rightSplitter = new wxSplitterWindow(topSplitter, wxID_ANY);
    // Create the four main panels
    // Sensor manager panel
    sensorManager_ = new SensorManagerPanel(topSplitter);

    // Data view area - Sensor Data Panel (rows added dynamically as sensors arrive)
    wxPanel* dataViewPanel = new wxPanel(rightSplitter);
    dataViewPanel->SetBackgroundColour(PCOLOUR_PANEL_GREY); 
    
    sensorDataGrid = new SensorDataFrame(dataViewPanel, wxArrayString());
    
    wxBoxSizer* dataViewSizer = new wxBoxSizer(wxVERTICAL);
    dataViewSizer->Add(sensorDataGrid, 1, wxEXPAND);
    dataViewPanel->SetSizer(dataViewSizer);


    // Graph panel area
    GraphPanel* graphPanel = new GraphPanel(rightSplitter);

    // Create text control for displaying messages (Console)
    consolePanel_ = new wxPanel(mainSplitter);
    wxBoxSizer* consoleSizer = new wxBoxSizer(wxVERTICAL);
    messageDisplay_ = new wxTextCtrl(consolePanel_, wxID_ANY, "",
                                     wxDefaultPosition, wxDefaultSize,
                                     wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP);
    messageDisplay_->SetBackgroundColour(PCOLOUR_DARK_GREY);
    messageDisplay_->SetForegroundColour(PCOLOUR_WHITE);

    wxStaticText* consoleLabel = new wxStaticText(consolePanel_, wxID_ANY, "Console");

    consoleSizer->Add(consoleLabel, 0, wxALL, 5);
    consoleSizer->Add(messageDisplay_, 1, wxEXPAND | wxALL, 5);
    consolePanel_->SetSizer(consoleSizer);

    // Assemble splitters
    rightSplitter->SplitHorizontally(dataViewPanel, graphPanel, 180);
    rightSplitter->SetMinimumPaneSize(100);
    rightSplitter->SetSashGravity(0.0); // keeps data panel a 180px, graph takes extra space
    
    topSplitter->SplitVertically(sensorManager_, rightSplitter, 200);
    topSplitter->SetMinimumPaneSize(100);
    topSplitter->SetSashGravity(0.0);

    int windowHeight = size.GetHeight();
    int consoleHeight = 150; // fixed height
    mainSplitter->SplitHorizontally(topSplitter, consolePanel_, -150);
    mainSplitter->SetMinimumPaneSize(50);
    mainSplitter->SetSashGravity(1.0); // keeps console constant at 150px

    // Layout
    mainSizer_ = new wxBoxSizer(wxVERTICAL);
    mainSizer_->Add(controlBar, 0, wxEXPAND);
    mainSizer_->Add(esp32Banner_, 0, wxEXPAND);
    mainSizer_->Add(mainSplitter, 1, wxEXPAND);
    SetSizer(mainSizer_);

    // Register as observer
    model_->addObserver(std::bind(&MainFrame::onModelUpdated, this));

    // Register checkbox toggle callback
    sensorManager_->SetOnSensorToggled(std::bind(&MainFrame::onSensorToggled, this));

    CreateStatusBar();

    // Start ESP32 auto-detection scanner
    esp32Scanner_ = std::make_unique<Esp32Scanner>();
    esp32Scanner_->setOnAvailabilityChanged([this](bool available) {
        if (available) {
            esp32BannerPending_.store(true);
        }
    });
    esp32Scanner_->start();
}

void MainFrame::onModelUpdated() {
    updatePending_.store(true);
}

void MainFrame::onSensorToggled() {
    sensorDataGrid->SetActiveSensors(sensorManager_->GetEnabledSensorNames());
}

void MainFrame::updateMessageDisplay() {
    // Append only new messages
    auto messages = model_->getMessages();
    for (size_t i = displayedMessageCount_; i < messages.size(); ++i) {
        messageDisplay_->AppendText(wxString::FromUTF8(messages[i].c_str()) + "\n");
    }
    displayedMessageCount_ = messages.size();

    // Append only new buffer entries
    auto allBuffer = dataBuffer_->readAll();
    size_t i = 0;
    for (const auto& entry : allBuffer) {
        if (i >= displayedBufferCount_) {
            messageDisplay_->AppendText(wxString::FromUTF8(dataBuffer_->toString(entry)));
        }
        ++i;
    }
    displayedBufferCount_ = allBuffer.size();
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

            // Skip entries with no data-type (first sensor reading)
            if (latestData.datatype.empty()) continue;

            // Add Sensors to sensor manager and data grid only when they are first seen
            if (registeredSensors_.find(latestData.datatype) == registeredSensors_.end()) {
                auto sensor = std::make_shared<Sensor>(latestData.datatype, latestData.dataunit);
                sensorManager_->AddSensor(sensor);
                sensorDataGrid->AddSensorRow(latestData.datatype);
                registeredSensors_.insert(latestData.datatype);
            }

            sensorDataGrid->UpdateReading(latestData.datatype, (double)latestData.data, latestData.dataunit);
            
            //std::cout << "Updated " << latestData.datatype << " with value: " << latestData.data << " " << latestData.dataunit << std::endl;
        }
    }
}



void MainFrame::CreateMenuBar() {
    wxMenuBar* menuBar = new wxMenuBar();

    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(ID_FILE_NEW, "&New\tCtrl+N", "Create a new file");
    fileMenu->Append(ID_FILE_OPEN, "&Open\tCtrl+O", "Open a file");
    fileMenu->Append(ID_FILE_SAVE, "&Save\tCtrl+S", "Save the file");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_FILE_EXIT, "E&xit\tAlt+F4", "Exit the application");

    wxMenu* editMenu = new wxMenu();
    editMenu->Append(ID_EDIT_UNDO, "&Undo\tCtrl+Z", "Undo last action");
    editMenu->Append(ID_EDIT_REDO, "&Redo\tCtrl+Y", "Redo last action");
    editMenu->AppendSeparator();
    editMenu->Append(ID_EDIT_PREFERENCES, "&Preferences...", "Open preferences");

    wxMenu* viewMenu = new wxMenu();
    viewMenu->AppendCheckItem(ID_VIEW_CONSOLE, "&Console\tCtrl+`", "Toggle console");
    viewMenu->Check(ID_VIEW_CONSOLE, true);
    viewMenu->AppendSeparator();
    viewMenu->Append(ID_VIEW_FULLSCREEN, "&Fullscreen\tF11", "Toggle fullscreen");  

    wxMenu* settingsMenu = new wxMenu();
    settingsMenu->Append(ID_SETTINGS_OPEN, "&Open Settings...\tCtrl+,", "Open settings dialog");

    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    menuBar->Append(viewMenu, "&View");
    menuBar->Append(settingsMenu, "&Settings");

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &MainFrame::OnFileNew, this, ID_FILE_NEW);
    Bind(wxEVT_MENU, &MainFrame::OnFileOpen, this, ID_FILE_OPEN);
    Bind(wxEVT_MENU, &MainFrame::OnFileSave, this, ID_FILE_SAVE);
    Bind(wxEVT_MENU, &MainFrame::OnFileExit, this, ID_FILE_EXIT);
    Bind(wxEVT_MENU, &MainFrame::OnEditPreferences, this, ID_EDIT_PREFERENCES);
    Bind(wxEVT_MENU, &MainFrame::OnViewConsole, this, ID_VIEW_CONSOLE);
    Bind(wxEVT_MENU, &MainFrame::OnViewFullscreen, this, ID_VIEW_FULLSCREEN);
    Bind(wxEVT_MENU, &MainFrame::OnSettingsOpen, this, ID_SETTINGS_OPEN);
}

void MainFrame::OnFileNew(wxCommandEvent& event) {
    // TODO: new file stuff
}

void MainFrame::OnFileOpen(wxCommandEvent& event) {
    wxFileDialog dialog(this, "Open file", "", "",
                        "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dialog.ShowModal() == wxID_OK) {
        // TODO: Handle file open
        wxString path = dialog.GetPath();
    }
}

void MainFrame::OnFileSave(wxCommandEvent& event) {
// Todo:
}

void MainFrame::OnFileExit(wxCommandEvent& event) {
    Close(true);
}

void MainFrame::OnEditPreferences(wxCommandEvent& event) {
    // Open settings dialog - you already have SettingsDialog
    // SettingsDialog dialog(this);
    // dialog.ShowModal();
}

void MainFrame::OnViewConsole(wxCommandEvent& event) {
    // Toggle console visibility
    bool isVisible = consolePanel_->IsShown();
    consolePanel_->Show(!isVisible);
    Layout();
}

void MainFrame::OnViewFullscreen(wxCommandEvent& event) {
    ShowFullScreen(!IsFullScreen());
}

void MainFrame::OnSettingsOpen(wxCommandEvent& event) {
    SettingsDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK && tcpClient_) {
        ConfigManager& config = ConfigManager::getInstance();
        std::string host;
        int port;
        bool autoReconnect;
        int reconnectDelay;
        if (config.getTcpSettings(host, port, autoReconnect, reconnectDelay)) {
            tcpClient_->reconnectWith(host, port);
        }
    }
}

void MainFrame::OnStartStream(wxCommandEvent& event) {
    if (tcpClient_) {
        tcpClient_->sendCommand("START");
    }
}

void MainFrame::OnStopStream(wxCommandEvent& event) {
    if (tcpClient_) {
        tcpClient_->sendCommand("STOP");
    }
}

void MainFrame::OnUpdateTimer(wxTimerEvent&) {
    if (updatePending_.exchange(false)) {
        updateMessageDisplay();
        updateDataPanel();
    }

    // Makesure the esp is actually detected
    if (esp32BannerPending_.exchange(false)) {
        ShowEsp32Banner(true);
    }
}

void MainFrame::ShowEsp32Banner(bool show) {
    if (esp32BannerVisible_ == show) return;
    esp32BannerVisible_ = show;
    esp32Banner_->Show(show);
    mainSizer_->Layout();
}

void MainFrame::OnEsp32Autostart(wxCommandEvent& event) {
    if (tcpClient_) {
        tcpClient_->reconnectWith(
            Esp32Scanner::ESP32_DEFAULT_HOST,
            Esp32Scanner::ESP32_DEFAULT_PORT);
        model_->addMessage("Connecting to ESP32 at 192.168.4.1:9000 (autostart)...");
        tcpClient_->sendCommand("START");
    }
    ShowEsp32Banner(false);
    if (esp32Scanner_) {
        esp32Scanner_->stop();
    }
}

void MainFrame::OnEsp32Connect(wxCommandEvent& event) {
    if (tcpClient_) {
        tcpClient_->reconnectWith(
            Esp32Scanner::ESP32_DEFAULT_HOST,
            Esp32Scanner::ESP32_DEFAULT_PORT);
        model_->addMessage("Connecting to ESP32 at 192.168.4.1:9000...");
    }
    ShowEsp32Banner(false);
    
    if (esp32Scanner_) {
        esp32Scanner_->stop();
    }
}

void MainFrame::OnEsp32Dismiss(wxCommandEvent& event) {
    ShowEsp32Banner(false);
}