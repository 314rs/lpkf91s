#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>

#include  "version.hpp"

using std::string;
namespace asio = boost::asio;



int main(int argc, char const *argv[])
{
	namespace po = boost::program_options;
	po::variables_map vm;
	try
	{
		po::options_description desc{"Options"};
		desc.add_options()
		("help,h", "Help screen, show this message")
		("version" , "Show version number")
		("port,p", po::value<string>(), "Serial port name")
		("baud,b", po::value<int>(), "Serial port baudrate")
		("input,i", po::value<string>(), "input file name");
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm); // important if this->notifier() is set on an option

		if (vm.count("version")) {
			std::cout << "Version: " << string(GIT_VERSION_STRING) << std::endl;
		}
		if (vm.count("help")) {
			std::cout << desc << '\n' << "developed by Lars Bornträger" << std::endl;
			return 0;
		}
		if (argc == 1) {
			std::cout << desc << '\n' << "developed by Lars Bornträger" << std::endl;
			return 1;
		}

	}
	catch (const po::error &ex)
	{
		
		std::cerr << "before" <<ex.what() << "after" << std::endl;
	}
	

	asio::io_context ioc;
	try 
	{
		asio::serial_port port{ioc, vm["port"].as<string>()};
		port.set_option(asio::serial_port_base::baud_rate{9600});
		port.set_option(asio::serial_port_base::stop_bits{asio::serial_port_base::stop_bits::one});
		port.set_option(asio::serial_port_base::character_size{8});
		port.set_option(asio::serial_port_base::parity{asio::serial_port_base::parity::none});
		port.set_option(asio::serial_port_base::flow_control{asio::serial_port_base::flow_control::hardware});
		std::ifstream in{vm["input"].as<string>()};

		if (in.is_open()) {
			// count lines
			auto count = std::count_if(std::istreambuf_iterator<char>{in}, {}, [](char c) { return c == '\n'; });
			std::cout << "#lines: " << count << std::endl;
			// return to start of file
			in.clear();
			in.seekg(0);

			// activate feedback
			port.write_some(asio::buffer("!CT1;\n"));
			int linenum = 0;
			for(std::string line; std::getline(in, line); linenum++) {
				if (line == "") {
					continue;
				}
				std::cout << line << std::endl;
				port.write_some(asio::buffer(line));
				asio::streambuf buf;
				asio::read_until(port, buf, "C");

				//progress bar
				std::cout << "[";
				int pos = int(20.0 * linenum / count);
				for (int i = 0; i < 20; ++i) {
					if (i < pos) std::cout << "=";
					else if (i == pos) std::cout << ">";
					else std::cout << " ";
				}
				std::cout << "] " << int((linenum * 100.0)/count) << " %\r";
				std::cout.flush();
			}
			std::cout << "Done." << std::endl;
		}
	} 
	catch (const std::exception &e) 
	{
		std::cerr << e.what() << std::endl;
	}

	
	return 0;
}
