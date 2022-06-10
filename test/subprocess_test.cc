/**
 * Copyright 2021-2022 Enflame. All Rights Reserved.
 *
 * @file    subprocess_test.cc
 * @brief   Contains test cases for the implemented 
 *          subprocess library
 *
 * @author  CAI
 * @date    2022-01-24
 * @version V1.0
 * @par     Copyright (c)
 *          Enflame Tech Company.
 * @par     History:
 */

#include "dtu/common/subprocess.h"
EF_DEFINE_MOD_STR_ARR
void PrintStatus(int process_status) {
  EFLOG(DBG) << "process_status: " << process_status;
  if (process_status == 0) {
    EFLOG(DBG) << "Process complete\n";
  } else if (process_status == 1) {
    EFLOG(DBG) << "Process in execution\n";
  } else if (process_status == 2) {
    EFLOG(DBG) << "Process stopped\n";
  } else if (process_status == 3) {
    EFLOG(DBG) << "Process exited or doesn't exist\n";
  } else {
    EFLOG(DBG) << "Error during wait for\n";
  }
}

// TESTCASE 1 corresponding to USECASE 1
void InstantStart() {
  // initialize command as a string
  std::string list_command = "ls";

  // create and execute a subprocess
  Subprocess subproc(list_command);

  // wait for subprocess to complete it's execution
  EFLOG(DBG) << subproc.SubprocessWait();
}

// TESTCASE 2 corresponding to USECASE 2
void DelayStart() {
  std::string list_command = "ls";

  // initialize command option as string
  std::string command_option = "-l";

  // define a boolean false value 
  bool start_execution = false; 

  Subprocess list_process(list_command, command_option, start_execution);

  // call Start API to start execution
  list_process.Start();
  list_process.SubprocessWait();
}

// TESTCASE 3 corresponding to USECASE 3
void CommandWithMultipleOptions() {
  std::string grep_command = "grep";

  // initialize string with multiple command option
  std::string command_option = "-rnw CMake";

  Subprocess grep_process(grep_command, command_option);
  grep_process.SubprocessWait();
}

// TESTCASE 4 corresponding to USECASE 4
void AbsolutePathTest() {
  std::string command = "g++";
  std::string option = "try.cpp -o main";
  bool start_execution = false;

  Subprocess create_executable(command, option, start_execution);
  create_executable.Start();
  create_executable.SubprocessWait();
  
  command = "./main";
  option = "";

  Subprocess run_executable(command, option, start_execution);
  run_executable.Start();
  run_executable.SubprocessWait();
}

// TESTCASE 5 corresponding to USECASE 4
void AbsolutePathWithOptionTest() {
  std::string command = "/bin/ls";
  std::string option = "-l";
  bool start_execution = false;

  Subprocess process(command, option, start_execution);
  process.Start();
  process.SubprocessWait();
}

// TESTCASE 6 corresponding to USECASE 5
void SubprocessWaitTest() {
  std::string list_command = "ls";
  std::string command_option = "-l";
  Subprocess list_process(list_command, command_option);

  // get wait exit code
  int exit_code = list_process.SubprocessWait();
  if(exit_code != -1) {
    EFLOG(DBG) << "Process completed";
  } else {
    EFLOG(DBG) << "Error during Process completion";
  }
}

// TESTCASE 7 corresponding to USECASE 6
void SuccessfulWaitForProcess() {
  std::string command = "ls";
  std::string option = "-l";
  bool start_execution = false;
  std::string output_file = "success_wait_for_output.log";
  int time_duration = 2;

  Subprocess process(command, option, start_execution);
  process.SendOutputToFile(output_file);
  process.Start();

  int process_status = process.SubprocessWaitForGivenTime(time_duration);
  PrintStatus(process_status);
}

// TESTCASE 8 corresponding to USECASE 6
void InExecutionWaitForProcess() {
  // ./loop takes 4 seconds to complete its execution
  std::string command = "./loop";
  std::string option = "";
  bool start_execution = false;
  std::string output_file = "in_execution_wait_for_output.log";
  int time_duration = 2;

  Subprocess process(command, option, start_execution);
  process.SendOutputToFile(output_file);
  process.Start();

  int process_status = process.SubprocessWaitForGivenTime(time_duration);
  PrintStatus(process_status);
}

// TESTCASE 9 corresponding to USECASE 6
void FailedWaitForProcess() {
  std::string command = "sl";
  std::string option = "-l";
  bool start_execution = false;
  std::string output_file = "failed_wait_for_output.log";
  int time_duration = 3;

  Subprocess process(command, option, start_execution);
  process.SendOutputToFile(output_file);
  process.Start();

  int process_status = process.SubprocessWaitForGivenTime(time_duration);
  PrintStatus(process_status);
}

