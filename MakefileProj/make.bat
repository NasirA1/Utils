@if not "%VISUALCVARS%" == "TRUE" (
@echo Setting environment for Visual C++ 2015...
@set VISUALCVARS=TRUE
@call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat")
@call make.exe %*
