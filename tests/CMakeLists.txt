cmake_minimum_required(VERSION 3.5)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Test
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Core Sql)

# ==============================
# Fontes de teste
# ==============================

set(TEST_SOURCES
    test_main.cpp

    # util
    util/test_dbutil.cpp

    # infra de teste
    db/test_db_factory.cpp

    # services
    services/test_produto_service.cpp
)

# ==============================
# Executável de testes
# ==============================
add_executable(QEstoqueLojaTests
    ${TEST_SOURCES}

    # Código real do projeto usado nos testes
    ${CMAKE_SOURCE_DIR}/src/util/dbutil.cpp
    ${CMAKE_SOURCE_DIR}/src/services/Produto_service.cpp
    ${CMAKE_SOURCE_DIR}/src/util/ibptutil.cpp
    services/test_produto_service.h
    ${CMAKE_SOURCE_DIR}/src/repository/Produto_repository.cpp

)

# ==============================
# Includes
# ==============================

target_include_directories(QEstoqueLojaTests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/util
    ${CMAKE_SOURCE_DIR}/src/services
    ${CMAKE_SOURCE_DIR}/src/dto
    ${CMAKE_SOURCE_DIR}/tests
)

# ==============================
# Linkagem
# ==============================

target_link_libraries(QEstoqueLojaTests PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
)


install(DIRECTORY ${CMAKE_SOURCE_DIR}/recursos/
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/QEstoqueLojaTests/recursos
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/reports/
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/QEstoqueLojaTests/reports
)

# --- Definir APPDATA_DIR corretamente ---
if (WIN32)
    # Roaming AppData
    file(TO_CMAKE_PATH "$ENV{APPDATA}" APPDATA_PATH)
    set(APPDATA_DIR_TEST "${APPDATA_PATH}/QEstoqueLojaTests")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if (DEFINED ENV{XDG_DATA_HOME} AND NOT "$ENV{XDG_DATA_HOME}" STREQUAL "")
        set(APPDATA_DIR_TEST "$ENV{XDG_DATA_HOME}/QEstoqueLojaTests")
    else()
        set(APPDATA_DIR_TEST "$ENV{HOME}/.local/share/QEstoqueLojaTests")
    endif()
endif()

# --- Copiar diretórios fixos ---
add_custom_command(TARGET QEstoqueLojaTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${APPDATA_DIR_TEST}/reports"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/reports" "${APPDATA_DIR_TEST}/reports"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${APPDATA_DIR_TEST}/recursos"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/recursos" "${APPDATA_DIR_TEST}/recursos"
    COMMENT "Copiando reports e recursos para ${APPDATA_DIR_TEST}"
)
# ==============================
# Registrar no CTest
# ==============================

add_test(
    NAME QEstoqueLojaTests
    COMMAND QEstoqueLojaTests
)
