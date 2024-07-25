cls

@echo off

setlocal
@REM out put
set "OutPut=.\build\BuildGame"
@REM libs
set "Vulkan=-L.\lib\VulkanSDK\lib\*.lib -I.\lib\VulkanSDK\include\ .\lib\VulkanSDK\lib\*.lib"
set "GLFW=.\lib\glfw-3.4\lib-static-ucrt\* -I.\lib\glfw-3.4\include\"
set "ImGui=.\lib\Dear-ImGui\*.cpp -I.\lib\Dear-ImGui\"
@REM all libs together
set "LIBs=%ImGui% %GLFW% %Vulkan%"
@REM project files path
set "ProjectFiles=.\src\main.cpp"
@REM compiler
set "Compiler=x86_64-w64-mingw32-g++ -std=c++23"
@echo on

%Compiler% -o %OutPut% %ProjectFiles% %LIBs%