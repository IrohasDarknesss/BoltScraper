# BoltScraper
BoltScraper is a lightning-fast, multithreaded web scraping library built with C++ and pybind11. It enables efficient data collection from multiple websites simultaneously, making it ideal for handling large-scale scraping tasks. 

## How to Install

1. Clone the repository or download the source code.
2. Install `pybind11` and `libcurl`.
   ```
   pip install pybind11
   sudo apt-get install libcurl4-openssl-dev zlib1g-dev  # Ubuntu/Debianの場合
   ```
3. Build the library with the following command
   ```
   python setup.py build_ext --inplace
   ```

## How to use

### Obtaining a single URL
```
import BoltScraper

content = BoltScraper.fetch_url('https://example.com')
print(content)
```

### Parallel acquisition of multiple URLs
```
import BoltScraper

urls = [
    'https://example.com',
    'https://www.python.org',
    'https://www.github.com'
]

contents = BoltScraper.fetch_urls(urls)

for idx, page in enumerate(contents):
    try:
        print(f"Contents of page {idx + 1}:\n{page[:200].encode('utf-8', errors='ignore').decode('utf-8')}\n")
    except UnicodeDecodeError as e:
        print(f"Decode error on page {idx + 1}: {e}")
```

### Performance Benchmarks
```
import BoltScraper

urls = [
    'https://example.com',
    'https://www.python.org',
    'https://www.github.com'
]

# BoltScraper Benchmarks
print("Running BoltScraper Benchmark...")
bolt_start = time.time()
bolt_results = BoltScraper.fetch_urls(urls)
bolt_end = time.time()

print(f"BoltScraper - Total elapsed time: {bolt_end - bolt_start:.4f} s")
print(f"BoltScraper - Average time per 1URL: {(bolt_end - bolt_start) / len(urls):.4f} s")
```

## License
This project is provided under the MIT License.