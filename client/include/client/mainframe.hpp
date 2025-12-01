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

//#include "client/graph_panel.hpp"
//#include "client/sensor_data_panel.h"

class MessageModel;
//class GraphPanel;
class SensorDataManager;
//class SensorDataPanel;

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
