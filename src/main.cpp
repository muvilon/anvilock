#include <anvilock/include/pam/pam.hpp>

using namespace anvlk;

using logL = logger::LogLevel;

// This is a basic PAM setup (very naive and impractical) that showcases the
// current progress of the porting process of Anvilock to C++.

auto main() -> int
{
  ClientState cs;

  cs.setLogContext(true, "log.txt", true, logger::LogLevel::Debug);

  pam::PamAuthenticator auth(cs);
  logger::log(logL::Info, cs.logCtx, "Hello, gib password: ");

  AuthString pwd;
  std::cin >> pwd;

  cs.pam.password = pwd;
  cs.pam.username = "s1dd";

  if (auth.AuthenticateUser())
  {
    logger::log(logL::Info, cs.logCtx, "ok");
  }
  else
  {
    logger::log(logL::Error, cs.logCtx, "Error while auth");
  }

  return 0;
}
