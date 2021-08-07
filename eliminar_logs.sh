echo Arranca eliminar_logs.sh
sleep 1
echo Eliminando: Discordiador.log
cd Discordiador/Debug
rm Discordiador.log
rm DiscordiadorOn.log
cd ../..
sleep 1
echo Eliminando: Mi-RAM-HQ.log
cd Mi-RAM/Debug
rm Mi-RAM-HQ.log
cd ../..
sleep 1
echo Eliminando: Mongo-Store.log
cd Mongo-Store/Debug
rm Mongo-Store.log
cd ../..
sleep 1
echo Terminó la eliminación de los logs