// TESTCASE 10 corresponding to USECASE 6
void WaitForAfterWait() {
  std::string command = "ls";
  std::string option = "-l";
  bool start_execution = false;
  std::string output_file = "after_wait_wait_for_output.log";
  int time_duration = 3;

  Subprocess process(command, option, start_execution);
  process.SendOutputToFile(output_file);
  process.Start();
  process.SubprocessWait();

  int process_status = process.SubprocessWaitForGivenTime(time_duration);
  PrintStatus(process_status);
}

// TESTCASE 11 corresponding to USECASE 6
void NegativeWaitFor() {
  std::string command = "ls";
  std::string option = "-l";
  bool start_execution = false;
  std::string output_file = "negative_wait_for_output.log";
  int time_duration = -2;

  Subprocess process(command, option, start_execution);
  process.SendOutputToFile(output_file);
  process.Start();

  int process_status = process.SubprocessWaitForGivenTime(time_duration);
  PrintStatus(process_status);
}

// TESTCASE 12 corresponding to USECASE 6
void ZeroWaitFor() {
  std::string command = "ls";
  std::string option = "-l";
  bool start_execution = false;
  std::string output_file = "zero_wait_for_output.log";
  int time_duration = 0;

  Subprocess process(command, option, start_execution);
  process.SendOutputToFile(output_file);
  process.Start();

  int process_status = process.SubprocessWaitForGivenTime(time_duration);
  PrintStatus(process_status);
}

// TESTCASE 13 corresponding to USECASE 6
void WaitForAfterKill() {
  std::string command = "ls";
  std::string option = "-l";
  bool start_execution = false;
  std::string output_file = "kill_wait_for_output.log";
  int time_duration = 2;

  Subprocess process(command, option, start_execution);
  process.SendOutputToFile(output_file);
  process.Start();

  process.SubprocessKill();
  int process_status = process.SubprocessWaitForGivenTime(time_duration);
  PrintStatus(process_status);
}

// TESTCASE 14 corresponding to USECASE 7
void KillProcess() {
  std::string command = "ls";

  Subprocess list_process(command);

  int kill_code = list_process.SubprocessKill();
  if (kill_code == 0) {
    EFLOG(DBG) << "Process successfully terminated\n";
  } else {
    EFLOG(DBG) << "Error while termination\n";
  }
}

// TESTCASE 15 corresponding to USECASE 8
void TransferInputThroughFile() {
  std::string grep_command = "grep";
  std::string command_option = "variable";
  bool start_execution = false;

  // initialize input filename as string
  // input file must exist
  std::string input_file = "CMakeCache.txt";
  Subprocess input_process(grep_command, command_option, start_execution);

  // set input fd
  input_process.ReceiveInputFromFile(input_file);
  input_process.Start();
  input_process.SubprocessWait();
}

// TESTCASE 16 corresponding to USECASE 9
void StoreOutputInFile() {
  std::string list_command = "ls";
  std::string command_option = "-l";
  bool start_execution = false;

  // initialize output filename as string
  // if file doesn't exist, new file is created and output is written in it
  // if file is present, output is appended in it
  std::string output_file = "list_output.log";
  Subprocess list_process(list_command, command_option, start_execution);

  // set output fd
  list_process.SendOutputToFile(output_file);
  list_process.Start();
  list_process.SubprocessWait();
}

// TESTCASE 17 corresponding to USECASE 10
void StoreErrorInFile() {
  std::string list_command = "sl";
  std::string command_option = "-l";
  bool start_execution = false;

  // initialize error filename as string
  // if file doesn't exist, new file is created and error is written in it
  // if file is present, error is appended in it
  std::string error_file = "list_error.log";
  Subprocess list_process(list_command, command_option, start_execution);

  // set error fd
  list_process.SendErrorToFile(error_file);
  list_process.Start();
  list_process.SubprocessWait();
}

// TESTCASE 18 corresponding to USECASE 11
void CommunicateTest(){
  std::string command = "ls";
  std::string command_option = "-l";
  bool start_execution = false;
  Subprocess first_process(command, command_option, start_execution);

  command = "grep";
  command_option = "CMake";
  Subprocess second_process(command, command_option, start_execution);

  // output of first process is taken as input to second process
  first_process.Communicate(second_process);
}

// TESTCASE 19 corresponding to USECASE 12
void DiscardingOutput() {
  std::string command = "ls";
  std::string option = "-l";
  bool start_execution = false;

  Subprocess process(command, option, start_execution);
  process.SendOutputToFile(nullptr);
  process.Start();
  process.SubprocessWait();
}

// TESTCASE 20 corresponding to USECASE 12
void DiscardingError() {
  std::string command = "sl";
  std::string option = "-l";
  bool start_execution = false;

  Subprocess process(command, option, start_execution);
  process.SendErrorToFile(nullptr);
  process.Start();
  process.SubprocessWait();
}

