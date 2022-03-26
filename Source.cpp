#define CURL_STATICLIB
#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

std::ofstream file;
std::string current;
std::vector<std::string> processed;
char* datar;
std::string odata;

size_t write_data(char* data, size_t itemsize, size_t nitems, void* ignore) {
	size_t bytes = itemsize * nitems;
	file << data;
	return bytes;
}


void findLinks(std::vector<std::string>& links) {
	std::ifstream f;
	f.open("temp.txt");
	std::string line;
	while (getline(f, line)) {
		for (size_t start = line.find("href="); start != std::string::npos && start < line.size(); start = line.find("href=", start)) {
			bool skip = false;
			for (start; line[start] != '=' && start < line.size(); start++);
			start += 2;
			if (start > line.size()) {
				skip = true;
			}
			if(!skip){
			std::string cur;
			for (start; line[start] != '#' && line[start] != '\"' && start < line.size(); start++) {
				cur.push_back(line[start]);
			}
			if (cur.find("https://") == std::string::npos) {
				int n = 0;
				std::string add;
				for (int i = 0; n < 3 && i < current.size(); i++) {
					if (current[i] == '/') {
						n++;
					}
					if (n < 3) {
						add.push_back(current[i]);
					}
				}
				std::string temp = add + cur;
				if (temp.find("#twtr-main") == std::string::npos) {
					links.push_back(temp);
				}
			}
			else {
				if (cur.find("#twtr-main") == std::string::npos) {
					links.push_back(cur);
				}
			}
			}
		}
	}
	f.close();
}
void findEmails(std::vector<std::string>& emails) {
	std::ifstream e;
	std::fstream f;
	f.open("emails.txt", std::ios::app | std::ios::out);
	e.open("temp.txt");
	std::string line;
	while (getline(e, line)) {
		size_t start = line.find("@");
		if (start != std::string::npos) {
				std::string temp;
				bool skip = false;
				bool non = false;
				for (start; line[start] != ' ' && line[start] != '\"' && line[start] != ':' && line[start] != '/' && start > 0; start--);
				if (line[start] == ':' || line[start] == '\"' || line[start] == '>') {
					start++;
				}
				if (start > line.size() - 1) {
					non = true;
				}
				bool domainmode = false;
				int domainsize = 0;
				if (!non) {
					for (start; line[start] != '\"' && line[start] != '\n' && line[start] != ' ' && line[start] != '<' && start < line.size(); start++) {
						if (domainmode) {
							if (line[start] > 255 || line[start] < -1) {
								skip = true;
								break;
							}
							if (std::isalnum(line[start]) || line[start] == '.' || line[start] == '-') {
								temp.push_back(line[start]);
								domainsize++;
							}
							else {
								skip = true;
								break;
							}
							//Maximum label length in domains
							if (domainsize > 63) {
								skip = true;
								break;
							}
						}
						else {
							temp.push_back(line[start]);
							if (line[start] == '@') {
								domainmode = true;
							}
						}
					}
					for (auto& i : emails) {
						if (temp.compare(i) == 0) {
							skip = true;
							break;
						}
					}
					if (!skip) {
						if (temp.find(".com") != std::string::npos || temp.find(".net") != std::string::npos || temp.find(".org") != std::string::npos) {
							std::cout << "Adding " << temp << std::endl;
							emails.push_back(temp);
							f << temp << '\n';
						}
					}
				}
			}
	}
	f.close();
	e.close();
}
//Use my twitter to test https://twitter.com/hyperbitgore
int main() {
	CURL* curl;
	std::vector<std::string> links;
	std::vector<std::string> emails;
	curl = curl_easy_init();
	std::string startingurl;
	std::cout << "Type in staring url please: ";
	std::cin >> startingurl;
	current = startingurl;
	links.push_back(current);
	remove("emails.txt");
	while(links.size() > 0) {
		bool skip = false;
		for (auto& i : processed) {
			if (links[0].compare(i) == 0) {
				links.erase(links.begin());
				skip = true;
				break;
			}
		}
		if (!skip) {
			file.open("temp.txt");
			current = links[0];
			std::cout << "Getting " << current << std::endl;
			curl_easy_setopt(curl, CURLOPT_URL, links[0].c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_perform(curl);
			curl_easy_reset(curl);
			findLinks(links);
			findEmails(emails);
			processed.push_back(links[0]);
			file.close();
		}
		links.erase(links.begin());
		std::sort(links.begin(), links.end());
		links.erase(std::unique(links.begin(), links.end()), links.end());
	}
	system("pause");

	curl_easy_cleanup(curl);
	return 0;
}