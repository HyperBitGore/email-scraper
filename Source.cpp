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
	file << data;
	//free(data);
	return bytes;
}


void findLinks(std::vector<std::string>& links) {
	std::ifstream f;
	f.open("temp.txt");
	std::string line;
	while (getline(f, line)) {
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
//Fix continous memory buildup because I don't clear curl buffers, then get email parsing in
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
	for (int i = 0; i < links.size(); i++) {
		file.open("temp.txt");
		current = links[i];
		std::cout << "Getting " << links[i] << std::endl;
		curl_easy_setopt(curl, CURLOPT_URL, links[i].c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_perform(curl);
		curl_easy_reset(curl);
		findLinks(links);
		links.erase(links.begin() + i);
		file.close();
	}
	system("pause");

	curl_easy_cleanup(curl);
	return 0;
}