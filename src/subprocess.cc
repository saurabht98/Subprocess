/**
 * Copyright 2021-2022 Enflame. All Rights Reserved.
 *
 * @file    subprocess.cc
 * @brief   Implementation subprocess library
 *          This library helps to create a child process and 
 *          executes the command provided by the user in the child process
 *          
 * @author  CAI
 * @date    2022-01-24
 * @version V1.0
 * @par     Copyright (c)
 *          Enflame Tech Company.
 * @par     History:
 */

#include "dtu/common/subprocess.h"

#define SPACE " "
#define READ_WRITE_PERMISSION 0640
#define NOT_EXIST -1
#define FD_READ_END 0
#define FD_WRITE_END 1
#define FILE_NOT_EXIST nullptr
#define DEV_NULL "/dev/null"
#define ERROR -1
#define SUCCESS 0
#define SIGNAL 0

enum ExitCodes {
  kError = -1,
  kSuccess,
  kInExecution,
  kStopped,
  kChildNotExist
};

Subprocess::Subprocess(std::string command, std::string option, bool start) {

  // parse the command passed by user
  Subprocess::InitializeCommand(command, option);

  if (start) Subprocess::CreateChildAndExecute();
}

void Subprocess::Start() {
  Subprocess::CreateChildAndExecute();
}

void Subprocess::InitializeCommand(std::string cmd, std::string option) {
  // set path variable
  if (cmd.find("/") != std::string::npos)
    path_ = true;

  // split and store cmd and option into vector_of_strings
  v_command_.push_back(cmd);
  bool condition = true;
  int start_index = 0, next_index = 1;
  if (!option.empty()) {
    while (condition) {
      int space_index = option.find(SPACE);

      if (space_index != NOT_EXIST) {
        std::string temp_str = option.substr(start_index, space_index);
        option = option.substr(space_index + next_index);

      if (temp_str.length())
        v_command_.push_back(temp_str);
      } else {
        if (cmd.length())
          v_command_.push_back(option);

        break;
      }
    }
  }
  Subprocess::ConvertToChar();
}

void Subprocess::ConvertToChar() {
  // convert vector_of_string to vector_of_char*
  for (auto it = v_command_.begin(); it != v_command_.end(); it++) {
     ch_command_.push_back(const_cast<char*>((*it).c_str()));
  }
  ch_command_.push_back(nullptr);
}

void Subprocess::CreateChildAndExecute() {
  /*
   * fork() - creates a new process
   * The value returned by fork() corresponds to:
   * 0: if it is the child process (the process created).
   * Positive value: if it is the parent process.
   * -1: if an error occurred.
   * change fork to vfork, otherwise it will conflict with
   * libopenblasp-r0-085ca80a.3.9.so in scipy @ronghua.zhou
   */
  int is_child_process = 0;
  int pid = vfork();

  if (pid == ERROR) {
    EFDLOG(SUBPROC) << "Child creation Failed:\n" << strerror(errno);
  } else if (pid == is_child_process) {
    // Closing Parent FDs in Child
    if (input_fd_[FD_WRITE_END] != NOT_EXIST &&
      close(input_fd_[FD_WRITE_END]) == ERROR) {
      EFDLOG(SUBPROC) << "Error closing input FD in Child Process:\n"
                      << strerror(errno);
    }

    if (output_fd_[FD_READ_END] != NOT_EXIST &&
        close(output_fd_[FD_READ_END]) == ERROR) {
      EFDLOG(SUBPROC) << "Error closing output FD in Child Process:\n"
                      << strerror(errno);
    }

    if (error_fd_[FD_READ_END] != NOT_EXIST &&
        close(error_fd_[FD_READ_END]) == ERROR) {
      EFDLOG(SUBPROC) << "Error Closing error FD in Child Process:\n"
                      << strerror(errno);
    }

    child_pid_ = getpid();
    ExecuteProcess();
  } else if (pid > is_child_process) {
    child_pid_ = pid;

    // Closing Child FDs in Parent
    if (input_fd_[FD_READ_END] != NOT_EXIST &&
        close(input_fd_[FD_READ_END]) == ERROR) {
      EFDLOG(SUBPROC) << "Error closing input FD in Parent Process:\n"
                      << strerror(errno);
    }

    if (output_fd_[FD_WRITE_END] != NOT_EXIST &&
        close(output_fd_[FD_WRITE_END]) == ERROR) {
      EFDLOG(SUBPROC) << "Error closing output FD in Parent Process:\n"
                      << strerror(errno);
    }

    if (error_fd_[FD_WRITE_END] != NOT_EXIST &&
        close(error_fd_[FD_WRITE_END]) == ERROR) {
      EFDLOG(SUBPROC) << "Error closing error FD in Parent Process:\n"
                      << strerror(errno);
    }
  }
}

