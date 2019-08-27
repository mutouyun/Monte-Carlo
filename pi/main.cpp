#include <iostream>
#include <cstdint>
#include <cmath>
#include <random>
#include <chrono>
#include <utility>
#include <algorithm>
#include <thread>
#include <future>
#include <iomanip>
#include <limits>

using namespace std;

int main() {
    size_t needles = 0;
    while (1) {
        cout << "enter count of needles: ";
        cin >> needles;
        if (needles == 0) {
            cout << "entered count is zero!" << endl;
        }
        else break;
    }

    size_t radius = (min)(needles / 3, static_cast<size_t>(sqrt((numeric_limits<size_t>::max)() / 2)));
    size_t coords_count = radius * radius;
    cout <<  "count is: " << needles      << endl
         << "radius is: " << radius       << endl
         << "coords is: " << coords_count << endl;

    size_t n = thread::hardware_concurrency();
    cout << endl << "hardware concurrency is: " << n << endl;
    if (n == 0) n = 1;

    auto calc = [needles, radius, coords_count, n](size_t k) {
        mt19937_64 rand_engine { random_device{}() };
        uniform_int_distribution<size_t> ud;
        size_t inner = 0;
        size_t size  = (needles / n) + ((k == (n - 1)) ? (needles % n) : 0);
        cout << "start calc(" << k << "): size = " << size << endl;
        for (size_t i = 0; i < size; ++i) {
            size_t v = ud(rand_engine) % coords_count; // pick a coordinate
            size_t y = v / radius;
            size_t x = v - (y * radius);
            if ((x * x + y * y) <= coords_count) ++inner;
        }
        return inner;
    };

    size_t inner = 0;
    cout << "start calculating..." << endl << endl;
    auto tp = chrono::steady_clock::now();
    if (n > 1) {
        vector<future<size_t>> calc_rets;
        for (size_t k = 0; k < n; ++k) {
            packaged_task<size_t(size_t)> task { calc };
            calc_rets.emplace_back(task.get_future());
            thread(move(task), k).detach();
        }
        for (size_t k = 0; k < n; ++k) {
            size_t r = calc_rets[k].get();
            cout << "got calc(" << k << "): " << r << endl;
            inner += r;
        }
    }
    else inner = calc(0);
    cout << endl << "calc time: "
         << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - tp).count() / 1000.0
         << " s" << endl;
    cout << "inner is: " << inner << endl;
    cout << "pi is: " << setprecision(numeric_limits<double>::max_digits10) 
                      << ((inner * 4.0) / needles) << endl;
    return 0;
}

