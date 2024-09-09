#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <filesystem>

enum CommandType
{
  Builtin,
  Executable,
  Nonexistent
};

struct FullCommandType
{
  CommandType type;
  std::string executable_path;
};

std::vector<std::string> parse_command_to_string_vector(const std::string &message, const std::string& delim);
FullCommandType command_to_full_command_type(std::string command);
std::string find_command_executable_path(std::string command);

int evaluate_command(std::string& command)
{
  std::vector<std::string> command_vector = parse_command_to_string_vector(command, " ");
  if(command_vector.size() == 0) return -1; // -1 is flag for continue

  FullCommandType fct = command_to_full_command_type(command_vector[0]);

  // Handle builtin commands
  if(fct.type == CommandType::Builtin)
  {
    if(command_vector[0].compare("exit") == 0)
    {
      if(command.compare("exit 0") == 0) return 0; // Signal to successfully terminate program
      else if(command_vector.size() == 1) return 0; // Signal to successfully terminate program even if no argument is passed
      return stoi(command_vector[1]); // Unsuccessful termination of program
    }
    else if(command_vector[0].compare("echo") == 0)
    {
      std::cout<<command.substr(5)<<std::endl;
      return -1; // Signal for successful execution of command and continue to next loop
    }
    else if(command_vector[0].compare("type") == 0)
    {
      if(command_vector.size() < 2) return -1;  // Continue as no argument is provided to the type command
      std::string command_name = command_vector[1];
      FullCommandType command_type = command_to_full_command_type(command_name);

      switch(command_type.type)
      {
        case Builtin:
          std::cout << command_name << " is a shell builtin\n";
          break;
        case Executable:
          std::cout << command_name << " is " << command_type.executable_path << "\n";
          break;
        case Nonexistent:
          std::cout << command_name << " not found\n";
          break;
        default:
          break;
      }
      return -1;  // Continue to the next loop
    }
    else if(command_vector[0].compare("pwd")==0)
    {
      std::cout<<std::filesystem::current_path()<<std::endl;
      return -1;
    }
    else if(command_vector[0].compare("cd")==0)
    {
      if(command_vector.size()==1) return -1;   // Skipping for now
      else if(command_vector[1][0] == '~')
      {
        std::string home = std::getenv("HOME");
        std::string path = home + command_vector[1].substr(1);
        if(std::filesystem::exists(path))
        {
          std::filesystem::current_path(path);
        }
        else 
        {
          std::cout<<"cd: "<<command_vector[1]<<": No such file or directory"<<std::endl;
        }
      }
      else if(std::filesystem::exists(command_vector[1]))
      {
        std::filesystem::current_path(command_vector[1]);
      }
      else 
      {
        std::cout<<"cd: "<<command_vector[1]<<": No such file or directory"<<std::endl;
      }
      return -1;
    }
    return -1;  // Continue to next loop if the built in command is specified in builtin vector but is yet not implemented
  }
  else if(fct.type == CommandType::Executable)
  {
    std::string command_with_full_path = fct.executable_path;

    for(int argn = 1; argn < command_vector.size(); argn++)
    {
      command_with_full_path += " ";
      command_with_full_path += command_vector[argn];
    }

    const char* command_ptr = command_with_full_path.c_str();
    system(command_ptr);
    return -1;  // Continue to next loop after running the program
  }
  else std::cout<<command<<": command not found\n";
  return -1;
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // (REPL Loop)
  while(true)
  {
    // Read command
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);

    // Evaluate command and Print output
    int result = evaluate_command(input);
    if(result == -1) continue;
    else return result;
  }
  return 0;
}

std::vector<std::string> parse_command_to_string_vector(const std::string &message, const std::string& delim) {
  std::vector<std::string> toks;
  std::stringstream ss = std::stringstream{message};
  std::string token;
  while (getline(ss, token, *delim.begin())) {
    if(!token.empty())
    {
      toks.push_back(token);
    }
    ss.ignore(delim.length() - 1);
  }
  return toks;
}

std::string find_command_executable_path(std::string command)
{
  std::string path_env = std::getenv("PATH");
  std::stringstream ss(path_env);
  std::string path;

  while(!ss.eof())
  {
    getline(ss, path, ':');
    std::string abs_path = path + "/" + command;

    if(std::filesystem::exists(abs_path))
    {
      return abs_path;
    }
  }
  return "";
}


FullCommandType command_to_full_command_type(std::string command)
{
  std::vector<std::string> builtin_commands = {"exit","echo","type","pwd","cd"};

  // handle builtin commands
  if(std::find(builtin_commands.begin(), builtin_commands.end(), command) != builtin_commands.end())
  {
    FullCommandType fct;
    fct.type = CommandType::Builtin;
    return fct;
  }

  // Check if command is found in path
  std::string exec_path = find_command_executable_path(command);
  if(!exec_path.empty())
  {
    FullCommandType fct;
    fct.type = CommandType::Executable;
    fct.executable_path = exec_path;
    return fct;
  }

  // Handling non-existent commands
  FullCommandType fct;
  fct.type = CommandType::Nonexistent;
  return fct;
}
