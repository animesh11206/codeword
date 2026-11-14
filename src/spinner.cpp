#include "spinner.h"
#include <iostream>
#include <iomanip>

#if defined(_WIN32)
#include <io.h>
#define ISATTY _isatty
#define FILENO _fileno
#else
#include <unistd.h>
#define ISATTY isatty
#define FILENO fileno
#endif

Spinner::Spinner(const std::string& message)
    : message_(message), running_(false), words_processed_(0), is_interactive_(true) {
    is_interactive_ = (ISATTY(FILENO(stdout)) != 0);
}

Spinner::~Spinner() {
    stop();
}

std::string Spinner::format_number(uint64_t n) {
    std::string s = std::to_string(n);
    int insert_pos = static_cast<int>(s.length()) - 3;
    while (insert_pos > 0) {
        s.insert(insert_pos, ",");
        insert_pos -= 3;
    }
    return s;
}

void Spinner::clear_line() {
    if (is_interactive_) {
        std::cout << "\r" << std::string(79, ' ') << "\r" << std::flush;
    }
}

void Spinner::start() {
    start_time_ = std::chrono::steady_clock::now();
    running_ = true;
    if (is_interactive_) {
        worker_ = std::thread(&Spinner::spin, this);
    }
}

void Spinner::increment(uint64_t count) {
    words_processed_.fetch_add(count, std::memory_order_relaxed);
}

uint64_t Spinner::get_count() const {
    return words_processed_.load(std::memory_order_relaxed);
}

double Spinner::get_elapsed_seconds() const {
    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end_time - start_time_;
    return diff.count();
}

void Spinner::stop() {
    if (running_.exchange(false)) {
        if (worker_.joinable()) {
            worker_.join();
        }
        clear_line();
    }
}

void Spinner::finish(const std::string& output_path) {
    stop();
    double elapsed = get_elapsed_seconds();
    std::cout << "[+] Successfully processed " << format_number(words_processed_.load(std::memory_order_relaxed))
              << " words in " << std::fixed << std::setprecision(2) << elapsed << "s.\n";
    std::cout << "[+] Output saved to: " << output_path << std::endl;
}

void Spinner::spin() {
    const char frames[] = {'|', '/', '-', '\\'};
    const size_t num_frames = sizeof(frames) / sizeof(frames[0]);
    size_t frame_idx = 0;

    while (running_) {
        std::string line = std::string("[") + frames[frame_idx] + "] " + message_ + " (" +
                           format_number(words_processed_.load(std::memory_order_relaxed)) + " words)";
        if (line.length() < 79) {
            line.append(79 - line.length(), ' ');
        }
        std::cout << "\r" << line << std::flush;
        frame_idx = (frame_idx + 1) % num_frames;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
