#include <iostream>
#include <unordered_set>

int evaluate_command(std::string& command, std::unordered_set<std::string>& built_ins)
{
  if(/*command.starts_with("exit")*//*command.find("exit") == 0*/ command.compare("exit 0") == 0)
    {
      return 0; // Signal to successfully terminating program
    }
    else if(command.find("echo") == 0)
    {
      std::cout<<command.substr(5)<<std::endl;
      return -1; // Signal for successful execution of command and continue to next loop
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

  // Defining the built-in commands
  std::unordered_set<std::string> built_ins={"echo","exit","type"};

  // (REPL Loop)
  while(true)
  {
    // Read command
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);

    // Evaluate command and Print output
    int result = evaluate_command(input, built_ins);
    if(result == 0) return 0;
  }
  return 0;
}
