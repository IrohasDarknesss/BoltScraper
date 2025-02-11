#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <curl/curl.h>
#include <thread>
#include <vector>
#include <string>
#include <mutex>

namespace py = pybind11;

std::mutex mtx;

// Function to write data fetched by libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t totalSize = size * nmemb;
    s->append((char*)contents, totalSize);
    return totalSize;
}

// Function to fetch a single URL
std::string fetch_url(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            throw std::runtime_error("CURL error: " + std::string(curl_easy_strerror(res)));
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return readBuffer;
}

// Function to fetch multiple URLs in parallel
std::vector<std::string> fetch_urls(const std::vector<std::string>& urls) {
    std::vector<std::string> results(urls.size());
    std::vector<std::thread> threads;

    for (size_t i=0; i < urls.size(); ++i) {
        threads.emplace_back([&, i] {
            try
            {
                std::string content = fetch_url(urls[i]);
                std::lock_guard<std::mutex> lock(mtx);
                results[i] = content;
            }
            catch(const std::exception& e)
            {
               std::lock_guard<std::mutex> lock(mtx);
               results[i] = "Error: " + std::string(e.what());
            } 
        });
    }

    for (auto& thread : threads) {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    return results;
}

PYBIND11_MODULE(BoltScraper, m) {
    m.doc() = "BoltScraper: A lightning-fast, multithreaded web scraper using C++ and pybind11";
    m.def("fetch_url", &fetch_url, "Fetch a single URL");
    m.def("fetch_urls", &fetch_urls, "Fetch multiple URLs in parallel");
}