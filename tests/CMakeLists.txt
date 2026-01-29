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
    services/test_inserirproduto_service.cpp
)

# ==============================
# Executável de testes
# ==============================

add_executable(QEstoqueLojaTests
    ${TEST_SOURCES}

    # Código real do projeto usado nos testes
    ${CMAKE_SOURCE_DIR}/src/util/dbutil.cpp
    ${CMAKE_SOURCE_DIR}/src/services/InserirProduto_service.cpp
    ${CMAKE_SOURCE_DIR}/src/util/ibptutil.cpp
    services/test_inserirproduto_service.h
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

# ==============================
# Registrar no CTest
# ==============================

add_test(
    NAME QEstoqueLojaTests
    COMMAND QEstoqueLojaTests
)
