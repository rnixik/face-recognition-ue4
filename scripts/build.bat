@ECHO OFF
for /f "delims=" %%x in (%~dp0.env) do (set "%%x")


echo Preparing Android.mk
SET ANDROID_MK_FILE=%~dp0..\jni\Android.mk
SET OPENCV_MK_PATH="%opencv_android_sdk_dir%\native\jni\OpenCV.mk"
if not exist "%OPENCV_MK_PATH%" (
  echo "Check .env file! OPENCV_MK_PATH not found by path: %OPENCV_MK_PATH%"
) else (
  copy /Y %~dp0AndroidOpenCV_mk.tpl "%ANDROID_MK_FILE%"
  powershell -Command "(gc %ANDROID_MK_FILE%) -replace '_OPENCV_MK_PATH_', '%OPENCV_MK_PATH%' | Out-File %ANDROID_MK_FILE%" -encoding ASCII
)

echo Building for Android ARM
if not exist "%nkd-build%" (
  echo "Check .env file! nkd-build.cmd not found by path: %nkd-build%"
) else (
  del /q "%~dp0..\obj\*"
  cd %~dp0..\source
  call "%nkd-build%" && echo OK
)

echo Preparing VS files
SET VCXPROJ_FILE=%~dp0..\source\FaceRecognition.vcxproj
SET OPENCV_INCLUDE_PATH="%opencv_win64_build_dir%\include"
SET OPENCV_LIB_PATH="%opencv_win64_build_dir%\x64\vc14\lib"
SET OPENCV_VERSION="%opencv_version%"
copy /Y %~dp0FaceRecognition_vcxproj.tpl "%VCXPROJ_FILE%"
powershell -Command "(gc %VCXPROJ_FILE%) -replace '_OPENCV_INCLUDE_PATH_', '%OPENCV_INCLUDE_PATH%' -replace '_OPENCV_LIB_PATH_', '%OPENCV_LIB_PATH%' -replace '_OPENCV_VERSION_', '%OPENCV_VERSION%' | Out-File %VCXPROJ_FILE%" -encoding ASCII


echo Building for Win x64
if not exist "%devenv%" (
  echo "Check .env file! devenv.exe not found by path: %devenv%"
) else (
  cd %~dp0..\source
  "%devenv%" FaceRecognition.sln /build Release && echo OK
)


echo Copying files to UE4
if not exist "%ue4_engine_dir%\Build\Android\Java\src\com\epicgames\ue4\GameActivity.java" (
  echo "Check .env file! Path of UE4 Engine is not correct: %ue4_engine_dir%"
) else (
  copy /Y %~dp0GameActivity.java "%ue4_engine_dir%\Build\Android\Java\src\com\epicgames\ue4\GameActivity.java"
  xcopy /Y /S /E /Q %~dp0org "%ue4_engine_dir%\Build\Android\Java\src\org\"
  xcopy /Y /S /E /Q %~dp0..\libs\armeabi-v7a "%ue4_engine_dir%\Build\Android\Java\jni\armeabi-v7a\"
  copy /Y %~dp0Android.mk "%ue4_engine_dir%\Build\Android\Java\jni\Android.mk"
  copy /Y %~dp0proguard-project.txt "%ue4_engine_dir%\Build\Android\Java\proguard-project.txt"
)

echo Copying files to project
if not exist "%ue4_project_dir%\Config\DefaultEngine.ini" (
  echo "Check .env file! Path of ue4-project is not correct: %ue4_project_dir%"
) else (
  xcopy /Y /S /E /Q %~dp0..\libs\armeabi-v7a\libfacerecognition.so "%ue4_project_dir%\ThirdParty\FaceRecognition\Android\"
  xcopy /Y /S /E /Q %~dp0..\source\x64\Release\FaceRecognition.dll "%ue4_project_dir%\ThirdParty\FaceRecognition\Win64\"
  xcopy /Y /S /E /Q %~dp0..\source\x64\Release\FaceRecognition.lib "%ue4_project_dir%\ThirdParty\FaceRecognition\Win64\"
  xcopy /Y /S /E /Q %~dp0..\source\x64\Release\FaceRecognition.dll "%ue4_project_dir%\Binaries\Win64\"
)

echo Copying OpenCV
if not exist "%opencv_win64_build_dir%\include\opencv2\core.hpp" (
  echo "Check .env file! Path of opencv_win64_build_dir is not correct: %opencv_win64_build_dir%"
) else (
  call %~dp0prepare_opencv_hpp.bat
  xcopy /Y /S /E /Q "%opencv_win64_build_dir%\include\opencv2" "%ue4_project_dir%\ThirdParty\FaceRecognition\Includes\opencv2\"
  copy /Y %~dp0utility.hpp "%ue4_project_dir%\ThirdParty\FaceRecognition\Includes\opencv2\core\utility.hpp"
  xcopy /Y /S /E /Q "%opencv_win64_build_dir%\x64\vc14\bin\opencv_world%opencv_version%.dll" "%ue4_project_dir%\Binaries\Win64\"
)

pause
exit