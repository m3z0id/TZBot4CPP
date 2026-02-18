#include "../include/Exceptions.h"

const char *QueueAbortException::what() const noexcept { return "Queue Aborted"; }
const char *PacketParseException::what() const noexcept { return "Failed to parse packet"; }
const char *SocketReadException::what() const noexcept { return "Failed to read from socket"; }