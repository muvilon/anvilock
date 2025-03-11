#ifndef TEST_PAM_H
#define TEST_PAM_H

#include "../log.h"
#include "password_buffer.h"
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int pam_conv_func(int num_msg, const struct pam_message** msg, struct pam_response** resp,
                         void* appdata_ptr)
{
  const char*          password = (const char*)appdata_ptr; // Get the password from appdata_ptr
  struct pam_response* reply    = calloc(num_msg, sizeof(struct pam_response));

  for (int i = 0; i < num_msg; i++)
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

  *resp = reply;
  return PAM_SUCCESS;
}

// Function to authenticate the user with PAM
int authenticate_user(const char* username, const char* input_password)
{
  // Create a secure password buffer
  char* password = password_buffer_create(strlen(input_password) + 1);
  if (!password)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to create secure password buffer.");
    return 0;
  }

  // Copy the input password into the secure buffer
  strncpy(password, input_password, strlen(input_password) + 1);

  struct pam_conv pam_conversation = {pam_conv_func, (void*)password}; // Pass password here
  pam_handle_t*   pamh             = NULL;
  int             pam_status       = pam_start("login", username, &pam_conversation, &pamh);

  if (pam_status != PAM_SUCCESS)
  {
    log_message(LOG_LEVEL_ERROR, "PAM start failed: %s", pam_strerror(pamh, pam_status));
    password_buffer_destroy(password, strlen(input_password) + 1); // Clean up
    return 0;
  }

  pam_status = pam_authenticate(pamh, 0);
  if (pam_status != PAM_SUCCESS)
  {
    log_message(LOG_LEVEL_ERROR, "PAM authentication failed: %s", pam_strerror(pamh, pam_status));
    pam_end(pamh, pam_status);
    password_buffer_destroy(password, strlen(input_password) + 1); // Clean up
    return 0;
  }

  pam_status = pam_acct_mgmt(pamh, 0);
  if (pam_status != PAM_SUCCESS)
  {
    log_message(LOG_LEVEL_ERROR, "PAM account management failed: %s",
                pam_strerror(pamh, pam_status));
    pam_end(pamh, pam_status);
    password_buffer_destroy(password, strlen(input_password) + 1); // Clean up
    return 0;
  }

  pam_end(pamh, PAM_SUCCESS);
  password_buffer_destroy(password, strlen(input_password) + 1); // Clean up
  return 1;
}

#endif // TEST_PAM_H
