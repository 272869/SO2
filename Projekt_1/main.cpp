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
    std::cout << "Filozof " << id << " " << state << std::endl;
}

[[noreturn]] void philosopher(int id) {
    const int left_fork = (id + 1) % N;
    const int right_fork = id;

    while (true) {
        int thinking_time = dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(thinking_time));
        report(id, "mysli");

        forks[left_fork].lock();
        report(id, "zabral lewy widelec");

        if (forks[right_fork].try_lock()) {
            report(id, "zabral prawy widelec i zaczyna jesc");

            int eating_time = dist(gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(eating_time));
            score[id]++;
            report(id, "skonczyl jesc");

            forks[right_fork].unlock();
            forks[left_fork].unlock();
        } else {
            report(id, "nie mogl zabrac prawego widelca, zwraca lewy");
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
        std::cout << "Filozof " << i << " jadl " << score[i] << " razy" << std::endl;
    }

    return 0;
}