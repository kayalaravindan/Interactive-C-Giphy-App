#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::vector<std::string> get_gif_urls(const std::string& query) {
    CURL* curl = curl_easy_init();
    std::string url = "https://api.giphy.com/v1/gifs/search?api_key=pZGkKNHrkhBC5DUgG3MsB0Ml4JIVu3kC&q=" + query + "&limit=10";
    std::string response_string;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        throw std::runtime_error("curl error: " + std::string(curl_easy_strerror(res)));
    }

    json data = json::parse(response_string);

    std::vector<std::string> gif_urls;
    for (const auto& gif_data : data["data"]) {
        gif_urls.push_back(gif_data["images"]["original"]["url"]);
    }

    return gif_urls;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " search <query>" << std::endl;
        return 1;
    }

    if (std::string(argv[1]) != "search") {
        std::cerr << "Usage: " << argv[0] << " search <query>" << std::endl;
        return 1;
    }

    std::string query = argv[2];

    try {
        std::vector<std::string> gif_urls = get_gif_urls(query);
        int num_results = std::min(5, (int)gif_urls.size());

        if (num_results == 0) {
            std::cout << "No GIFs found for query: " << query << std::endl;
        } else {
            std::cout << "Here are the top " << num_results << " GIFs for your search:\n";
            for (int i = 0; i < num_results; i++) {
                std::cout << i + 1 << ". " << gif_urls[i] << std::endl;
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
main.cpp
        Displaying main.cpp.