// TESTCASE 21 corresponding to USECASE 13
void GetInputOutputErrorFDs() {
  std::string grep_command = "grep";
  std::string command_option = "CMake*";
  bool start_execution = false;
  std::string input_file = "list_output.log";
  std::string output_file = "grep_output.log";
  std::string error_file = "grep_error.log";

  Subprocess grep_process(grep_command, command_option, start_execution);
  grep_process.ReceiveInputFromFile(input_file);
  grep_process.SendOutputToFile(output_file);
  grep_process.SendErrorToFile(error_file);
  grep_process.Start();

  // get input/output/error FDs
  EFLOG(DBG) << "InputFD:" << grep_process.GetInputFD();
  EFLOG(DBG) << "OutputFD:" << grep_process.GetOutputFD();
  EFLOG(DBG) << "ErrorFD:" << grep_process.GetErrorFD();
  grep_process.SubprocessWait();
}

// TESTCASE 22
void ExecuteTwoProcesses(){
  std::string command = "ls";
  std::string command_option = "-l";
  bool start_execution = false;
  std::string first_output_file = "first_subprocess_output.log";

  Subprocess first_process(command, command_option, start_execution);
  first_process.SendOutputToFile(first_output_file);
  first_process.Start();
  first_process.SubprocessWait();

  command = "grep";
  command_option = "CMake";
  start_execution = false;
  std::string second_output_file = "second_subprocess_output.log";

  Subprocess second_process(command, command_option, start_execution);
  second_process.ReceiveInputFromFile(first_process.GetOutputFD());
  second_process.SendOutputToFile(second_output_file);
  second_process.Start();
  second_process.SubprocessWait(); 
}

// TESTCASE 23
void WaitAfterSleep() {
  std::string command = "ls";
  std::string command_option = "-l";
  bool start_execution = false;
  std::string output_file = "wait_after_sleep_output.log";

  Subprocess list_process(command, command_option, start_execution);
  list_process.SendOutputToFile(output_file);
  list_process.Start();

  // time in seconds to sleep
  int sleep_time_in_sec = 3;
  sleep(sleep_time_in_sec);
  list_process.SubprocessWait();
}

int main() {
  clock_t clk_start, clk_end;
  clk_start = clock();

  EFLOG(DBG) << "\nTEST 1 InstantStart:\n";
  InstantStart();

  EFLOG(DBG) << "\nTEST 2: DelayStart\n";
  DelayStart();

  EFLOG(DBG) << "\nTEST 3: CommandWithMultipleOptions\n";
  CommandWithMultipleOptions();

  EFLOG(DBG) << "\nTEST 4: AbsolutePathTest\n";
  AbsolutePathTest();

  EFLOG(DBG) << "\nTEST 5: AbsolutePathWithOptionTest\n";
  AbsolutePathWithOptionTest();

  EFLOG(DBG) << "\nTEST 6: SubprocessWaitTest\n";
  SubprocessWaitTest();

  EFLOG(DBG) << "\nTEST 7: SuccessfulWaitForProcess\n";
  SuccessfulWaitForProcess();

  EFLOG(DBG) << "\nTEST 8: InExecutionWaitForProcess\n";
  InExecutionWaitForProcess();

  EFLOG(DBG) << "\nTEST 9: FailedWaitForProcess\n";
  FailedWaitForProcess();

  EFLOG(DBG) << "\nTEST 10: WaitForAfterWait\n";
  WaitForAfterWait();

  EFLOG(DBG) << "\nTEST 11: NegativeWaitFor\n";
  NegativeWaitFor();

  EFLOG(DBG) << "\nTEST 12: ZeroWaitFor\n";
  ZeroWaitFor();

  EFLOG(DBG) << "\nTEST 13: WaitForAfterKill\n";
  WaitForAfterKill();

  EFLOG(DBG) << "\nTEST 14: KillProcess\n";
  KillProcess();

  EFLOG(DBG) << "\nTEST 15: TransferInputThroughFile\n";
  TransferInputThroughFile();

  EFLOG(DBG) << "\nTEST 16: StoreOutputInFile\n";
  StoreOutputInFile();

  EFLOG(DBG) << "\nTEST 17: StoreErrorInFile\n";
  StoreErrorInFile();

  EFLOG(DBG) << "\nTEST 18: CommunicateTest\n";
  CommunicateTest();

  EFLOG(DBG) << "\nTEST 19: Discarding Output\n";
  DiscardingOutput();

  EFLOG(DBG) << "\nTEST 20: Discarding Error\n";
  DiscardingError();

  EFLOG(DBG) << "\nTEST 21: GetInputOutputErrorFDs\n";
  GetInputOutputErrorFDs();

  EFLOG(DBG) << "\nTEST 22: ExecuteTwoProcesses\n";
  ExecuteTwoProcesses();

  EFLOG(DBG) << "\nTEST 23: WaitAfterSleep\n";
  WaitAfterSleep();

  clk_end = clock();
  EFLOG(DBG) << "Total time " << (clk_end - clk_start);

  return 0;
}

