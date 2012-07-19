@echo off
IF "%ProgramFiles(x86)%"=="" SET PATH="C:\Borland\BCC55\Bin";"C:\Program Files\Git\cmd";"c:\Program Files\doxygen\bin";C:\Windows\System32 & exit /b
SET PATH=C:\Borland\BCC55\Bin;"C:\Program Files (x86)\Git\cmd";"c:\Program Files\doxygen\bin";C:\Windows\System32
