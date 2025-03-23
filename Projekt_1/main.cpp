#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

const int N = 5;

std::mutex forks[N];
std::mutex print_protect;
int score[N] = {0};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dist(1000, 5000);

void report(int id, const std::string& state) {
    std::lock_guard<std::mutex> lock(print_protect);
    std::cout << "Philosopher " << id << " " << state << std::endl;
}

[[noreturn]] void philosopher(int id) {
    const int left_fork = (id + 1) % N;
    const int right_fork = id;

    while (true) {
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

int main() {
    std::thread philosophers[N];
    for (int i = 0; i < N; i++) {
        philosophers[i] = std::thread(philosopher, i);
    }

    for (auto & philosopher : philosophers) {
        philosopher.join();
    }

    for (int i = 0; i < N; i++) {
        std::cout << "Philosopher " << i << " ate " << score[i] << " times" << std::endl;
    }

    return 0;
}