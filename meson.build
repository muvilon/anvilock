project('anvilock', 'c', '11')

# Dependencies
freetype_dep = dependency('freetype2', required: true)
wayland_client_dep = dependency('wayland-client', required: true)
wayland_server_dep = dependency('wayland-server', required: true)
pam_dep = dependency('pam', required: true)
xkbcommon_dep = dependency('xkbcommon', required: true)

# Sources
src = 'main.c'

# Executable
executable('anvilock', src,
    dependencies: [freetype_dep, wayland_client_dep, wayland_server_dep, pam_dep, xkbcommon_dep],
    cpp_args: ['-Wall', '-Wextra', '-Wpedantic', '-g']
)
