#include <string>
#include <iostream>

extern "C" {
int add(int a, int b) { return a + b; }
void print(const char* str) { std::cout << str << std::endl; }
size_t strlen(const char* str) { return std::string(str).size(); }
}