#include <anvilock/include/ClientState.hpp>

void ClientState::setLogContext(bool writeToFile, fsPath path, bool useTimestamp,
                                anvlk::logger::LogLevel logLevel)
{
  logCtx = {
    .toFile = writeToFile, .logFilePath = path, .timestamp = useTimestamp, .minLogLevel = logLevel};

  anvlk::logger::init(logCtx);
}
