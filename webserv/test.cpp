#include <iostream>
#include <string>
#include <curl/curl.h>

// Callback function to receive data returned by the server
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    userp->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void performGetRequest(const std::string &url)
{
    CURL *curl;
    CURLcode res;
    std::string responseString;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "GET request failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            std::cout << "GET request response:\n"
                      << responseString << std::endl;
        }

        curl_easy_cleanup(curl);
    }
}

void performPostRequest(const std::string &url, const std::string &postData)
{
    CURL *curl;
    CURLcode res;
    std::string responseString;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "POST request failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            std::cout << "POST request response:\n"
                      << responseString << std::endl;
        }

        curl_easy_cleanup(curl);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <url> <get/post> [data]" << std::endl;
        return 1;
    }

    std::string method = argv[2];
    std::string url = argv[1];

    if (method == "get")
    {
        performGetRequest(url);
    }
    else if (method == "post" && argc > 3)
    {
        std::string postData = argv[3];
        performPostRequest(url, postData);
    }
    else
    {
        std::cerr << "Invalid arguments. For POST, you need to provide data." << std::endl;
    }

    return 0;
}
