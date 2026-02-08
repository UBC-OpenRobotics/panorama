#include "client/mainframe.hpp"
#include "client/message_model.hpp"
#include "client/DataBuffer.hpp"
#include "client/graph_panel.hpp"
#include "client/sensor_data_panel.h"
#include "client/sensor_manager.hpp"
#include "client/settings_dialog.hpp"
#include <wx/dcbuffer.h>
#include <wx/sizer.h>

MainFrame::MainFrame(const wxString& title, std::shared_ptr<MessageModel> model,
    std::shared_ptr<DataBuffer> dataBuffer,
    const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size), model_(model), dataBuffer_(dataBuffer) {

    CreateMenuBar();

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
    sensorNames.Add("Sensor 6");
    sensorNames.Add("Sensor 7");
    sensorNames.Add("Sensor 8");
    sensorNames.Add("Another one");
    wxPanel* dataViewPanel = new wxPanel(rightSplitter);
    dataViewPanel->SetBackgroundColour(wxColour(240, 240, 240));

  SensorDataFrame* sensorDataGrid = new SensorDataFrame(dataViewPanel, sensorNames);

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
    messageDisplay_->SetBackgroundColour(wxColour(40, 40, 40));
    messageDisplay_->SetForegroundColour(wxColour(255, 255, 255));

    wxStaticText* consoleLabel = new wxStaticText(consolePanel_, wxID_ANY, "Console");

    consoleSizer->Add(consoleLabel, 0, wxALL, 5);
    consoleSizer->Add(messageDisplay_, 1, wxEXPAND | wxALL, 5);
    consolePanel_->SetSizer(consoleSizer);

    // Assemble splitters
    rightSplitter->SplitHorizontally(dataViewPanel, graphPanel, 180);
    rightSplitter->SetMinimumPaneSize(100);
    rightSplitter->SetSashGravity(0.0); // keeps data panel a 180px, graph takes extra space

    topSplitter->SplitVertically(sensorPanel, rightSplitter, 200);
    topSplitter->SetMinimumPaneSize(100);
    topSplitter->SetSashGravity(0.0);

    int windowHeight = size.GetHeight();
    int consoleHeight = 150; // fixed height
    mainSplitter->SplitHorizontally(topSplitter, consolePanel_, -150);
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
    dialog.ShowModal();
}
