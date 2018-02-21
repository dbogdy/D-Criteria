#!/bin/bash
# Modifica Fisierul MPCORB.DAT

echo "Download dabase files ..."

echo "Download NEA database ..."
wget https://www.minorplanetcenter.net/iau/MPCORB/NEA.txt

# for all asteroids uncomment the next line and comment the previous line
#wget https://www.minorplanetcenter.net/iau/MPCORB/MPCORB.DAT

echo "Download Meteor shower database ..."
wget pallas.astro.amu.edu.pl/~jopek/MDC2007/Etc/streamestablisheddata.csv

echo "Start ..."

# Modify the file streamestablisheddata.csv  ($3-ad node $4-code  $14-a $15-q 16-e $17-peri $18-node $19-inc)
echo "File streamestablisheddata.csv ----> processing ..."
cat database/streamestablisheddata.csv \ |grep -v ^:|grep -v ^+|more|sed -e 's/\"//g'\ | gawk 'BEGIN{FS="|"}{print $3" "$4" "$14" "$15" "$16" "$17" "$18" "$19}' \
> data/stream_showers.dat

echo "File NEA.txt ----> processing ..."
# Delete the first 42 lines from the file 
# If NEA.txt let the value 0, 
tail -n + 0 database/NEA.txt > data/mpcorb-light.dat 

# copiaza coloana cu marimea data intr-un fisier separat
cut -c 38-46 data/mpcorb-light.dat > Peri
cut -c 49-57 data/mpcorb-light.dat > Node
cut -c 60-68 data/mpcorb-light.dat > Incl
cut -c 71-79 data/mpcorb-light.dat > exccent
cut -c 93-103 data/mpcorb-light.dat > semiaxa
#cut -c 106-106 data/mpcorb-light.dat > U
#sed -i -e 's/ /Z/g' U  # sterge toate spatiile goale
cut -c 167-192 data/mpcorb-light.dat > name
sed -i -e 's/ //g' name  # sterge toate spatiile goale

paste name semiaxa exccent Incl Peri Node > data/mpcorb-light.dat
rm Peri Node Incl exccent semiaxa name

echo "Finish."
