#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <vector>
#include <wx/spinctrl.h>
#include <wx/richtooltip.h>
#include <wx/scrolwin.h>
#include <wx/wxhtml.h>
#include <boost/asio.hpp>

using std::string;
namespace asio = boost::asio;


class MyApp : public wxApp
{
public:
    bool OnInit() override;
private:
    asio::io_context ioc;
    asio::serial_port* port = nullptr;
};
 
wxIMPLEMENT_APP(MyApp);

class FrameCommands : public wxFrame
{
public:
    FrameCommands(wxWindow *parent, asio::io_context& ioc, asio::serial_port* port = nullptr);
 
private:
    void OnConnect(wxCommandEvent& event);
    asio::serial_port* port = nullptr;
    asio::io_context& ioc;
};
 
bool MyApp::OnInit()
{
    FrameCommands *frame = new FrameCommands(nullptr, ioc, port);
    frame->Show(true);
    return true;
}

class Command : public wxStaticBoxSizer 
{
public:
    Command(wxWindow *parent, wxBoxSizer *sizer, asio::serial_port* port = nullptr);
    string name;
    string command;
    string description;
    std::vector<std::pair<std::string, std::string>> optinos;
    void create_add_to_sizer();
private:
    asio::serial_port* port = nullptr;
    wxWindow *parent;
};
 

FrameCommands::FrameCommands(wxWindow *parent, asio::io_context &ioc, asio::serial_port *port) : 
    wxFrame(parent, wxID_ANY, "Commands"), 
    ioc(ioc), 
    port(port)
{
    // wxButton* btn_init = new wxButton(this, wxID_ANY, "Init");
    Center();

    wxPanel *panel = new wxPanel(this); /// @todo make scrolled
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(sizer);

    auto aa = new Command(panel, sizer, port);
    aa->name = "Arc absolute";
    aa->command = "AA";
    aa->description = "Draws an arc around the absolute coordinate (x,y) starting from the current position with the arc angle a=[degree].  With a negative arc angle a, it is drawn clockwise, but otherwise anticlockwise.";
    aa->optinos.push_back(std::make_pair("x", "0"));
    aa->optinos.push_back(std::make_pair("y", "0"));
    aa->create_add_to_sizer();    
  
    wxStaticBoxSizer *box2 = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Arc relative");
    sizer->Add(box2, 0, wxEXPAND | wxALL, 5);
    box2->Add(new wxButton(panel, wxID_ANY, "AR"));
    
    wxRichToolTip tip("Arc relative", "Draws an arc around the relative coordinate (x,y) starting from the current position with the arc angle a=[degree].  With a negative arc angle a, it is drawn clockwise, but otherwise anticlockwise.");
};



Command::Command(wxWindow *parent, wxBoxSizer *sizer, asio::serial_port* port) : wxStaticBoxSizer(wxVERTICAL, parent, "Command")
{
    this->parent = parent;
    this->port = port;
    sizer->Add(this, 0, wxEXPAND | wxALL, 5);
};

void Command::create_add_to_sizer()
{
    auto bsizer = new wxBoxSizer(wxHORIZONTAL);
    this->Add(bsizer, 0, wxEXPAND | wxALL, 5);
    bsizer->Add(new wxButton(parent, wxID_ANY, command), 0, wxEXPAND | wxALL, 5);
    for (auto option : optinos)
    {   
        bsizer->Add(new wxStaticText(parent, wxID_ANY, option.first), 0, wxALL, 5);
        bsizer->Add(new wxSpinCtrl(parent, wxID_ANY, option.second), 0, wxALL, 5);
    }
    // auto text = new wxHtmlWindow(parent, wxID_ANY);
    // text->SetRelatedFrame();
    // text->SetRelatedStatusBar(0);
    // text->SetPage("<html><body>Hello, world!</body></html>");
    // this->Add(text , 0, wxEXPAND | wxALL | wxALIGN_LEFT, 5);
};