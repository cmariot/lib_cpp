#include "data_structures/data_buffer.hpp"
#include <iostream>
#include <string>
#include <cassert>

void test_multiple_types() {
    DataBuffer buffer;

    // Test avec plusieurs types
    int num = 42;
    float pi = 3.14159f;
    std::string text = "Test";

    buffer << num << pi << text;

    int read_num;
    float read_pi;
    std::string read_text;

    buffer >> read_num >> read_pi >> read_text;

    std::cout << "Nombre lu: " << read_num << std::endl;
    std::cout << "Pi lu: " << read_pi << std::endl;
    std::cout << "Texte lu: " << read_text << std::endl;

    assert(num == read_num);
    assert(pi == read_pi);
    assert(text == read_text);
}

void test_buffer_overflow() {
    DataBuffer buffer;

    try {
        std::string str;
        buffer >> str;  // Devrait lancer une exception car le buffer est vide
        std::cout << "ERREUR: Exception attendue non reçue" << std::endl;
        assert(false);
    } catch (const std::out_of_range& e) {
        std::cout << "OK: Exception attendue reçue: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "=== Test des types multiples ===" << std::endl;
    test_multiple_types();

    std::cout << "\n=== Test de dépassement de buffer ===" << std::endl;
    test_buffer_overflow();

    std::cout << "\nTous les tests ont réussi !" << std::endl;
    return 0;
}