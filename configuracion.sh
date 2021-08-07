echo Inicia configuracion.sh
cd ~/
echo 1) Clonar las commons
sleep 2
git clone https://github.com/sisoputnfrba/so-commons-library/

echo 2) Instalación de commons...
sleep 2
cd so-commons-library
sudo make install

echo 3) Clonar librerias del mapa
cd ~/
git clone https://github.com/sisoputnfrba/so-nivel-gui-library/
sleep 2

echo 4) Instalación de librerias del mapa...
cd so-nivel-gui-library
make install
sleep 2

echo 5) Volviendo a la carpeta principal...
cd /home/utnso/tp-2021-1c-UTNIX
sleep 2

echo 6) Agregar las librerias compartidas
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/home/utnso/tp-2021-1c-UTNIX/utils/Debug
cd utils/Debug
sleep 2

echo 7) Compilar la libreria LibUtils
make clean
make all
sudo cp libutils.so /usr/lib
sleep 2

echo 8) Volviendo a la carpeta principal...
cd /home/utnso/tp-2021-1c-UTNIX

echo Finaliza configuracion.sh
