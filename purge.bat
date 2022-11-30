:: Purge for Visual Studio Work Environment
:: Felix Schuster | 2022-03-17

@echo off

:fun
:: rmdir /s /q .git 2>NUL
rmdir /s /q .vs 2>NUL
rmdir /s /q x64 2>NUL
for /D %%d in (*) do (
    cd %%d
    call :fun
    cd ..
)