#include "../libftpp.hpp"


int main() {

    // Create an instance of the thread-safe I/O stream
    ThreadSafeIOStream threadSafeCout;

    // Set a prefix for the output
    threadSafeCout.setPrefix("[Thread 1] ");

    // // Use the thread-safe output stream
    threadSafeCout << "Hello, World!";

    return (0);

}