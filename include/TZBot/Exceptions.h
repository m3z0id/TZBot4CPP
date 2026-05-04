#pragma once
#include <exception>

class QueueAbortException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override;
};

class PacketParseException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override;
};

class SocketReadException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override;
};