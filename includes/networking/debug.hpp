#ifndef LIBFTPP_NETWORKING_DEBUG_HPP
#define LIBFTPP_NETWORKING_DEBUG_HPP

// To enable network debug logging, define LIBFTPP_NETWORK_DEBUG at compile time.
#ifdef LIBFTPP_NETWORK_DEBUG
#include <iostream>
#define NET_LOG(x) do { std::cout << x << std::endl; std::flush(std::cout); } while(0)
#else
#define NET_LOG(x) do { (void)0; } while(0)
#endif

#endif // LIBFTPP_NETWORKING_DEBUG_HPP
