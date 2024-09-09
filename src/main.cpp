#include <iostream>
#include <vector>
#include <unordered_set>
#include <sstream>
// #include <cstdlib>
#include <filesystem>

std::vector<std::string> split_command(const std::string &message, const std::string& delim) {
  std::vector<std::string> toks;
  std::stringstream ss = std::stringstream{message};
  std::string line;
  while (getline(ss, line, *delim.begin())) {
    toks.push_back(line);
    ss.ignore(delim.length() - 1);
  }
  return toks;
}

std::string get_path(std::string command)
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

int evaluate_command(std::string& command)
{
  std::vector<std::string> cmd_tokens = split_command(command, " ");
  if(cmd_tokens[0].compare("exit") == 0)
  {
    if(command.compare("exit 0") == 0) return 0; // Signal to successfully terminating program
    return 1; // Unsuccessful termination of program
  }
  else if(cmd_tokens[0].compare("echo") == 0)
  {
    std::cout<<command.substr(5)<<std::endl;
    return -1; // Signal for successful execution of command and continue to next loop
  }
  else if(cmd_tokens[0].compare("type") == 0)
  {
    std::string path = get_path(cmd_tokens[1]);

    if(path.empty())
    {
      std::cout<<cmd_tokens[1]<<": not found"<<std::endl;
    }
    else
    {
      std::cout<<cmd_tokens[1]<<" is "<<path<<std::endl;
    }
    return -1;
  }
  else
  {
    std::cout<<command<<": command not found"<<std::endl;
    return -1;
  }
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
    if(result == 0) return 0;
  }
  return 0;
}
