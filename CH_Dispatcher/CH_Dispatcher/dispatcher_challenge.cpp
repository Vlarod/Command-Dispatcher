#include <iostream>
#include <map>
#include <string>
#include <iostream>
#include <functional>
#include<algorithm>
#include <stdlib.h>
#include <random>
#include <windows.h>
#include <shellapi.h>

//
// supporting tools and software
//
// Validate and test your json commands
// https://jsonlint.com/

// RapidJSON : lots and lots of examples to help you use it properly
// https://github.com/Tencent/rapidjson
//

// std::function
// std::bind
// std::placeholders
// std::map
// std::make_pair

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace std;

bool g_done = false;

//
// TEST COMMANDS
//
auto help_command = R"({\"command\":\"help\", \"payload\":{\"usage\":\"-> Enter json command in console.\",\"usage2\":\"-> Command have to be on one line.\",\"usage3\":\"-> Press Enter.\"}})";

auto exit_command = R"({\"command\":\"exit\", \"payload\":{\"reason\":\"User requested exit.\"}})";

auto fibonacci_command = R"({\"command\":\"fibonacci\", \"payload\":{\"number1\":13, \"number2\":21}})";

auto random_numb_command = R"({\"command\":\"names\", \"payload\":{\"min\":0, \"max\":100, \"seed\":1582315}})";

auto about_command = R"({\"command\":\"about\", \"payload\":{\"url\":\"https://rapidjson.org/\", \"max\":100, \"seed\":1582315}})";

class Controller {
public:
	bool help(rapidjson::Value &payload)
	{
		cout << "\n\nController::help: command: \n";

		// implement
		Value::MemberIterator itr;
		itr = payload.FindMember("usage");
		rapidjson::Value& usage = itr->value;
		string usageV = usage.GetString();

		itr = payload.FindMember("usage2");
		usage = itr->value;
		string usageV2 = usage.GetString();
		
		itr = payload.FindMember("usage3");
		usage = itr->value;
		string usageV3 = usage.GetString();

		cout << usageV << endl;
		cout << usageV2 << endl;
		cout << usageV3 << endl;

		return true;
	}

	bool exit(rapidjson::Value &payload)
	{
		// implement
		cout << "\n\nController::exit: command: \n";
		Value::MemberIterator itr = payload.FindMember("reason");
		rapidjson::Value& reason = itr->value;
		string reasonV = reason.GetString();
		cout << reasonV << endl;

		g_done = true;

		return true;
	}

	bool fibonacci(rapidjson::Value &payload) {
		Value::MemberIterator itr;
		itr = payload.FindMember("number1");
		rapidjson::Value& usage = itr->value;
		int number1 = usage.GetInt();

		itr = payload.FindMember("number2");
		usage = itr->value;
		int number2 = usage.GetInt();

		int nextNumber = number1 + number2;
		itr->value.SetInt(nextNumber);

		return false;
	}

	bool randomNum(rapidjson::Value &payload) {
	
		Value::MemberIterator itr;
		itr = payload.FindMember("min");
		rapidjson::Value& value = itr->value;
		int minI = value.GetInt();

		itr = payload.FindMember("max");
		value = itr->value;
		int maxI = value.GetInt();

		itr = payload.FindMember("seed");
		value = itr->value;
		int seedI = value.GetInt();

		std::minstd_rand0 generator;
		generator.seed(seedI);
		int newNumber = minI + generator() % ((maxI + 1) - minI);
		
		cout << "Generated number from interval <" + std::to_string(minI) + ";" + to_string(maxI) + "> is: " + to_string(newNumber) << endl;
		return true;
	}

	bool about(rapidjson::Value &payload) {
		Value::MemberIterator itr;
		itr = payload.FindMember("url");
		rapidjson::Value& value = itr->value;
		string url = value.GetString();

		ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);

		return true;
	}

	// implement 3-4 more commands
};

// gimme ... this is actually tricky
// Bonus Question: why did I type cast this?
// Yoy want to define new type as pointer to member function from Controller. 
// This type can be executed with used function. 
typedef std::function<bool(rapidjson::Value &)> CommandHandler;

