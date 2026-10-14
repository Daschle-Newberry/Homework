#include <iostream>
#include <optional>
#include <fstream>
#include <stdexcept>

#include "NFA.h"

int main(int argc, char** argv) {
  std::ifstream file;
  std::istream* in = &std::cin;

  if(argc >= 2) {
    file.open(argv[1]);
    
    if(!file) {
      std::cerr << argv[0] << ": cannot open '" << argv[1] << "'\n'";
      return 1;
    }
    
    in = &file;
  }
  std::string line;

  while(std::getline(*in, line)) {
    std::cout<<"RE: " << line << std::endl; 
    try {
      NFA nfa = NFA::fromPostfix(line, "abcde");
      std::cout << nfa << std::endl;
    } catch(const std::invalid_argument& e) { 
      std::cout << e.what() << std::endl;
    }
  }

}
