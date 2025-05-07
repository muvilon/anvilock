#pragma once

#include <anvilock/include/Types.hpp>
#include <anvilock/include/pam/PasswordBuffer.hpp>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

namespace anvlk::types::pam
{
using PamHandle_   = pam_handle_t;
using PamMessage_  = struct pam_message;
using PamResponse_ = struct pam_response;
using PamConv_     = struct pam_conv;
} // namespace anvlk::types::pam

namespace anvlk::pam
{

class PamAuthenticator
{
public:
  explicit PamAuthenticator(types::AuthString& userName, types::AuthString& password);
  auto AuthenticateUser() -> bool;

private:
  static auto PamConvFunc(int numMsg, const types::pam::PamMessage_** msg,
                          types::pam::PamResponse_** resp, types::VPtr appDataPtr) -> int;

  types::AuthString&     m_userName;
  types::AuthString      m_password;
  pwdbuf::PasswordBuffer m_passwordBuffer;
};

} // namespace anvlk::pam
