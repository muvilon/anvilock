#include <algorithm>
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
  pamAuth = std::make_unique<anvlk::pam::PamAuthenticator>(pam.username, pam.password);
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

auto PamState::canSeekIndex() -> bool { return password.size() < MAX_PASSWORD_LENGTH; }

auto PamState::canSeekToIndex(const i64& idx) -> bool
{
  return idx >= 0 && (password.size() + static_cast<size_t>(idx) <= MAX_PASSWORD_LENGTH);
}

void PamState::seekToIndex(const i64& idx)
{
  if (idx >= 0)
    passwordIndex += idx;
}

void PamState::clearPassword()
{
  passwordIndex = 0;
  std::ranges::fill(password, '\0');
  password.clear();
}
