#ifndef AGG2_LOG_H
#define AGG2_LOG_H

#include <iostream>
#include <alog/Logger.h>
#include <alog/Configurator.h>

#define AGG2_LOG_CONFIG(configPath) do {                          \
        try {                                                   \
            alog::Configurator::configureLogger(configPath);    \
        } catch(std::exception &e) {                            \
            std::cerr << "parse alog config failed "            \
                      << e.what() << std::endl;                 \
            exit(-1);                                           \
        }                                                       \
    }while(0)

#define AGG2_LOG_DECLARE() static alog::Logger *_logger;

#define AGG2_LOG_SETUP(n,c) alog::Logger *c::_logger     \
    = alog::Logger::getLogger("Agg2." #n "."  #c)

#define AGG2_LOG_SETUP_TEMPLATE(n,c,T) template <typename T>     \
    alog::Logger *c<T>::_logger                                 \
    = alog::Logger::getLogger("Agg2." #n "."  #c)

#define AGG2_LOG(level, format, args...)                         \
    ALOG_LOG(_logger, alog::LOG_LEVEL_##level, format, ##args)

#define AGG2_LOG_DECLARE_WITH_NAME(name) static alog::Logger *name

#define AGG2_LOG_SETUP_WITH_NAME(name, n, c) alog::Logger *c::name \
        = alog::Logger::getLogger("Agg2." #n "."  #c)

#define AGG2_LOG_WITH_NAME(name, level, format, args...) ALOG_LOG(name, alog::LOG_LEVEL_##level, format, ##args)

#define AGG2_LOG_SHUTDOWN() alog::Logger::shutdown()

#define AGG2_LOG_FLUSH() alog::Logger::flushAll()
#endif //AGG2_LOG_H
