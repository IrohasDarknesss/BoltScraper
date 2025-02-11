#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <curl/curl.h>
#include <zlib.h>
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

// Function to decompress gzip data
std::string decompress_gzip(const std::string& data) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit2(&zs, 16 + MAX_WBITS) != Z_OK) {
        throw std::runtime_error("inflateInit failed");
    }

    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data.data()));
    zs.avail_in = data.size();

    int ret;
    char outbuffer[32768];
    std::string outstring;

    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out) {
            outstring.append(outbuffer, zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        throw std::runtime_error("Exception during zlib decompression");
    }

    return outstring;
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
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");  // Handle compressed data
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);    // Follow redirects

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            throw std::runtime_error("CURL error: " + std::string(curl_easy_strerror(res)));
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    try {
        return decompress_gzip(readBuffer);
    } catch (...) {
        return readBuffer;  // Return as-is if not gzip compressed
    }
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

// Function to measure performance
py::dict benchmark(const std::vector<std::string>& urls) {
    auto start = std::chrono::high_resolution_clock::now();
    fetch_urls(urls);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    py::dict result;
    result["urls_fetched"] = urls.size();
    result["elapsed_time"] = elapsed.count();
    result["average_time_per_url"] = elapsed.count() / urls.size();

    return result;
}

PYBIND11_MODULE(BoltScraper, m) {
    m.doc() = "BoltScraper: A lightning-fast, multithreaded web scraper using C++ and pybind11";
    m.def("fetch_url", &fetch_url, "Fetch a single URL");
    m.def("fetch_urls", &fetch_urls, "Fetch multiple URLs in parallel");
    m.def("benchmark", &benchmark, "Benchmark the scraping performance");
}