#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <atomic>
#include <vector>

// Mutex chroniący wypisywanie na ekran
std::mutex print_protect;
std::atomic<bool> running(true);

// Generator liczb losowych do symulacji czasu jedzenia i myślenia
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dist(1000, 5000);

// Funkcja wypisująca stan filozofa na ekran
void report(int id, const std::string& state) {
    std::lock_guard<std::mutex> lock(print_protect);
    std::cout << "Philosopher " << id << " " << state << std::endl;
}

// Funkcja reprezentująca zachowanie filozofa
void philosopher(int id, std::vector<std::mutex>& forks, std::vector<int>& score) {
    int left_fork = (id + 1) % forks.size(); // Identyfikator lewego widelca
    int right_fork = id; // Identyfikator prawego widelca

    while (running) {
        // Filozof myśli przez losowy czas
        int thinking_time = dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(thinking_time));
        report(id, "thinking...");

        // Próba podniesienia lewego widelca
        forks[left_fork].lock();
        report(id, "took the left fork");

        // Próba podniesienia prawego widelca
        if (forks[right_fork].try_lock()) {
            report(id, "took the right fork and started eating");

            // Filozof je przez losowy czas
            int eating_time = dist(gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(eating_time));
            score[id]++; // Zwiększenie licznika posiłków
            report(id, "finished eating");

            // Zwolnienie obu widelców
            forks[right_fork].unlock();
            forks[left_fork].unlock();
        } else {
            // Jeśli nie można podnieść prawego widelca, zwolnienie lewego
            report(id, "couldn't take the right fork, returns the left one");
            forks[left_fork].unlock();
        }
    }
}

int main(int argc, char* argv[]) {
    // Sprawdzenie poprawności argumentów programu
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_philosophers>" << std::endl;
        return 1;
    }

    int N = std::stoi(argv[1]); // Liczba filozofów
    int run_time;

    // Pobranie czasu trwania symulacji od użytkownika
    std::cout << "Enter the duration of execution in seconds: ";
    std::cin >> run_time;

    // Inicjalizacja widelców i wyników
    std::vector<std::mutex> forks(N);
    std::vector<int> score(N, 0);
    std::vector<std::thread> philosophers;

    // Tworzenie wątków dla każdego filozofa
    for (int i = 0; i < N; i++) {
        philosophers.emplace_back(philosopher, i, std::ref(forks), std::ref(score));
    }

    // Uruchomienie symulacji na określony czas
    std::this_thread::sleep_for(std::chrono::seconds(run_time));
    running = false; // Zakończenie działania filozofów

    // Oczekiwanie na zakończenie wszystkich wątków
    for (auto& philosopher : philosophers) {
        philosopher.join();
    }

    // Wypisanie wyników końcowych
    for (int i = 0; i < N; i++) {
        std::cout << "Philosopher " << i << " ate " << score[i] << " times" << std::endl;
    }

    return 0;
}
