qt6_widgets = dependency('qt6', modules: ['Widgets'])

executable('reproductor', 'main.cpp', 'VentanaPrincipal.cpp',
    # Componentes visuales
    'componentes/Portada.cpp',
    'modelos/Canciones.cpp',
    'paneles/PanelAlbum.cpp',
    'paneles/PanelCanciones.cpp',

    # Lector de metadatos
    '../minero/LectorMetadatos.cpp',

    # Base de datos
    '../baseDatos/CrearBD.cpp', '../baseDatos/ConexionSQLite.cpp',
    '../baseDatos/InicializarBD.cpp', '../baseDatos/Rutas.cpp',
    '../baseDatos/Biblioteca.cpp',

    # Repositorio
    '../baseDatos/repositorio/ConsultaRolas.cpp',
    '../baseDatos/repositorio/RepositorioMusical.cpp',

    include_directories: include_directories('../baseDatos',
    '../baseDatos/repositorio', '../minero'),

    dependencies: [qt6_widgets, taglib, sqlite3],
    install: false
)