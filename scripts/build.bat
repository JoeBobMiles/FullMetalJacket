@echo off

rem
rem @file build.bat
rem @author Joseph Miles <josephmiles2015@gmail.com>
rem @date 2019-02-03
rem
rem This is the build script for Windows, using clang-cl.
rem

cls

if %1.==release. (
    set debug=""
) else (
    set debug="/D DEBUG"
)

if not exist build\ (
    md build\
)

pushd build
clang-cl %debug% /Zi ..\src\win32_main.cpp user32.lib /I ..\include /o fullmetaljacket.exe
popd
