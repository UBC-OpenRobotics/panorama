#include <wx/wx.h>
#include <wx/timer.h>
#include <memory>
#include "client/mainframe.hpp"
#include "client/argparser.hpp"
#include "client/message_model.hpp"
#include "client/tcp_client.hpp"

// ALEX WAS HERE!!!!! AHHHAA


// fhdusiah fish foia
// fdsa fdas f
// da fdsf 
// ad fsda 

// Hello Nami was here firstFIRST!!!!! I DONT CARE!!!!!
// ALEX HERE
class PanoramaClient : public wxApp {
public:
    virtual bool OnInit() override {
        ArgParser parser(argc, argv);
        
        // Create model (shared between view and controller)
        model_ = std::make_shared<MessageModel>();
        
        // Create and start TCP client on separate thread
        tcpClient_ = std::make_unique<TcpClient>("127.0.0.1", 3000, model_);
        tcpClient_->start();
        
        // Create view
        MainFrame* w = new MainFrame("Panorama Client", model_);
        w->Show();
        
        // If test mode, quit after 3 seconds
        if (parser.isTestMode()) {
            wxTimer* timer = new wxTimer(this);
            
            Bind(wxEVT_TIMER, [this, timer](wxTimerEvent&) {
                timer->Stop();
                wxTheApp->ExitMainLoop(); 
            });
            timer->StartOnce(3000);
        }

        return true;
    }
    
    virtual int OnExit() override {
        // Clean shutdown of TCP client
        if (tcpClient_) {
            tcpClient_->stop();
        }
        return wxApp::OnExit();
    }
    
private:
    std::shared_ptr<MessageModel> model_;
    std::unique_ptr<TcpClient> tcpClient_;
};

wxIMPLEMENT_APP(PanoramaClient);