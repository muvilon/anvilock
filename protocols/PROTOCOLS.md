# Protocols

It is a pre-requisite to understand how Wayland Protocols work in order to contribute or understand this part of the code.

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

The protocols that we have used currently for this project are available in the repository.

> [!IMPORTANT]
> 
> These headers/source code for Wayland Protocols **can and will** be deprecated or invalid depending on what type of protocol/compositor you use 
> 
> There it is **HIGHLY RECOMMENDED** to generate YOUR own code using wayland-scanner to generate the required client/server headers and public/private code
> 
