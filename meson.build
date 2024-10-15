project('muvilock', 'c')

cc = meson.get_compiler('c')

wayland_client = dependency('wayland-client')
wayland_server = dependency('wayland-server')
pam = cc.find_library('pam', required: true)
xkbcommon = dependency('xkbcommon')

executable(
  'muvilock',
  'main.c',
  dependencies: [wayland_client, wayland_server, pam, xkbcommon],
  install: true
)