void Subprocess::ExecuteProcess() {
  // Set Subprocess FDs as stdin, stdout and stderr
  if (input_fd_[FD_READ_END] != NOT_EXIST) {
    if (dup2(input_fd_[FD_READ_END], STDIN_FILENO) == ERROR) {
      EFDLOG(SUBPROC) << "Error during dup2 input FD:\n" << strerror(errno);
    } else if (close(input_fd_[FD_READ_END]) == ERROR) {
      EFDLOG(SUBPROC) << "Error during closing dup2 input FD:\n"
                      << strerror(errno);
    }
  }

  if (output_fd_[FD_WRITE_END] != NOT_EXIST) {
    if (dup2(output_fd_[FD_WRITE_END], STDOUT_FILENO) == ERROR) {
      EFDLOG(SUBPROC) << "Error during dup2 output FD:\n" << strerror(errno);
    } else if (close(output_fd_[FD_WRITE_END]) == ERROR) {
      EFDLOG(SUBPROC) << "Error during closing dup2 output FD:\n"
                      << strerror(errno);
    }
  }

  if (error_fd_[FD_WRITE_END] != NOT_EXIST) {
    if (dup2(error_fd_[FD_WRITE_END], STDERR_FILENO) == ERROR) {
      EFDLOG(SUBPROC) << "Error during dup2 error FD:\n" << strerror(errno);
    } else if (close(error_fd_[FD_WRITE_END]) == ERROR) {
      EFDLOG(SUBPROC) << "Error during closing dup2 error FD:\n"
                      << strerror(errno);
    }
  }

  /*
   * execvp() - replaces the current process image with a new process image
   * arguments - The initial argument is the name of a file that is to be
   *             executed.
   * Return -1, only when an error has occured 
   */
  int executable_index = 0;
  if(!path_ &&
     execvp(ch_command_[executable_index], ch_command_.data()) == ERROR) {
    EFDLOG(SUBPROC) << "Error during execvp():\n" << strerror(errno);
    _exit(EXIT_FAILURE);
  } else if(execv(ch_command_[executable_index], ch_command_.data()) == ERROR) {
    EFDLOG(SUBPROC) << "Error during execvp():\n" << strerror(errno);
    _exit(EXIT_FAILURE);
  }
}


int Subprocess::SubprocessWait() {
  int process_status;
  pid_t pid;

  // combination of WNOHANG, WUNTRACED, WCONTINUED 
  int flag = 0;

  if (kill(child_pid_, SIGNAL) == NOT_EXIST) {
    return kError;
  }

  /* 
   * waitpid() - wait for process to change state
   * Returns
   *  -1 - when error occurs
   *   0 - when WNOHANG is set, and no child process has changed its state
   *   process-pid of child whose state has changes - when successful
   */
  pid = waitpid(child_pid_, &process_status, flag);
  if (pid == ERROR) {
    EFCHECK(false) << "Error during waitpid():\n" << strerror(errno);
  } else {
    if (WIFEXITED(process_status)) {
      EFDLOG(SUBPROC) << "The process ended with status: "
                      << WEXITSTATUS(process_status);
      return WEXITSTATUS(process_status);
    }
    if (WIFSIGNALED(process_status)) {
      EFDLOG(SUBPROC) << "The process ended with kill: "
                      << WTERMSIG(process_status);
      return WTERMSIG(process_status);
    }
  }

  return pid;
}

// Input Channel
void Subprocess::ReceiveInputFromFile(std::string filename) {
  int fd_read = open(filename.c_str(), O_RDONLY);
  if (fd_read == ERROR) {
    EFDLOG(SUBPROC) << "Error during open() on Input:\n" << strerror(errno);
  } else {
    input_fd_[FD_READ_END] = fd_read;
  }
}

void Subprocess::ReceiveInputFromFile(FILE* fp) {
  int fd = fileno(fp);
  if (fd == ERROR) {
    EFDLOG(SUBPROC) << "Error during fileno() on Input:\n" << strerror(errno);
  } else {
    input_fd_[FD_READ_END] = fd;
  }
}

void Subprocess::ReceiveInputFromFile(int fd) {
  input_fd_[FD_READ_END] = fd;
}

// Output Channel
void Subprocess::SendOutputToFile(std::string filename) {
  int fd_write = open(filename.c_str(),
                      O_APPEND | O_CREAT | O_WRONLY, READ_WRITE_PERMISSION);
  if (fd_write == ERROR) {
    EFDLOG(SUBPROC) << "Error during open() on Output:\n" << strerror(errno);
  } else {
    output_fd_[FD_WRITE_END] = fd_write;
  }
  int fd_read = open(filename.c_str(), O_RDONLY);
  if (fd_read == ERROR) {
    EFDLOG(SUBPROC) << "Error during open() on Output:\n" << strerror(errno);
  } else {
    output_fd_[FD_READ_END] = fd_read;
  }
}

