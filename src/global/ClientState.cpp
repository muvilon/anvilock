#include "anvilock/include/Types.hpp"
#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/shaders/ShaderHandler.hpp>

void ClientState::setLogContext(bool writeToFile, fsPath path, bool useTimestamp,
                                anvlk::logger::LogLevel logLevel)
{
  logCtx = {
    .toFile = writeToFile, .logFilePath = path, .timestamp = useTimestamp, .minLogLevel = logLevel};

  anvlk::logger::init(logCtx);
}

void ClientState::initShaderManager()
{
  shaderManagerPtr = std::make_unique<anvlk::gfx::ShaderManager>(homeDir, logCtx);
}

void ClientState::initPamAuth()
{
  pamAuth = std::make_unique<anvlk::pam::PamAuthenticator>(pamState.username, pamState.password);
}

void ClientState::destroyEGL()
{
  eglDestroySurface(eglDisplay, eglSurface);
  eglDestroyContext(eglDisplay, eglContext);
  eglTerminate(eglDisplay);
}

void ClientState::disconnectWLDisplay()
{
  wl_display_roundtrip(wlDisplay);
  wl_display_disconnect(wlDisplay);
}

/// KB STATE ///

void KeyboardState::resetState()
{
  ctrlHeld          = false;
  backspaceHeld     = false;
  lastBackspaceTime = SteadyClock::now();
}

/// PAM STATE ///

auto PamState::canSeekIndex() -> bool { return password.size() < MAX_PASSWORD_LENGTH; }

auto PamState::canSeekToOffset(const i64& offset) -> bool
{
  return offset >= 0 &&
         (password.size() + static_cast<anvlk::types::iter>(offset) <= MAX_PASSWORD_LENGTH);
}

void PamState::seekToIndex(const i64& idx)
{
  if (idx >= 0)
    passwordIndex += idx;
  password.resize(passwordIndex);
}

void PamState::clearPassword()
{
  passwordIndex = 0;
  password.clear();
}
