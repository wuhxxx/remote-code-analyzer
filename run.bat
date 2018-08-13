cd Debug
start Sever.exe ../zSeverDir/ 8081
start FrontEnd.exe ../zSeverDir/ ../zClientDir/ 8081 8082
cd ..