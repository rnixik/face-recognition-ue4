@ECHO OFF
for /f "delims=" %%x in (%~dp0.env) do (set "%%x")

echo Building for Win x64
if not exist "%devenv%" (
  echo "Check .env file! devenv.exe not found by path: %devenv%"
) else (
  cd %~dp0..\source
  "%devenv%" FaceRecognition.sln /build Release && echo OK
)

echo Copying files to project
if not exist "%ue4_project_dir%\Config\DefaultEngine.ini" (
  echo "Check .env file! Path of ue4-project is not correct: %ue4_project_dir%"
) else (
  xcopy /Y /S /E /Q %~dp0..\source\FaceRecognition.h "%ue4_project_dir%\ThirdParty\FaceRecognition\Includes\"
  xcopy /Y /S /E /Q %~dp0..\source\x64\Release\FaceRecognition.dll "%ue4_project_dir%\ThirdParty\FaceRecognition\Win64\"
  xcopy /Y /S /E /Q %~dp0..\source\x64\Release\FaceRecognition.lib "%ue4_project_dir%\ThirdParty\FaceRecognition\Win64\"
  xcopy /Y /S /E /Q %~dp0..\source\x64\Release\FaceRecognition.dll "%ue4_project_dir%\Binaries\Win64\"
)
