SETLOCAL
IF [%1] == [] (
  SET config=Release
) ELSE (
  SET config="%1"
)
CALL %~dp0../../darcel/build/windows/build.bat %config%
CALL %~dp0../../applications/darcel/build/windows/build.bat %config%
ENDLOCAL
