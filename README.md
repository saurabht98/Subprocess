# Subproc Library

## Design Goals
 - Implementing **Subprocess** to create and execute a process
 - Setting up **input/output/error file descriptors**
 - Setting up **communication** between related processes
 - Implementing **wait(), kill(), waitforgiventime()** subprocesses
 - Error handling

## Supported Platforms
The library supports Linux based Operating System. It has only been tested on Ubuntu

## Integration
To use subprocess library include ***subprocess.h*** header file present at ***tops/sdk/lib/common/subproc***

```cpp
#include "subprocess.h";
```

## Examples
Example for each specific Use Case:

#### Use Case 1

**API**
```cpp
Subprocess object(string command, string option = "", bool start = true)
```
**Description** - Create a Subprocess object and start execution immediately if start is set to true

**Parameters**
```
command - a std::string executable name or absolute/relative path with executable name
option  - a std::string option for the executable (default option = "")
start   - a boolean false value so user has control over when to start execution (default start = true)
```
**Example**
```cpp
std::string command = "ls";
Subprocess subproc(command);
```

#### Use Case 2
**API**
```cpp
Start()
```
**Description** - This API will start the process if start flag is set to false when creating Subprocess object

**Example**
```cpp
std::string command = "ls";
std::string command_option = "";
bool start_execution = false;
Subprocess list_process(command, command_option, start_execution);
list_process.Start();
```

#### Use Case 3
**API**
```cpp
Subprocess object(string command, string option = "", bool start = true)
```
**Description** - Create a Subprocess object and execute command with multiple options

**Parameters**
```
command - a std::string executable name or absolute/relative path with executable name
option  - a std::string option for the executable (default option = "")
start   - a boolean false value so user has control over when to start execution (default start = true)
```
**Example**
```cpp
std::string command = "g++";
std::string option = "-std=c++11 -c *.cc";
Subprocess compile_process(command, option);
```

#### Use Case 4
**API**
```cpp
Subprocess object(string command, string option = "", bool start = true)
```
**Description** - Create a Subprocess object and execute a command with providing the absolute path for the binary

**Parameters**
```
command - a std::string executable name or absolute/relative path with executable name
option  - a std::string option for the executable (default option = "")
start   - a boolean false value so user has control over when to start execution (default start = true)
```
**Example**
```cpp
std::string command = "/local/CelebAI/bin/g++";
std::string option = "-std=c++11 -c *.cc";
Subprocess process(command, option);
```

#### Use Case 5
**API**
```cpp
SubprocessWait()
```
**Description** - This API will wait for subprocess to complete its execution

**Example**
```cpp
std::string command = "ls";
std::string option = "-l";
Subprocess process(comamnd, option);
int exit_code = process.SubprocessWait();
```

#### Use Case 6
**API**
```cpp
SubprocessWaitForGivenTime(int time_duration)
```
**Description** - This API will wait for subprocess for a given time duration in seconds

**Parameters**
```
time_duration - an integer value in seconds to wait for subprocess
```
**Example**
```cpp
std::string command = "ls";
std::string option = "-l";
int wait_time = 3;
Subprocess list_all_process(command, option);
int process_status = list_all_process.SubprocessWaitForGivenTime(wait_time);
```

#### Use Case 7
**API**
```cpp
SubprocessKill()
```
**Description** - This API will kill the Subprocess and stop its execution

**Example**
```cpp
std::string command = "ls";
std::string option = "-l";
Subprocess list_process(command, option);
int exit_code = list_process.SubprocessKill();
```

#### Use Case 8
**API**
```cpp
SubprocessReceiveInputFromFile(string filename)
```
**Description** - This API will provide input to a subprocess from a given file. This is an overloaded function which takes string, integer or FILE* as parameter

**Parameters**
```
filename/fd/FILE*  - a std::string file name or integer fd or FILE pointer
```
**Example**
```cpp
std::string command = "ls";
std::string option = "-l";
std::string input_file = "file_name.txt";
Subprocess process(command, option, false);
process.ReceiveInputFromFile(input_file);
process.Start();
```

#### Use Case 9
**API**
```cpp
SubprocessSendOutputToFile(string filename)
```
**Description** - This API will send output of a subprocess to a given file. This is an overloaded function which takes string, integer or FILE* as an input parameter

**Parameters**
```
filename/fd/FILE*  - a std::string file name or integer fd or FILE pointer
```
**Example**
```cpp
std::string command = "ls";
std::string option = "-l";
std::string output_file = "file_name.log";
subprocess process(command, option, false);
process.SendOutputToFile(output_file);
process.Start();
```

