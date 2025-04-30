#pragma once

#include "PasswordBuffer.hpp"
#include "anvilock/include/Types.hpp"
#include <anvilock/include/ClientState.hpp>
#include <memory>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

namespace anvlk::pam
{
class PamAuthenticator
{
public:
  explicit PamAuthenticator(const ClientState& clientState)
      : m_clientState(clientState), m_passwordBuffer(m_clientState.pam.password.size() + 1)
  {
  }

  auto AuthenticateUser() -> bool
  {
    std::memcpy(m_passwordBuffer.get(), m_clientState.pam.password.data(),
                m_clientState.pam.password.size() + 1);

    struct pam_conv pamConversation = {
      .conv        = PamConvFunc,
      .appdata_ptr = m_passwordBuffer.get()}; // Pass the buffer to the conversation function

    pam_handle_t* pamHandle = nullptr;
    Status        pamStatus =
      pam_start("login", m_clientState.pam.username.c_str(), &pamConversation, &pamHandle);

    if (pamStatus != PAM_SUCCESS)
    {
      return false;
    }

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

private:
  static auto PamConvFunc(int numMsg, const struct pam_message** msg, struct pam_response** resp,
                          void* appDataPtr) -> int
  {
    auto password = static_cast<PamString>(appDataPtr); // Get the password
    auto reply    = std::make_unique<struct pam_response[]>(numMsg);

    for (iter i = 0; i < numMsg; ++i)
    {
      if (msg[i]->msg_style == PAM_PROMPT_ECHO_OFF)
      {
        // Use the provided password for echo-off prompts
        reply[i].resp         = strdup(password);
        reply[i].resp_retcode = 0;
      }
      else if (msg[i]->msg_style == PAM_PROMPT_ECHO_ON)
      {
        reply[i].resp         = strdup(msg[i]->msg);
        reply[i].resp_retcode = 0;
      }
    }

    *resp = reply.release(); // Transfer ownership of the response array to PAM
    return PAM_SUCCESS;
  }

  const ClientState&     m_clientState;    // Reference to the client state (no copying)
  pwdbuf::PasswordBuffer m_passwordBuffer; // Secure password buffer
};

} // namespace anvlk::pam
