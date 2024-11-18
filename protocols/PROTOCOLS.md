# Protocols

You don't have to do much here, this is for understanding and actually abiding by Wayland Protocols to make use of their display system for rendering and getting a session lock.

In order for the display server to respect our request as a Wayland client, we should general some protocol code and header through their tool `wayland-scanner` that scans Wayland's XML protocols to actual code / header 

> [!NOTE]
> 
> **Generating protocol code from XML**:
> 
> Wayland Protocols are written in .xml files (typically in `/usr/share/wayland-protocols`)
> 
> To convert them to client/server headers:
> ```bash
> wayland-scanner client-header (or server-header) /path/to/protocol.xml /path/to/protocol.h
>
> # To convert the source code, you can convert protocols to public or private code (private code is HIGHLY RECOMMENDED BY WAYLAND)
> 
> wayland-scanner private-code (or public-code) /path/to/protocol.xml /path/to/protocol.c
> 
> ```
> 

## Generating required protocols:

Typically the **ext-session-lock-v1** protocol is located in `/usr/share/wayland-protocols/staging/ext-session-lock/ext-session-lock-v1.xml`

Similarly, **xdg-shell**'s *STABLE* protocol is typically in `/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml`.

Modify the below code's xml directory if this does not match your location of your xml protocols

```bash 

# FOR EXT_SESSION_LOCK_V1 PROTOCOL

# client header
[anvilock]$ wayland-scanner client-header /usr/share/wayland-protocols/staging/ext-session-lock/ext-session-lock-v1.xml protocols/ext-session-lock-client-protocol.h

# private source code 
[anvilock]$ wayland-scanner private-code /usr/share/wayland-protocols/staging/ext-session-lock/ext-session-lock-v1.xml protocols/src/ext-session-lock-client-protocol.c

# FOR XDG_SHELL_PROTOCOL

# client header 
[anvilock]$ wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml protocols/xdg-shell-client-protocol.h 

# private source code 
[anvilock]$ wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml protocols/src/xdg-shell-client-protocol.c
```

The protocols that we have used currently for this project are available in the repository.

> [!IMPORTANT]
> 
> These headers/source code for Wayland Protocols **can and will** be deprecated or invalid depending on what type of protocol/compositor you use 
> 
> There it is **HIGHLY RECOMMENDED** to generate YOUR own code using wayland-scanner to generate the required client/server headers and public/private code
> 
