#include <fstream>
#include <iostream>

#include "analyser/analyser.h"
#include "argparse/argparse.hpp"
// #include "fmt/core.h"
// #include "fmts.hpp"
#include "generator/generator.h"
#include "tokenizer/tokenizer.h"

std::vector<miniplc0::Token> _tokenize(std::istream &input) {
  miniplc0::Tokenizer tkz(input);
  auto p = tkz.AllTokens();
  if (p.second.has_value()) {
    fprintf(stderr, "Tokenization error: %s\n",
            p.second.value().GetString().c_str());
    // 由于平台限制，必须返回 0
    exit(2);
  }
  return p.first;
}

void Tokenize(std::istream &input, std::ostream &output) {
  auto v = _tokenize(input);
  for (auto &it : v) output << it.Print() << std::endl;
  return;
}

void Analyse(std::istream &input, std::ostream &output) {
  auto tks = _tokenize(input);
  miniplc0::Analyser analyser(tks);
  try {
    auto p = analyser.Analyse();
    printf("==== Begin Generator! ====\n");
    auto generator = miniplc0::Generator(output);
    generator.generateProgram(p);

    // for (auto &isa : generator.instructions) { 
    //   char *val = isa.ISA2Hex();
    //   output.write(val, isa.GetLen());
    // }
    
  } catch (miniplc0::CompilationError &error) {
    fprintf(stderr, "Line: %3lu Column: %3lu Syntactic analysis error: %s\n",
            error.GetPos().first, error.GetPos().second,
            error.GetString().c_str());
    exit(2);
  }

  // if (p.second.has_value()) {
  //   fmt::print(stderr, "Syntactic analysis error: {}\n", p.second.value());
  //   // 同上
  //   exit(0);
  // }
  // auto v = p.first;
  // for (auto &it : v) output << fmt::format("{}\n", it);
  return;
}

int main(int argc, char **argv) {
  argparse::ArgumentParser program("miniplc0");
  program.add_argument("input").help("speicify the file to be compiled.");
  program.add_argument("-t").default_value(false).implicit_value(true).help(
      "perform tokenization for the input file.");
  program.add_argument("-l").default_value(false).implicit_value(true).help(
      "perform syntactic analysis for the input file.");
  program.add_argument("-o", "--output")
      .required()
      .default_value(std::string("-"))
      .help("specify the output file.");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    fprintf(stderr, "%s\n\n", err.what());
    std::cout << program;
    exit(2);
  }

  auto input_file = program.get<std::string>("input");
  auto output_file = program.get<std::string>("--output");
  std::istream *input;
  std::ostream *output;
  std::ifstream inf;
  std::ofstream outf;
  if (input_file != "-") {
    inf.open(input_file, std::ios::in);
    if (!inf) {
      fprintf(stderr, "Fail to open %s for reading.\n", input_file.c_str());
      exit(2);
    }
    input = &inf;
  } else
    input = &std::cin;
  if (output_file != "-") {
    outf.open(output_file, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!outf) {
      fprintf(stderr, "Fail to open %s for writing.\n", output_file.c_str());
      exit(2);
    }
    output = &outf;
  } else
    output = &std::cout;
  if (program["-t"] == true && program["-l"] == true) {
    fprintf(
        stderr,
        "You can only perform tokenization or syntactic analysis at one time.");
    exit(2);
  }
  if (program["-t"] == true) {
    Tokenize(*input, *output);
  } else if (program["-l"] == true) {
    Analyse(*input, *output);
  } else {
    fprintf(stderr, "You must choose tokenization or syntactic analysis.");
    exit(2);
  }
  return 0;
}
