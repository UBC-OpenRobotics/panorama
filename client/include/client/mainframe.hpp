#pragma once
#include <wx/wx.h>

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title, 
              const wxPoint& pos = wxDefaultPosition, 
              const wxSize& size = wxDefaultSize);
    ~MainFrame() override = default;
};