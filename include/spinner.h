#ifndef SPINNER_H
#define SPINNER_H

#include <string>
#include <chrono>
#include <thread>
#include <atomic>
#include <cstdint>

class Spinner {
public:
    explicit Spinner(const std::string& message = "Processing wordlist...");
    ~Spinner();

    void start();
    void increment(uint64_t count = 1);
    void finish(const std::string& output_path);
    void stop();

    uint64_t get_count() const;
    double get_elapsed_seconds() const;

private:
    void spin();
    void clear_line();
    static std::string format_number(uint64_t n);

    std::string message_;
    std::atomic<bool> running_{false};
    std::atomic<uint64_t> words_processed_{0};
    std::chrono::steady_clock::time_point start_time_;
    std::thread worker_;
    bool is_interactive_{true};
};

#endif // SPINNER_H
