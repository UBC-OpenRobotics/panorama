#pragma once
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

#include "client/graph_panel.hpp"
#include "client/sensor_data_frame.hpp"

class MessageModel;
//class GraphPanel;
class SensorDataManager;
//class SensorDataFrame;  // Add this

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title, std::shared_ptr<MessageModel> model,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(1200, 800));

private:
    void updateMessageDisplay();

    std::shared_ptr<MessageModel> model_;
    wxTextCtrl* messageDisplay_;
};

/*
class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title, std::shared_ptr<MessageModel> model,
              const wxPoint& pos = wxDefaultPosition, 
              const wxSize& size = wxSize(600, 400));
	virtual ~MainFrame() = default;
private:
    void updateMessageDisplay();
    void OnOpenSensorData(wxCommandEvent& event);  // Add this

    std::sharedptr<MessageModel> model;
    wxTextCtrl* messageDisplay;
    SensorPanel* sensorPanel;
    SensorDataFrame* sensorDataFrame_;  // Add this

};
*/

/*
private:
    void updateMessageDisplay();
    void OnOpensensorData(wxCommandEvent& event);

    //UI Components
    wxSplitterWindow* leftSplitter_;
    wxSplitterWindow* rightSplitter_;
	wxPanel* sensorManagerPanel_;
	wxPanel* dataViewerPanel_;
    wxPanel* graphContainerPanel_;
	wxNotebook* notebook_;
    wxTextCtrl* consoleText_;
    wxButton* addDataBtn_;
    wxButton* settingsBtn_;
    wxButton* saveDataBtn_;
	wxButton* leftToggleBtn_;
	wxButton* rightToggleBtn_;

    //Sensor controls
	std::vector<wxCheckBox*> sensorCheckBoxes_;
    wxTextCtrl* searchBox_;

    //Data viewer
	wxPanel* rawDataPanel_;
	std::map<int, wxStaticText*> currentValueLabels_;
	std::map<int, wxStaticText*> averageValue_;

    //Graph panel
    GraphPanel* m_liveGraphPanel;
	GraphPanel* m_historicalGraphPanel;

    //Data management
    std::shared_ptr<MessageModel> model_;               // console messages
    wxTextCtrl* messageDisplay_;
    std::shared_ptr<SensorDataManager> dataManager_;    // sensor data

    //State
    bool leftPanelVisible_;
    bool rightPanelVisible_;
    int leftPanelWidth_;
    int rightPanelWidth_;

    // Configuration
	int sampleRateMs_;
    std::map<int, double> sensorLimits_;

    // Event handlers
	void OnToggleLeftPanel(wxCommandEvent& event);
	void OnToggleRightPanel(wxCommandEvent& event);
	void OnSensorCheckBox(wxCommandEvent& event);
	void OnAddData(wxCommandEvent& event);
	void OnSettings(wxCommandEvent& event);
	void OnSaveData(wxCommandEvent& event);
	void OnUpdateTimer(wxTimerEvent& event);
	void OnSocketEvent(wxSocketEvent& event);
    void OnSearch(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);

	// Helper methods
	void CreateMenuBar();
    void CreateSensorMAnager();
	void CreateDataViewer();
	void CreateGraphPanel();
	void CreateConsole();
	void UpdateSensorDisplays();
	void UpdateGraphs();
    void CheckSensorLimits();
	void LogToConsole(const wxString& message);
	void ExportToCSV(const wxString& filename);

	wxDECLARE_EVENT_TABLE();
};

*/