class CommandDispatcher {
public:
	// ctor - need impl
	CommandDispatcher()
	{
		command_handlers_ = std::map<std::string, CommandHandler>();
	}

	// dtor - need impl
	virtual ~CommandDispatcher()
	{
			// question why is it virtual? Is it needed in this case?
			// Virtual destructor is used in polymorphism to prevent abnormalities in the execution of the destructor.
			// In this case it is not needed.
	}

	bool addCommandHandler(std::string command, CommandHandler handler)
	{
		// implement
		cout << "CommandDispatcher: addCommandHandler: " << command << std::endl;
		command_handlers_.insert(std::pair<std::string, CommandHandler>(command, handler));
		
		return true;
	}

	bool dispatchCommand(std::string command_json)
	{
		// implement
		cout << "COMMAND: " << command_json << endl;
		string command = command_json;
		Document documet;
		if (command_handlers_.count(command)) {
			auto handler = command_handlers_.find(command);
			command.erase(remove(command.begin(), command.end(), '\\'), command.end());
			rapidjson::ParseResult result = documet.Parse(command.c_str());
			if (result) {
				rapidjson::Value& valueN = documet["payload"];
				if (!handler->second(valueN)) {
					StringBuffer buffer;
					Writer<StringBuffer> writer(buffer);
					documet.Accept(writer);
					string newCommand = buffer.GetString();

					addCommandHandler(newCommand, handler->second);
					command_handlers_.erase(command_json);

					printHandlers();
				}
				
			} else {
				  cout << "\nNOT VALID JSON COMMAND\n\n";
			}
		}
		else {
			cout << "\nCOMMAND NOT FOUND\n\n";
		}
		return true;
	}

	void printHandlers() {
		std::map<std::string, CommandHandler>::iterator it;

		system("CLS");
		for (it = command_handlers_.begin(); it != command_handlers_.end(); it++)
		{
			cout << "CommandDispatcher: addCommandHandler: " << it->first << std::endl;

		}
	}
		

private:

	// gimme ...
	std::map<std::string, CommandHandler> command_handlers_;

	// another gimme ...
	// Question: why delete these?
	// It is not delete but we do not want allow the usage of copy-constructor for this class.

	// delete unused constructors
	CommandDispatcher(const CommandDispatcher&) = delete;
	CommandDispatcher& operator= (const CommandDispatcher&) = delete;

};


int main()
{
	std::cout << "COMMAND DISPATCHER: STARTED" << std::endl;

	CommandDispatcher command_dispatcher;
	Controller controller;                 // controller class of functions to "dispatch" from Command Dispatcher
	// Implement
	// add command handlers in Controller class to CommandDispatcher using addCommandHandler
	CommandHandler handlerExit = std::bind(&Controller::exit, controller, std::placeholders::_1);
	CommandHandler handlerHelp = std::bind(&Controller::help, controller, std::placeholders::_1);
	CommandHandler handlerAbout = std::bind(&Controller::about, controller, std::placeholders::_1);
	CommandHandler handlerFibonacci = std::bind(&Controller::fibonacci, controller, std::placeholders::_1);
	CommandHandler handlerRandNum = std::bind(&Controller::randomNum, controller, std::placeholders::_1);

	command_dispatcher.addCommandHandler(exit_command, handlerExit);
	command_dispatcher.addCommandHandler(help_command, handlerHelp);
	command_dispatcher.addCommandHandler(fibonacci_command, handlerFibonacci);
	command_dispatcher.addCommandHandler(random_numb_command, handlerRandNum);
	command_dispatcher.addCommandHandler(about_command, handlerAbout);
		// gimme ...
		// command line interface
		string command;
	while (!g_done) {

		cout << "\tenter command : ";
		getline(cin, command);
		command_dispatcher.dispatchCommand(command);
	}

	std::cout << "COMMAND DISPATCHER: ENDED" << std::endl;

	system("pause");

	return 0;
}