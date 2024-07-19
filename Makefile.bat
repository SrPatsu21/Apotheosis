@echo off

setlocal
@REM out put
set "OutPut=./build/BuildGamegit"
@REM libs
set "Vulkan=.\lib\VulkanSDK\lib\*"
set "GLFW=.\lib\glfw-3.4\lib-static-ucrt\*"
set "ImGui=./lib/Dear-ImGui/*.cpp"
@REM all libs together
set "LIBs=%Vulkan% %GLFW% %ImGui%"
@REM project files path
set "ProjectFiles=./src/main.cpp"

g++ %LIBs% %ProjectFiles% -o %OutPut%