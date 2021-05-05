//
// Created by tbwsl on 5/1/21.
//

#ifndef INFORF_LOGGINGBACKEND_H
#define INFORF_LOGGINGBACKEND_H
#include <string>

namespace lg {

enum SeverityLevel { normal, notification, warning, error, critical };

class LoggingBackend {
  public:
    LoggingBackend();
    static void disable_logging();
  private:
    const std::string full_logging_path = "/tmp/rfdst_full.log";
    const std::string important_logging_path = "/tmp/rfdst_important.log";
    const std::string timing_logging_path = "/tmp/rfdst_timing.log";
};

std::ostream &operator<<(std::ostream &strm, SeverityLevel level);
} // namespace lg
#endif // INFORF_LOGGINGBACKEND_H
