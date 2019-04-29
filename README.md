## Table of contents
* [Description](#description)
* [Requirements](#requirements)
* [Setup](#setup)
* [Usage](#usage)

## Description
<p>This project allows to create a simple parsers based on information which are obtained from grammar rules. A defined parser can be used to process a text using lexical and syntax analysis. 
An output from the analysis is a parser tree. The information can be used in a front-end compiler as a tool. The parse tree can be traversed to obtain more information that can be used i.e.
in a semantics analysis.</p>
	
## Requirements
* programming language compiler: C++11
* build utils: Cmake

## Setup
To setup the project, use the followings commands (all dependencies are downloaded by Cmake):
```
$ cd <project_dir>
$ git clone https://github.com/krystianAndrzejewski/parser
$ cd parser
cmake -H. -Bbuild -G <generator_name:"Visual Studio 14 2015 Win64">
```

## Usage
An example of usage the project is defined in main.cpp. Input files are located in ./example. An grammar files has a special syntax:
* %start <nonterminal_name> - takes name of non-terminal that causes a transition to a final state.
* %ignore <pattern> - ignores a text based on the pattern that is defined according to the argument.
* %token <<type>>? <name> <pattern> - takes a pattern and a name and produces a token (terminal) (first argument is type name and is practically unused).
* %type <<type>>? <name> - takes a name and produces a non-terminal (first argument is type name and is practically unused).
* %left <name> - takes a name and produces a left precedence (left-associative operators).
* %right <name> - takes a name and produces a right precedence (right-associative operators).
* %nonassoc <name> - takes a name and produces a nonassoc precedence (the operators in a associative way produces a syntax error).
* <non-terminal-name> : <non-terminal-or-terminal-name>+ ( | <non-terminal-or-terminal-name>+ )* - defines a non-terminal productions based on terminals and non-terminals (recursion is allowed).
* <non-terminal-name> : %empty - defines a empty production for the non-terminal (the rule can be connected with other productions).
* // <comment-text> - defines a comment section.
Errors during processing either a grammar definition or a text to compilation are reported by exception or returned values. 

