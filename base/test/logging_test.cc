#include "logging.h"
#include "timer.h"
#include <thread>

using namespace openmi;

static int N = 10;
static int NN = 10000;

const char* msg = "openmi logging";

void default_logging() {
  printf("default_logging. file:%s, line:%d\n", __FILE__, __LINE__);
  LOG(INFO) << msg;
  LOG(WARN) << msg;
  LOG(ERROR) << msg;
}

void init_logging(char** argv) {
  InitLogging(argv);
  g_log_dir = "test_logging";
  g_alsologtostderr = true;
  LOG(INFO) << msg;
  LOG(WARN) << msg;
  LOG(ERROR) << msg;
  ShutdownLogging();
}

void logging_bench() {
  for (int i = 0; i < NN; ++i) {
    LOG(INFO) << msg;
    LOG(WARN) << msg;
    LOG(ERROR) << msg;
  }
}

void multi_thread_logging(char** argv) {
  openmi::Timer timer;
  
  InitLogging(argv);
  g_log_dir = "test_logging";
  g_alsologtostderr = false;

  std::thread t1(logging_bench);
  std::thread t2(logging_bench);
  std::thread t3(logging_bench);
  t1.join();
  t2.join();
  t3.join();

  ShutdownLogging();
  int end_time = timer.Elapsed();
  printf("[%d] time:[%d] speed:%f\n", 3*NN, end_time, 3*NN*1.0 / (end_time*1.0 / 1e6));
}

// custom send method 
internal::LogFile* g_logfile = nullptr;

void send_method(const char* msg, size_t size, LogSeverity severity) {
  g_logfile->Write(msg, size);
}

void custom_send_method(char** argv) {
  InitLogging(argv);
  const char* base_dirname = "test_logging";
  const char* bin_name = "logging_test";
  internal::LogFile log_file(base_dirname, bin_name, 1024*1024*1024);
  g_logfile = &log_file;
  openmi::LogMessage::SetSendMethod(send_method);
  logging_bench();

  ShutdownLogging();
}

int main(int argc, char** argv) {
  /*
  default_logging();
  init_logging(argv);
  multi_thread_logging(argv);
  */
  custom_send_method(argv);
  return 0;
}