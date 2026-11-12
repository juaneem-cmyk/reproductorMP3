qt6_widgets = dependency('qt6', modules: ['Widgets'])
qt6_multimedia = dependency('qt6', modules: ['Multimedia'])
qt6_concurrent = dependency('qt6', modules: ['Concurrent'])

executable('reproductor', 'main.cpp', 'VentanaPrincipal.cpp',
    # Componentes visuales
    'componentes/Portada.cpp',
    'modelos/Canciones.cpp', 
    'paneles/PanelAlbum.cpp', 'paneles/PanelCanciones.cpp',
    'paneles/PanelControles.cpp', 'paneles/BarraHerra.cpp',
    'ventanas/VentanaEdicion.cpp', 'ventanas/VentanaInterp.cpp',
    'ventanas/VentanaAlbumes.cpp',
    'reproduccion/ControladoRep.cpp',
    'controladores/ContEditCanciones.cpp', 'controladores/ContEditInterp.cpp',
    'controladores/ContImport.cpp', 'controladores/ContRepro.cpp',
    'controladores/ContEditAlbumes.cpp',
    'servicios/GestorImagenes.cpp',

    # Lector de metadatos e importación
    '../minero/LectorMetadatos.cpp', '../minero/ImportarMusica.cpp',
    '../minero/ExplorarDirectorios.cpp',

    # Base de datos
    '../baseDatos/CrearBD.cpp', '../baseDatos/ConexionSQLite.cpp',
    '../baseDatos/InicializarBD.cpp', '../baseDatos/Rutas.cpp',
    '../baseDatos/Biblioteca.cpp', 

    # Repositorio
    '../baseDatos/repositorio/ConsultaRolas.cpp',
    '../baseDatos/repositorio/RepositorioMusical.cpp',
    '../baseDatos/repositorio/EdicionCanciones.cpp',
    '../baseDatos/repositorio/EdicionInterp.cpp',
    '../baseDatos/repositorio/CrearInterp.cpp',
    '../baseDatos/repositorio/EdicionAlbumes.cpp',

    include_directories: include_directories('../baseDatos',
    '../baseDatos/repositorio', '../minero'),

    dependencies: [qt6_widgets, qt6_multimedia, qt6_concurrent, taglib, sqlite3],
    install: false
)