#### Use Case 10
**API**
```cpp
SubprocessSendErrorToFile(string filename)
```
**Description** - This API will send error of a subprocess to a given file. This is an overloaded function which takes string, integer or FILE* as parameter

**Parameters**
```
filename/fd/FILE*  - a std::string file name or integer fd or FILE pointer
```
**Example**
```cpp
std::string command = "sl";
std::string option = "-l";
std::string error_file = "file_name.log";
subprocess process(command, option, false);
process.SendErrorToFile(error_file);
process.Start();
```

#### Use Case 11
**API**
```cpp
Communicate(Subprocess receiver)
```
**Description** - This API will communicate between two Subprocesses

**Parameters**
```
receiver  - a Subprocess object which will take output of first subprocess as an input
```
**Example**
```cpp
std::string first_command = "ls";
std::string first_option = "-l";
std::string second_command = "grep";
std::string second_option = "CMake*";
Subprocess sender_process(first_command, first_option, false);
Subprocess receiver_process(second_command, second_option, false);
sender_process.Communicate(receiver_process);
```

#### Use Case 12
**API**
```cpp
SubprocessSendOutputToFile(nullptr)
SubprocessSendErrorToFile(nullptr)
```
**Description** - These APIs will discard Output/Error of a subprocess

**Parameters**
```
nullptr   - a nullptr to discard output and error of a subprocess
```
**Example**
```cpp
std::string command = "ls";
std::string option = "-l";
Subprocess process(command, option, false);
process.SendOutputToFile(nullptr);
process.SendErrorToFile(nullptr);
process.Start();
```

#### Use Case 13
**API**
```cpp
GetInputFD()
GetOutputFD()
GetErrorFD()
```
**Description** - These APIs will get input/output/error file descriptor of a process

**Example**
```cpp
std::string command = "ls";
std::string option = "-l";
std::string input_file = "input.log";
std::string output_file = "output.log";
std::string error_file = "error.log";
Subprocess process(command, option, false);
process.ReceiveInputFromFile(input_file);
process.SendOutputToFile(output_file);
process.SendErrorToFile(error_file);
process.Start();
int InputFD = process.GetInputFD();
int OutputFD = process.GetOutputFD();
int ErrorFD = process.GetErrorFD();
```

### Enabling Sanitizer Build
For enabling Sanitizer build, we have to add following flag in the cmake configuration step -
> cmake -S ../tops **-DSANITIZE=address** -G Ninja

### Markdown Preview of README.md file
User can use Atom editor for Markdown Preview of README.md file
> Packages -> Markdown Preview -> Toggle Preview

### Including Logging Library
For including logging library, following lines has been added in CMakeLists.txt file -
> add_dependencies(**subprocess** sdk_include2)  

> include_directories(${CMAKE_BINARY_DIR}/sdk/include) \
> include_directories(${CMAKE_BINARY_DIR}/tools/logging)

> target_link_libraries(**subprocess** logging_lib_logging logging_lib_util logging_include) \
> target_link_libraries(**test_subprocess** logging_lib_logging logging_lib_util logging_include) \
> target_link_libraries(**test_subprocess** dtu_config_lib dtu_config_proto)

#### README FOR DOXYGEN COMMENTS

##### Pre-requisites for Doxygen comment
We have added comments in specific format to generate doxygen document. \
Below are the examples for multi-line and single-line doxygen comments:-
```
/*!
 * Doxygen comments should be added in
 * following manner. The /*! or /** mark
 * specifies that this is a doxygen comment.
 */

/// Single Line Doxygen Comment
```
##### Steps to create and view documentation using doxygen

1) On Linux terminal, outside subproc directory, run the below command:-
> doxygen -g config

2) Open config file and set the following flags:-
> PROJECT_NAME = "Subproc Library"

> INPUT = subproc/include/subprocess.h subproc/src/subprocess.cc
  (space must be present in-between to create documentation using different input files)

3) To generate documentation, run the below command:-
> doxygen config

4) To view documentation:-

4.1) If html files cannot be accessed outside Linux terminal

-  **lynx** must be installed and following command is used to view html documentation:
> lynx html/index.html

-  Use up/down/left/right arrow keys to navigate

4.2) If html files can be accessed outside Linux terminal

-  Documentation can be viewed by opening html/index.html in a web browser
