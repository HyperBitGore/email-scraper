#define CURL_STATICLIB
#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <vector>
#include <string>

std::ofstream file;
std::string current;

size_t write_data(char* data, size_t itemsize, size_t nitems, void* ignore) {
	size_t bytes = itemsize * nitems;
	for (int i = 0; i < bytes; i++) {
		file << data[i];
	}
	return bytes;
}
int countSubstring(const std::string& str, const std::string& sub)
{
	if (sub.length() == 0) return 0;
	int count = 0;
	for (size_t offset = str.find(sub); offset != std::string::npos;
		offset = str.find(sub, offset + sub.length()))
	{
		++count;
	}
	return count;
}

void findLinks(std::vector<std::string>& links) {
	std::ifstream f;
	f.open("temp.txt");
	std::string line;
	while (getline(f, line)) {

		int number = countSubstring(line, "href=");
		if (number > 0) {
			std::cout << number << std::endl;
		}
		for (size_t start = line.find("href="); start != std::string::npos; start = line.find("href=", start)) {
			for (start; line[start] != '='; start++);
			start += 2;
			std::string cur;
			for (start; line[start] != '\"'; start++) {
				cur.push_back(line[start]);
			}
			if (cur.find("https://") == std::string::npos) {
				int n = 0;
				std::string add;
				for (int i = 0; i < current.size(); i++) {
					add.push_back(current[i]);
					if (i == '/') {
						n++;
					}
					if (n > 2) {
						i = current.size();
					}
				}
				std::string temp = add + cur;
				links.push_back(temp);
			}
			else {
				links.push_back(cur);
			}
		}
	}
	std::ofstream temp;
	temp.open("tt.txt");
	for (auto& i : links) {
		temp << i << '\n';
	}
	temp.close();
	f.close();
}
void findEmails(std::string emails) {
	std::fstream e;
	e.open(emails);



	e.close();
}

//https://curl.se/libcurl/c/CURLOPT_REDIR_PROTOCOLS.html
//might need this
//Get crawling working properly, then get email parsing in, and finally add some way to manually exit
//Use my twitter to test https://twitter.com/hyperbitgore
int main() {
	CURL* curl;
	std::vector<std::string> links;
	curl = curl_easy_init();
	std::string startingurl;
	std::cout << "Type in staring url please: ";
	std::cin >> startingurl;
	current = startingurl;
	links.push_back(current);
	while (links.size() > 0) {
		file.open("temp.txt");
		for (auto& j : links) {
			curl_easy_setopt(curl, CURLOPT_URL, j.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_perform(curl);
			curl_easy_reset(curl);
			findLinks(links);
		}
		file.close();
	}
	system("pause");

	curl_easy_cleanup(curl);
	return 0;
}