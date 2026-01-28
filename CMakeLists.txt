cmake_minimum_required (VERSION 3.8)

project ("snap7smake")

# 1. Ustawienie folderu z plikami nagłówkowymi (.h)
include_directories("${CMAKE_SOURCE_DIR}/libs")

# 2. Tworzenie pliku wykonywalnego
# UWAGA: Upewnij się, że ścieżka do main.cpp jest poprawna.
# Jeśli main.cpp jest w podfolderze, wpisz "snap7smake/main.cpp"
add_executable (snap7smake "main.cpp" "snap7.cpp" "Snap7Helper.h")

# 3. KLUCZOWE: Podpięcie biblioteki "na sztywno"
# To mówi Linkerowi dokładnie, gdzie leży plik .lib
target_link_libraries (snap7smake PRIVATE "${CMAKE_SOURCE_DIR}/libs/snap7.lib")

# 4. Kopiowanie DLL po kompilacji (żeby program działał)
add_custom_command(TARGET snap7smake POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/snap7.dll"
    $<TARGET_FILE_DIR:snap7smake>)