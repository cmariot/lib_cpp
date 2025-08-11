#include <mutex>
#include <iostream>
#include <string>


class ThreadSafeIOStream {

    /*
    Input / Output operations often become bottlenecks in high-performance applications.
    This class implements a thread-safe I/O stream with prefixed lines.
    This will allow to debug and monitor applications with multiple threads more easily.
    */

    public:

        ThreadSafeIOStream() : _prefix("") {}
        ~ThreadSafeIOStream() {}

        // Operator overloads for <<
        template<typename T>
        ThreadSafeIOStream& operator<<(const T& value) {
            std::lock_guard<std::mutex> lock(_mutex);
            std::cout << _prefix << value;
            return *this;
        }

        // Operator overloads for >>
        template<typename T>
        ThreadSafeIOStream& operator>>(T& value) {
            std::lock_guard<std::mutex> lock(_mutex);
            std::cin >> value;
            std::cout << _prefix << "You entered: " << value << std::endl;
            return *this;
        }

        // Sets the prefix printed before the line when using the iostream overload
        void setPrefix(const std::string& prefix) {
            _prefix = prefix;
            std::cout << "Prefix set to: " << _prefix << std::endl;
        }


        // template<typename T>
        // void prompt(const std::string& question, T& dest)
        template<typename T>
        void prompt(const std::string& question, T& dest) {
            std::lock_guard<std::mutex> lock(_mutex);
            std::cout << _prefix << question;
            std::cin >> dest;
            std::cout << _prefix << "You entered: " << dest << std::endl;
        }


    private:

        std::mutex _mutex; // Mutex for thread safety
        std::string _prefix; // Prefix for each line

    /*
    Ensure it is thread-safe and thread-local, adding a prefix to each line.
    Additionally, provide an equivalent to std::cout so there’s no need to create a custom iostream.
    We expect you to include something like thread_local ThreadSafeIOStream threadSafeCout;
    in your header file.
    */

};