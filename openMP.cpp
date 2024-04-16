#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;

vector<int> naiveOpenMp(string str, string pattern) {
    int m = pattern.length();
    int n = str.length();
    vector<int> res = vector<int>();

    #pragma omp parallel for
    for (int i = 0; i <= n - m; i++) {
        bool found = true;
        for (int j = 0; j < m; j++) {
            if (str[i + j] != pattern[j]) {
                found = false;
                break;
            }
        }

        if (found) {
            #pragma omp critical
            {
                res.push_back(i);
            }
        }
    }

    return res;
}

vector<int> rabinKarp(string str, string pattern) {
    
    if (str.length() && pattern.length()) {
        int n = str.length();
        int m = pattern.length();
        int i, j;  
        int s = 0, p = 0;  
        const int pm = 256;                      
        const int q = 101;                       
        int h = 1;                               
        bool flag = false;
        vector<int> res = vector<int>();

        for (i = 0; i < m - 1; i++)
            h = (h * pm) % q;

        for (i = 0; i < m; i++) {
            s = (pm * s + str[i]) % q;
            p = (pm * p + pattern[i]) % q;
        }

        for (i = 0; i <= n - m; i++) {
            if (s == p) { 
                for (j = 0; j < m; j++)
                    if (str[i + j] != pattern[j])
                        break;
                if (j == m) {
                    res.push_back(i);
                    flag = true;
                }
            }
            
            s = (pm * (s - h * str[i]) + str[i + m]) % q;
            if (s < 0) 
                s = s + q;
        }
        if (!flag)
            std::cout << "Pattern not found.." << endl;
        return res;
    }
    std::cout << "Text or pattern cannot be empty.." << endl;
}

vector<int> substringMatchingNaive(string str, string pattern) {
    int strSize = str.size();
    int patternSize = pattern.size();
    vector<int> res = vector<int>();

    for (int i = 0; i < strSize - patternSize + 1; i++) {
        bool isMatch = true;

        for (int j = 0; j < patternSize; j++) {
            if (str[i + j] != pattern[j])
                isMatch = false;
        }

        if (isMatch)
            res.push_back(i);
    }

    return res;
}

vector<int> rabinKarpOpenMp(string str, string pattern) {
    if (str.length() && pattern.length()) {
        int n = str.length();
        int m = pattern.length();
        int i, j;
        int s = 0, p = 0;
        const int pm = 256;
        const int q = 101;
        int h = 1;
        vector<int> res;

        for (i = 0; i < m - 1; i++)
            h = (h * pm) % q;

        for (i = 0; i < m; i++) {
            p = (pm * p + pattern[i]) % q;
        }

        #pragma omp parallel for private(j, s) schedule(static)
        for (i = 0; i <= n - m; i++) {

            if (s <= 0) {
                for (j = 0; j < m; j++) {
                    s = (pm * s + str[i + j]) % q;
                }
            }
            if (s == p) {
                for (j = 0; j < m; j++)
                    if (str[i + j] != pattern[j])
                        break;
                if (j == m) {
                    #pragma omp critical
                    {
                        res.push_back(i);
                    }
                }

            }

            s = (pm * (s - h * str[i]) + str[i + m]) % q;
            if (s < 0)
                s = s + q;
        }

        return res;
    }
    std::cout << "Text or pattern cannot be empty.." << endl;
    return {};
}

int main() {
    ifstream file("C:/Users/Novikov/source/repos/SubstringMatching/input.txt");
    string str;
    string file_contents;
    while (getline(file, str))
    {
        file_contents += str;
        file_contents.push_back('\n');
    }

    string pattern = "ROMEO";
   
    auto start = chrono::steady_clock::now();

    auto res = rabinKarpOpenMp(file_contents, pattern);

    auto end = chrono::steady_clock::now();

    auto diff = end - start;

    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(diff).count() << endl;

    return 0;
}