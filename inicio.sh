echo Arranca inicio.sh
sleep 2
echo Arranca: Discordiador
cd Discordiador/Debug
make clean
make all
cd ../..
sleep 2
echo Arranca: Mongo-Store
cd Mongo-Store/Debug
make clean
make all
cd ../..
sleep 2
echo Arranca: Mi-RAM
cd Mi-RAM/Debug
make clean
make all
cd ../..
sleep 2
echo Se terminó la carga de los módulos
