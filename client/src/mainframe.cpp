#include "client/mainframe.hpp"
#include "client/message_model.hpp"

MainFrame::MainFrame(const wxString& title, std::shared_ptr<MessageModel> model,
                     const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size), model_(model) {
    
    // Create text control for displaying messages
    messageDisplay_ = new wxTextCtrl(this, wxID_ANY, "",
                                     wxDefaultPosition, wxDefaultSize,
                                     wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP);
    
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(messageDisplay_, 1, wxEXPAND | wxALL, 5);
    SetSizer(sizer);
    
    // Register as observer
    model_->addObserver([this]() {
        // Use CallAfter to update GUI from non-GUI thread
        CallAfter(&MainFrame::updateMessageDisplay);
    });
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