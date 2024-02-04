#ifndef LOGGER_H
#define LOGGER_H

//#ifdef DEBUG
#if 1
#include <iostream>

#define LOG(x) do { x; } while (0)

#else

#define LOG(x)

#endif

#endif
