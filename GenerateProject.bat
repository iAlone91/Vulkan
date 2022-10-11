@echo off
if not exist "C:\VulkanSDK\" (
  echo It seams that you don't have the Vulkan SDK installed. && echo Please download it at: https://vulkan.lunarg.com/
  PAUSE
)


call Premake5\Premake5.exe vs2022