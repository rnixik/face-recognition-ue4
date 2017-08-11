@ECHO OFF
for /f "delims=" %%x in (%~dp0.env) do (set "%%x")

echo Building for Android ARM
if not exist "%nkd-build%" (
  echo "Check .env file! nkd-build.cmd not found by path: %nkd-build%"
) else (
  del /q "%~dp0..\obj\*"
  cd %~dp0..\source
  call "%nkd-build%" && echo OK
)

echo Copying files to UE4
if not exist "%ue4_engine_dir%\Build\Android\Java\src\com\epicgames\ue4\GameActivity.java" (
  echo "Check .env file! Path of UE4 Engine is not correct: %ue4_engine_dir%"
) else (
  xcopy /Y /S /E /Q %~dp0..\libs\armeabi-v7a "%ue4_engine_dir%\Build\Android\Java\jni\armeabi-v7a\"
)

echo Copying files to project
if not exist "%ue4_project_dir%\Config\DefaultEngine.ini" (
  echo "Check .env file! Path of ue4-project is not correct: %ue4_project_dir%"
) else (
  xcopy /Y /S /E /Q %~dp0..\source\FaceRecognition.h "%ue4_project_dir%\ThirdParty\FaceRecognition\Includes\"
  xcopy /Y /S /E /Q %~dp0..\libs\armeabi-v7a\libfacerecognition.so "%ue4_project_dir%\ThirdParty\FaceRecognition\Android\"
)
