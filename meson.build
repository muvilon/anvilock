project('anvilock', 'c',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'c_std=c11'
  ]
)

# Dependencies
freetype_dep = dependency('freetype2')
wayland_client_dep = dependency('wayland-client')
wayland_server_dep = dependency('wayland-server')
wayland_egl_dep = dependency('wayland-egl')
egl_dep = dependency('EGL')
gles_dep = dependency('GLESv2')
pam_dep = dependency('pam')
xkbcommon_dep = dependency('xkbcommon')
maths_dep = dependency('m')

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
    wayland_egl_dep,
    egl_dep,
    gles_dep,
    pam_dep,
    xkbcommon_dep,
    maths_dep
  ],
  include_directories: include_directories('toml')
)

# Custom target for config file generation
config_dir = '$HOME/.config/anvilock'
config_file = config_dir / 'config.toml'

run_command = find_program('sh')

run_target('config',
  command: [
    run_command, '-c',
    '''
    CONFIG_DIR="$HOME/.config/anvilock"
    CONFIG_FILE="$CONFIG_DIR/config.toml"
    
    # Exit if config file already exists
    if [ -f "$CONFIG_FILE" ]; then
      exit 0
    fi
    
    mkdir -p "$CONFIG_DIR"
    
    echo "[font]" > "$CONFIG_FILE"
    echo "name = \\"# your font name goes here\\"" >> "$CONFIG_FILE"
    echo "path = \\"# your font path goes here\\"" >> "$CONFIG_FILE"
    echo "\n" >> "$CONFIG_FILE"
    echo "[bg]" >> "$CONFIG_FILE"
    echo "name = \\"# your background name goes here\\"" >> "$CONFIG_FILE"
    echo "path = \\"# your background path goes here\\"" >> "$CONFIG_FILE"
    echo "Created config.toml with placeholder values in $CONFIG_DIR"
    '''
  ]
)
