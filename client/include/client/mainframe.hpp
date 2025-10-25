#pragma once
#include <wx/wx.h>
#include <memory>

class MessageModel;

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title, std::shared_ptr<MessageModel> model,
              const wxPoint& pos = wxDefaultPosition, 
              const wxSize& size = wxSize(600, 400));
    
private:
    void updateMessageDisplay();
    
    std::shared_ptr<MessageModel> model_;
    wxTextCtrl* messageDisplay_;
};