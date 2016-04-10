rmdir WildWilley
mkdir WildWilley
copy Release\WW.exe WildWilley\WildWilley.exe
copy WW\config.xml WildWilley\config.xml
mkdir WildWilley\Resources
xcopy WW\Resources WildWilley\Resources /E