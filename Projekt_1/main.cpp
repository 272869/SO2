#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <atomic>
#include <vector>

std::mutex print_protect;
std::atomic<bool> running(true);

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dist(1000, 5000);

void report(int id, const std::string& state) {
    std::lock_guard<std::mutex> lock(print_protect);
    std::cout << "Philosopher " << id << " " << state << std::endl;
}

void philosopher(int id, std::vector<std::mutex>& forks, std::vector<int>& score) {
    int left_fork = (id + 1) % forks.size();
    int right_fork = id;

    while (running) {
        int thinking_time = dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(thinking_time));
        report(id, "thinking...");

        forks[left_fork].lock();
        report(id, "took the left fork");

        if (forks[right_fork].try_lock()) {
            report(id, "took the right fork and started eating");

            int eating_time = dist(gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(eating_time));
            score[id]++;
            report(id, "finished eating");

            forks[right_fork].unlock();
            forks[left_fork].unlock();
        } else {
            report(id, "couldn't take the right fork, returns the left one");
            forks[left_fork].unlock();
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_philosophers>" << std::endl;
        return 1;
    }

    int N = std::stoi(argv[1]);
    int run_time;

    std::cout << "Enter the duration of execution in seconds: ";
    std::cin >> run_time;

    std::vector<std::mutex> forks(N);
    std::vector<int> score(N, 0);
    std::vector<std::thread> philosophers;

    for (int i = 0; i < N; i++) {
        philosophers.emplace_back(philosopher, i, std::ref(forks), std::ref(score));
    }

    std::this_thread::sleep_for(std::chrono::seconds(run_time));
    running = false;

    for (auto& philosopher : philosophers) {
        philosopher.join();
    }

    for (int i = 0; i < N; i++) {
        std::cout << "Philosopher " << i << " ate " << score[i] << " times" << std::endl;
    }

    return 0;
}
