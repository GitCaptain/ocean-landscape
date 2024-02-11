#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

#define LOG(x) do { x; } while (0)

#ifdef DEBUG

#define LOG_INFO(x) LOG(x)
#define LOG_DEBUG(x) LOG(x)

#else

#define LOG_INFO(x) LOG(x)
#define LOG_DEBUG(x)

#endif

#define START() LOG_DEBUG(std::cout << "Begin function: " \
                                    << __PRETTY_FUNCTION__  << '\n';);

#endif
