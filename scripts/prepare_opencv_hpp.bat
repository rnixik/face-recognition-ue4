@ECHO OFF
for /f "delims=" %%x in (%~dp0.env) do (set "%%x")

echo Copying OpenCV hpp
if not exist "%opencv_android_sdk_dir%\native\jni\include\opencv2\core\utility.hpp" (
  echo "Check .env file! Path of opencv_android_sdk_dir is not correct: %opencv_android_sdk_dir%"
) else (
  SET UTILITY_HPP_FILE=%~dp0utility.hpp
  copy /Y "%opencv_android_sdk_dir%\native\jni\include\opencv2\core\utility.hpp" "%UTILITY_HPP_FILE%"
  powershell -Command "(gc %UTILITY_HPP_FILE%) -replace 'if defined\(check\)', 'if defined(check_should_not_be_defined)' -replace 'bool check\(\) const;', '//bool check() const;' | Out-File %UTILITY_HPP_FILE%" -encoding ASCII
)
