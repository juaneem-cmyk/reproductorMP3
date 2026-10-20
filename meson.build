project('reproductorMP3', 'cpp', version: '1.0.0', 
default_options: ['cpp_std=c++17', 'warning_level=3'])

taglib = dependency('taglib', method: 'pkg-config')
sqlite3 = dependency('sqlite3', method: 'pkg-config')
add_project_arguments('-DREPRODUCTORMP3_SOURCE_DIR="' + meson.project_source_root() + '"', language: 'cpp')

subdir('src/minero')
