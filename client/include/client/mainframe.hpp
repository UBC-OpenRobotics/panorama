#ifndef __MAINFRAME__
#define __MAINFRAME__
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/notebook.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/timer.h>
#include <wx/socket.h>
#include <vector>
#include <memory>
#include <map>
#include <atomic>
#include "client/sensor_data_panel.h"
#include "client/sensor_manager.hpp"
#include "client/sensor.hpp"
#include "client/graph_panel.hpp"
#include <set>

class MessageModel;
class DataBuffer;
class TcpClient;
class Esp32Scanner;
class SensorDataManager;

class MainFrame : public wxFrame {
public:
    enum {
        ID_FILE_NEW = wxID_HIGHEST + 1,
        ID_FILE_OPEN,
        ID_FILE_SAVE,
        ID_FILE_EXIT,
        ID_EDIT_UNDO,
        ID_EDIT_REDO,
        ID_EDIT_PREFERENCES,
        ID_VIEW_CONSOLE,
        ID_VIEW_FULLSCREEN,
        ID_SETTINGS_OPEN,
        ID_BTN_START,
        ID_BTN_STOP,
        ID_ESP32_AUTOSTART,
        ID_ESP32_CONNECT,
        ID_ESP32_DISMISS
    };

    MainFrame(const wxString& title, std::shared_ptr<MessageModel> model,
        std::shared_ptr<DataBuffer> dataBuffer,
        TcpClient* tcpClient,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(1200, 800));

private:
    void onModelUpdated();
    void onSensorToggled();
    void CreateMenuBar();
    void updateMessageDisplay();
    void updateDataPanel();
    SensorDataFrame* sensorDataGrid;
    SensorManagerPanel* sensorManager_;

    //List of sensors in sensor manager
    std::set<std::string> registeredSensors_; 

    void OnFileNew(wxCommandEvent& event);
    void OnFileOpen(wxCommandEvent& event);
    void OnFileSave(wxCommandEvent& event);
    void OnFileExit(wxCommandEvent& event);
    void OnEditPreferences(wxCommandEvent& event);
    void OnViewConsole(wxCommandEvent& event);
    void OnViewFullscreen(wxCommandEvent& event);
    void OnSettingsOpen(wxCommandEvent& event);
    void OnStartStream(wxCommandEvent& event);
    void OnStopStream(wxCommandEvent& event);
    void OnEsp32Autostart(wxCommandEvent& event);
    void OnEsp32Connect(wxCommandEvent& event);
    void OnEsp32Dismiss(wxCommandEvent& event);
    void ShowEsp32Banner(bool show);

    std::shared_ptr<MessageModel> model_;
    std::shared_ptr<DataBuffer> dataBuffer_;
    TcpClient* tcpClient_;
    wxTextCtrl* messageDisplay_;
    wxPanel* consolePanel_;
    GraphPanel* graphPanel_;

    wxTimer updateTimer_;
    std::atomic<bool> updatePending_{false};
    size_t displayedMessageCount_ = 0;
    size_t displayedBufferCount_ = 0;

    void OnUpdateTimer(wxTimerEvent& event);

    // Auto detecting ESP32s
    std::unique_ptr<Esp32Scanner> esp32Scanner_;
    wxPanel* esp32Banner_ = nullptr;
    wxBoxSizer* mainSizer_ = nullptr;
    std::atomic<bool> esp32BannerPending_{false};
    bool esp32BannerVisible_ = false;
};

#endif // __MAINFRAME__