#include <accordserver/Arguments.h>

#include <iostream>

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>

namespace accord {

namespace po = boost::program_options;

Arguments ArgumentParser::parse(int argc, char **argv)
{
	Arguments arguments = { };
	po::options_description desc("Server Options");
	desc.add_options()
		("help", "print out help message\n")
		("port", po::value<int>(), "set server listen port\ndefault = 6524\n")
		("threads", po::value<int>(), "set the number of threads that will be used\n")
		("loglevel", po::value<int>(), "set the loglevel\n"
		 "0 - errors only\n"
		 "1 - warning and error\n"
		 "2 - warning, info and error\n"
		 "3 - warning, info, debug and error");
	po::variables_map varMap;
	po::store(po::parse_command_line(argc, argv, desc), varMap);
	po::notify(varMap);

	if (varMap.count("help")) {
		std::cout << desc << std::endl;
		exit(0);
	}

	if (varMap.count("port"))
		arguments.port = varMap["port"].as<int>();
	else
		arguments.port = 6524;

	if (varMap.count("threads"))
		arguments.threads = varMap["threads"].as<int>();
	else
		arguments.threads = 4;

	if (varMap.count("loglevel"))
		arguments.logLevel = varMap["loglevel"].as<int>();
	else
		arguments.logLevel = 2;

	return arguments;
}

} /* namespace accord */
