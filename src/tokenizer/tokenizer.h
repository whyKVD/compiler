#pragma once

#include <iostream>
#include <vector>
#include <optional>
#include <fstream>

using namespace std;

enum class TokenType{
  ident,
		main,
		ret,
		int_lit,
  open_curl,
  close_curl,
  open_p,
  close_p,
		open_ang,
		closed_ang,
		semi,
		dbl_qts,
		qts,
		excl,
		quest,
		hstg
};

struct Token{
  TokenType type;
  optional<string> value;
};

class Tokenizer{
public:
  inline Tokenizer(const string& src) : a_src(move(src)) {}

  inline vector<Token> tokenize(){
    vector<Token> tokens;

    while(peek().has_value()){
      char c = consume();
      string buf;
      if(isalpha(c)){
        buf.push_back(c);
        while(peek().has_value() && isalnum(peek().value())){
          buf.push_back(consume());
        }
								if(buf == "main")
          tokens.push_back(Token{.type = TokenType::main});
								if(buf == "return")
										tokens.push_back({.type = TokenType::ret});
        else
										tokens.push_back({.type = TokenType::ident, .value = buf});
        buf.clear();
								continue;
      }else if(ispunct(c)){
								if(c == '{')
											tokens.push_back({.type = TokenType::open_curl});
								else if(c == '}')
											tokens.push_back({.type = TokenType::close_curl});
								else if(c == '(')
											tokens.push_back({.type = TokenType::open_p});
								else if(c == ')')
											tokens.push_back({.type = TokenType::close_p});
								else if(c == '<')
											tokens.push_back({.type = TokenType::open_ang});
								else if(c == '>')
											tokens.push_back({.type = TokenType::closed_ang});
								else if(c == ';')
											tokens.push_back({.type = TokenType::semi});
								else if(c == '"')
											tokens.push_back({.type = TokenType::dbl_qts});
								else if(c == '\'')
											tokens.push_back({.type = TokenType::qts});
								else if(c == '!')
											tokens.push_back({.type = TokenType::excl});
								else if(c == '?')
											tokens.push_back({.type = TokenType::quest});
								else if(c == '#')
											tokens.push_back({.type = TokenType::hstg});
								continue;
      }else if(isspace(c)){
        continue;
      }else if(isdigit(c)){
        buf.push_back(c);
        while(peek().has_value() && isdigit(peek().value())){
          buf.push_back(consume());
        }
								tokens.push_back({.type = TokenType::int_lit, .value = buf});
								buf.clear();
						}else{
        break;
      }
    }
    
    return tokens;
  }

  void token_to_asm(vector<Token> tokens){
    stringstream output;
    bool main = false;
				string ret_val;
				int curl = 0;
				int paren = 0;
				int ang = 0;
				int quotes = 0;
				int dbl_quotes = 0;

    output << "global _start" << endl;

				for(int i = 0; i < tokens.size(); i++){
       if(tokens.at(i).type == TokenType::main){
									main = true;
         output << "_start:" << endl;
									while(i < tokens.size()){
											switch(tokens.at(i).type){
													case TokenType::ret:
															i++;
															if(tokens.at(i).type == TokenType::int_lit){
																 i++;
																	if(tokens.at(i).type != TokenType::semi){
												       cerr << "expected semicolumn" << endl;
																			exit(EXIT_FAILURE);
										       }
                 output << "  mov rax, 60" << endl;
                 output << "  mov rdi, " << tokens.at(i - 1).value.value() << endl;
                 output << "  syscall" << endl;
															}
															break;
													case TokenType::open_ang:
															ang++;
														 break;
													case TokenType::open_curl:
															curl++;
															break;
													case TokenType::open_p:
															paren++;
															break;
													case TokenType::closed_ang:
															ang--;
															break;
													case TokenType::close_curl:
															curl--;
															break;
													case TokenType::close_p:
															paren--;
															break;
													case TokenType::dbl_qts:
															dbl_quotes++;
													  break;
													case TokenType::qts:
															quotes++;
															break;
													default:break;
											}
											i++;
									}
         output << "  mov rax, 60" << endl;
         output << "  mov rdi, 0" << endl;
         output << "  syscall" << endl;
							}else if(tokens.at(i).type == TokenType::open_curl)
										curl++;
							else if(tokens.at(i).type == TokenType::close_curl)
										curl--;
       else if(tokens.at(i).type == TokenType::open_p)
										paren++;
							else if(tokens.at(i).type == TokenType::close_p)
										paren--;
       else if(tokens.at(i).type == TokenType::open_ang)
										ang++;
							else if(tokens.at(i).type == TokenType::closed_ang)
										ang--;
       else if(tokens.at(i).type == TokenType::qts)
										quotes++;
       else if(tokens.at(i).type == TokenType::dbl_qts)
										dbl_quotes++;
				}

				if(!main){
						cerr << "no main found" << endl;
						exit(EXIT_FAILURE);
				}else if(curl != 0){
							cerr << "curly brackets ";
						 cerr <<	(curl < 0 ? "closed but never opened" : "opened but never close") << endl;
							exit(EXIT_FAILURE);
				}else if(paren != 0){
							cerr << "paren ";
						 cerr <<	(paren < 0 ? "closed but never opened" : "opened but never closed") << endl;
							exit(EXIT_FAILURE);
				}else if(ang != 0){
							cerr << "angular brackets ";
						 cerr <<	(ang < 0 ? "closed but never opened" : "opened but never closed") << endl;
							exit(EXIT_FAILURE);
				}else if(quotes%2 != 0){
							cerr << "missing a quotes \"'\"" << endl;
				}else if(dbl_quotes%2 != 0){
							cerr << "missing a double quotes '\"'" << endl;
				}

    {
      string out = output.str();
      ofstream fOut("out.asm");
      fOut << out;
						fOut.close();
    }
  }
private:

  optional<char> peek(){
				if(a_index > a_src.length() - 1)
						return {};
				else
						return a_src.at(a_index);
		}

		char consume(){
				return a_src.at(a_index++);
		}

  const string a_src;
  int a_index = 0;
};
