#include <wx/wx.h>
#include <wx/timer.h>
#include <memory>
#include "client/mainframe.hpp"
#include "client/argparser.hpp"
#include "client/message_model.hpp"
#include "client/tcp_client.hpp"
#include "client/json_reader.hpp" 
#include <iostream>
using namespace std;

class PanoramaClient : public wxApp {
public:
    virtual bool OnInit() override {
        ArgParser parser(argc, argv);

        // --- Load JSON data at startup ---
        // JsonReader reader("example.json");
        // if (!reader.parse()) {
        //     wxLogError("Failed to parse data.json");
        //     return false; // stop app if JSON fails
        // }

        // const auto& doc = reader.getDocument();

        // const rapidjson::Value& array = doc["cats"];

        // for (const auto& item : array.GetArray()) {
        //     cout << "Type: " << item["type"].GetString() << " | Age: ";
        //     cout << item["age"].GetInt() << " | Color: ";
        //     cout << item["color"].GetString() << "\n";
        // }


        // --- Create model (shared between view and controller) ---
        model_ = std::make_shared<MessageModel>();

        // --- Create and start TCP client on separate thread ---
        tcpClient_ = std::make_unique<TcpClient>("127.0.0.1", 3000, model_);
        tcpClient_->start();

        // For running without a gui
        if (parser.isNoGuiMode()) {
            cout << "Running in console mode (no GUI). Press Ctrl+C to exit.\n";
            cout << "Listening for JSON stream...\n\n";
            
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1)); // Run forever in console
            }
            return true;
        }

        // --- Create view ---
        MainFrame* w = new MainFrame("Panorama Client", model_);
        w->Show();

        // --- If test mode, quit after 3 seconds ---
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
