@echo off

for /r . %%g in (*.gv) do (
	echo Plotting %%~ng
	dot -Tpng "%%g" > "%%g".png
	dot -Teps "%%g" > "%%g".eps
)

move /Y exports\*.png plots\ 
move /Y exports\*.eps plots\ 