void Subprocess::SendOutputToFile(FILE* fp) {
  if (fp) {
    int fd = fileno(fp);
    if (fd == ERROR) {
      EFDLOG(SUBPROC) << "Error during fileno() on Output:\n"
                      << strerror(errno);
    } else {
      output_fd_[FD_WRITE_END] = fd;
    }
  } else {
    int fd_null =  open(DEV_NULL, O_WRONLY);
    if (fd_null == ERROR) {
      EFDLOG(SUBPROC) << "Error during open() on /dev/null for Output:\n"
                      << strerror(errno);
    } else {
      output_fd_[FD_WRITE_END] = fd_null;
    }
  }
}

void Subprocess::SendOutputToFile(int fd) {
  output_fd_[FD_WRITE_END] = fd;
}

// Error Channel
void Subprocess::SendErrorToFile(std::string filename) {
  int fd_write = open(filename.c_str(),
                O_APPEND | O_CREAT | O_WRONLY, READ_WRITE_PERMISSION);
  if (fd_write == ERROR) {
    EFDLOG(SUBPROC) << "Error during open() on Error:\n" << strerror(errno);
  } else {
    error_fd_[FD_WRITE_END] = fd_write;
  }
  int fd_read = open(filename.c_str(), O_RDONLY);
  if (fd_read == ERROR) {
    EFDLOG(SUBPROC) << "Error during open() on Error:\n" << strerror(errno);
  } else {
    error_fd_[FD_READ_END] = fd_read;
  }
}

void Subprocess::SendErrorToFile(FILE* fp) {
  if (fp) {
    int fd = fileno(fp);
    if (fd == ERROR) {
      EFDLOG(SUBPROC) << "Error during fileno() on Error:\n" << strerror(errno);
    } else {
      error_fd_[FD_WRITE_END] = fd;
    }
  } else {
    int fd_null = open(DEV_NULL, O_WRONLY);
    if (fd_null == ERROR) {
      EFDLOG(SUBPROC) << "Error during open() on /dev/null for Error:\n"
                      << strerror(errno);
    } else {
      error_fd_[FD_WRITE_END] = fd_null;
    }
  }
}

void Subprocess::SendErrorToFile(int fd) {
  error_fd_[FD_WRITE_END] = fd;
}

int Subprocess::GetInputFD() {
  return input_fd_[FD_READ_END];
}

int Subprocess::GetOutputFD() {
  return output_fd_[FD_READ_END];
}

int Subprocess::GetErrorFD() {
  return error_fd_[FD_READ_END];
}

void Subprocess::Communicate(Subprocess receiver) {
  int fd[FD_SIZE];
  int ret = pipe(fd);

  if (ret == ERROR) {
    EFDLOG(SUBPROC) << "PIPE creation failed:\n" << strerror(errno);
  } else {
    output_fd_[FD_WRITE_END] = fd[FD_WRITE_END];
    output_fd_[FD_READ_END] = fd[FD_READ_END];

    Subprocess::CreateChildAndExecute();

    receiver.ReceiveInputFromFile(fd[FD_READ_END]);
    receiver.Start();
    receiver.SubprocessWait();
  }
}

int Subprocess::SubprocessKill() {
  int termination_code = ::kill(child_pid_, SIGKILL);
  if(termination_code == ERROR) {
    EFDLOG(SUBPROC) << "Invalid kill:\n" << strerror(errno);
  }

  return termination_code;
}

int Subprocess::SubprocessWaitForGivenTime(int time_duration) {
  siginfo_t signal_info;
  pid_t wait_pid;
  auto current_time = std::chrono::steady_clock::now();
  auto final_time = current_time + std::chrono::seconds(time_duration);
  if(final_time < current_time){
    return kError;
  }
  int sleep_duration = 100000;
  if (kill(child_pid_, SIGNAL) == NOT_EXIST) {
    return kChildNotExist;
  }

  while (final_time > std::chrono::steady_clock::now()) {
    wait_pid = waitid(P_PID, child_pid_,
                      &signal_info, WEXITED | WSTOPPED | WNOHANG);

    if (signal_info.si_code == CLD_KILLED) {
      return kStopped;
    }

    if (signal_info.si_code == CLD_EXITED) {
      if (wait_pid == SUCCESS && signal_info.si_status == SUCCESS) {
        return kSuccess;
      }
      return kError;
    }
    usleep(sleep_duration);
  }
  return kInExecution;
}
