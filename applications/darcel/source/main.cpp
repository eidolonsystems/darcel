#include <fstream>
#include <tclap/CmdLine.h>
#include "darcel/lexicon/token_parser.hpp"
#include "darcel/reactors/reactor_executor.hpp"
#include "darcel/reactors/reactor_translator.hpp"
#include "darcel/syntax/syntax_parser.hpp"
#include "darcel/version.hpp"

using namespace darcel;
using namespace std;
using namespace TCLAP;

int main(int argc, const char** argv) {
  string sourcePath;
  try {
    CmdLine cmd("", ' ', "1.0-r" DARCEL_VERSION
      "\nCopyright (C) 2018 Eidolon Systems Ltd.");
    ValueArg<string> sourceArg("s", "source", "Source file", true, "", "path");
    cmd.add(sourceArg);
    cmd.parse(argc, argv);
    sourcePath = sourceArg.getValue();
  } catch(const ArgException& e) {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
    return -1;
  }
  ifstream sourceFile(sourcePath);
  if(!sourceFile.good()) {
    cerr << sourcePath << " not found." << endl;
    return -1;
  }
  string contents((std::istreambuf_iterator<char>(sourceFile)),
    std::istreambuf_iterator<char>());
  token_parser tp;
  tp.feed(contents.c_str(), contents.size());
  syntax_parser sp;
  while(auto t = tp.parse_token())  {
    sp.feed(*t);
  }
  reactor_translator rt;
  trigger t;
  try {
    while(auto s = sp.parse_node()) {
      rt.translate(*s, t);
    }
  } catch(const syntax_error& e) {
    std::cerr << e.get_location().get_line_number() << ":" <<
      e.get_location().get_column_number() << " - " <<
      e.what() << std::endl;
    return -1;
  }
  auto main_reactor = rt.get_main();
  if(main_reactor == nullptr) {
    cerr << "Main reactor undefined." << endl;
    return -1;
  }
  reactor_executor executor(main_reactor, t);
  executor.execute();
}
