#include "anvilock/include/Types.hpp"
#include <anvilock/include/pam/PamAuthenticator.hpp>
#include <cstring> // for std::memcpy
#include <cstring> // for strdup
#include <memory>  // for std::make_unique

namespace anvlk::pam
{

PamAuthenticator::PamAuthenticator(types::AuthString& userName, types::AuthString& password)
    : m_userName(std::move(userName)), m_password(std::move(password)),
      m_passwordBuffer(m_password.size() + 1)
{
}

auto PamAuthenticator::AuthenticateUser() -> bool
{
  std::memcpy(m_passwordBuffer.get(), m_password.data(), m_password.size() + 1);

  types::pam::PamConv_ pamConversation = {.conv        = PamConvFunc,
                                          .appdata_ptr = m_passwordBuffer.get()};

  types::pam::PamHandle_* pamHandle = nullptr;
  types::Status pamStatus = pam_start("login", m_userName.c_str(), &pamConversation, &pamHandle);

  if (pamStatus != PAM_SUCCESS)
    return false;

  pamStatus = pam_authenticate(pamHandle, 0);
  if (pamStatus != PAM_SUCCESS)
  {
    pam_end(pamHandle, pamStatus);
    return false;
  }

  pamStatus = pam_acct_mgmt(pamHandle, 0);
  if (pamStatus != PAM_SUCCESS)
  {
    pam_end(pamHandle, pamStatus);
    return false;
  }

  pam_end(pamHandle, PAM_SUCCESS);
  return true;
}

auto PamAuthenticator::PamConvFunc(int numMsg, const types::pam::PamMessage_** msg,
                                   types::pam::PamResponse_** resp, types::VPtr appDataPtr) -> int
{
  auto password = static_cast<types::PamString>(appDataPtr);
  auto reply    = std::make_unique<types::pam::PamResponse_[]>(numMsg);

  for (types::iter i = 0; i < numMsg; ++i)
  {
    if (msg[i]->msg_style == PAM_PROMPT_ECHO_OFF)
    {
      reply[i].resp         = strdup(password);
      reply[i].resp_retcode = 0;
    }
    else if (msg[i]->msg_style == PAM_PROMPT_ECHO_ON)
    {
      reply[i].resp         = strdup(msg[i]->msg);
      reply[i].resp_retcode = 0;
    }
  }

  *resp = reply.release();
  return PAM_SUCCESS;
}

} // namespace anvlk::pam
