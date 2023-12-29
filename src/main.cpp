#include <iostream>
#include <fstream>
#include <sstream>

#include "tokenizer/tokenizer.h"

using namespace std;

int main(int argc, char* argv[]){
  if(argc < 2){
    cerr << "usage: ./a.out <input.file>" << endl;
    return -1;
  }
 
  ifstream fIn(argv[1]);
    
  stringstream source;
  source << fIn.rdbuf();

  Tokenizer t(source.str());

		t.token_to_asm(t.tokenize());

		system("nasm -felf64 out.asm");
		system("ld -o main.test.out out.o");
}
