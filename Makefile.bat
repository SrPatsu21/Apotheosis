cls

@echo off

setlocal
@REM out put
set "OutPut=.\build\BuildGamegit"
@REM libs
set "Vulkan=.\lib\VulkanSDK\lib\* -I.\lib\VulkanSDK\include\"
set "GLFW=.\lib\glfw-3.4\lib-static-ucrt\* -I.\lib\glfw-3.4\include\"
set "ImGui=.\lib\Dear-ImGui\*.cpp -I.\lib\Dear-ImGui\"
@REM all libs together
set "LIBs=%Vulkan% %GLFW% %ImGui%"
@REM project files path
set "ProjectFiles=.\src\main.cpp"

@echo on

g++ %LIBs% %ProjectFiles% -o %OutPut%