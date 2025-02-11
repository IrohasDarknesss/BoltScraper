import time
import BoltScraper
import requests
from bs4 import BeautifulSoup

# List of URLs to be compared
urls = [
    'https://qiita.com/',
    'https://cloud.google.com/',
    'https://openai.com/',
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

# BeautifulSoup Benchmarks
print("\nRunning BeautifulSoup Benchmark...")
soup_start = time.time()

soup_results = []
for url in urls:
    response = requests.get(url)
    soup = BeautifulSoup(response.content, 'html.parser')
    soup_results.append(soup)

soup_end = time.time()

print(f"BeautifulSoup - Total elapsed time: {soup_end - soup_start:.4f} s")
print(f"BeautifulSoup - Average time per 1URL: {(soup_end - soup_start) / len(urls):.4f} s")

# Brief comparison of results
print("\n--- Performance Comparison ---")
if (bolt_end - bolt_start) < (soup_end - soup_start):
    print("✅ BoltScraper is faster than BeautifulSoup!")
else:
    print("⚠️  BeautifulSoup produced faster results.")
