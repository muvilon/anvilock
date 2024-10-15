# SECURITY for MUVILOCK

## Security Policy

This document outlines the security practices followed in the development and maintenance of the Wayland-based screenlock application.

### Supported Versions

We actively support the latest version of the screenlock application. All security updates, bug fixes, and new features are included in this version. We recommend keeping the application updated to the latest release to ensure you have the latest security patches.

### Reporting a Vulnerability

If you discover a security vulnerability in the application, please report it directly to our security team. All security vulnerabilities are taken seriously, and we will respond promptly to address the issue.

Contact: security@projectname.org

Please avoid publicly disclosing the vulnerability until we have had a chance to address it and deploy a fix.

## Security Features

### Wayland Protocol Compliance

This screenlock application is built natively for the Wayland protocol, ensuring compatibility with modern Linux display servers. Key features include:

- **Exclusive Access to the Display**: The screenlock leverages the Wayland protocol’s security mechanisms to isolate and block access to the display during screen locking. The compositor ensures that no unauthorized application can access or intercept input events while the screen is locked.
  
- **Input Capture**: We follow the recommended Wayland protocol practices to capture keyboard and pointer input exclusively when the screenlock is active. This ensures that no other application can interact with the system until the screen is unlocked.

### Password Buffer Security

For password input handling, we use a secure password buffer mechanism to mitigate risks associated with password storage and exposure:

- **Memory Sanitization**: Passwords are stored in secure memory buffers that are explicitly zeroed out after use, preventing residual password data from being accessed after unlocking.

- **No Disk Storage**: Passwords are never written to disk, including swap or other storage systems. The buffer resides entirely in RAM, reducing the risk of data recovery attacks.

- **Time-Limited Buffers**: Password buffers are retained only for the time necessary to validate the user’s credentials. Once the check is complete, the buffer is immediately erased to avoid lingering sensitive data in memory.

### Session lock

- **Sesstion lock protocol**: This implementation of a screen locker abides by the `ext-session-lock-v1` Wayland Protocol, ensuring that in the event of unexpected display server crashes, malicious attempts at bypassing the screen locker will be futile as the server ensures the safety and session integrity of us, the client.

- **Failed Attempts Logging**: Repeated failed attempts to unlock the screen are logged, with configurable limits on the number of allowed retries before additional security measures, such as temporary lockouts, are enforced (based on PAM entirely).

## Best Practices

We recommend following best practices to ensure maximum security when using the screenlock application:

- **Strong Passwords**: Users are encouraged to configure strong passwords for their accounts to prevent unauthorized access.
  
- **System Hardening**: This screenlock is designed as a part of a secure system configuration. We encourage users to apply system-wide security hardening practices, such as using encrypted storage and enabling secure boot.
  
- **Regular Updates**: Always update your system and the screenlock application to the latest version to benefit from the latest security fixes.

## Third-Party Libraries and Dependencies

We rely on trusted libraries and cryptographic implementations. All third-party dependencies are regularly reviewed and updated to mitigate potential vulnerabilities. These libraries include:

- `libwayland`: For Wayland protocol handling.
- `libXkbcommon`: For keyboard input management.
- `pam`: For authentication.

We follow strict guidelines to ensure that our dependencies are up-to-date and free from known vulnerabilities.
