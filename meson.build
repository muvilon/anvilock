project('anvilock', 'c',
  default_options : [
    'warning_level=3',
    'c_std=c11'
  ]
)

# Dependencies
freetype_dep = dependency('freetype2')
wayland_client_dep = dependency('wayland-client')
wayland_server_dep = dependency('wayland-server')
pam_dep = dependency('pam')
xkbcommon_dep = dependency('xkbcommon')

# Source files
src_files = [
  'main.c',
  'toml/toml.c'
]

# Compiler flags
add_project_arguments(
  '-g',
  '-Wpedantic',
  language: 'c'
)

# Create executable
executable('anvilock',
  sources: src_files,
  dependencies: [
    freetype_dep,
    wayland_client_dep,
    wayland_server_dep,
    pam_dep,
    xkbcommon_dep
  ],
  include_directories: include_directories('toml')
)

# Custom target for config file generation
config_file = 'config.toml'
run_command = find_program('sh')

run_target('config',
  command: [
    run_command, '-c',
    '''
    if [ ! -f @0@ ]; then
      echo "[font]" > @0@
      echo "name = \\"# your font name goes here\\"" >> @0@
      echo "path = \\"# your font path goes here\\"" >> @0@
      echo "Created config.toml with placeholder values."
    fi
    '''.format(config_file)
  ]
)
