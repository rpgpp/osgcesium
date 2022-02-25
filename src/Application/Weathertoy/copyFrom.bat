@echo off
@set root=%~dp0
xcopy %root%\..\..\..\..\apache-tomcat-7.0.99\webapps\WeatherLab\WeatherLab\js\app.js %root%\main /D/Y
xcopy %root%\..\..\..\..\apache-tomcat-7.0.99\webapps\WeatherLab\WeatherLab\js\chunk-vendors.js %root%\main /D/Y
pause