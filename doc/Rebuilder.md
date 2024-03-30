```
mkdir prepared

./ep1_portkit.py -c -r -pf E1_R373_G_0E.30.DAR -o build -f E1_R373_G_0E.30.DAR.smg

mv build/Lib.sym prepared/portkit.sym

./forge.py -sn -s prepared/gsm_flash_dev.sym -d ../../res/E1_R373_G_0E.30.DAR/elfloader.sym -e EP1 -pf 'E1_R373_G_0E.30.DAR' -o prepared/ep1_gsm.sym

./forge.py -sn -s prepared/gsm_flash_dev.sym -d ../../res/E1_R373_G_0E.30.DAR/library.sym -e EP2 -pf 'E1_R373_G_0E.30.DAR' -o prepared/ep2_gsm.sym


./compare.py -s prepared/ep1_gsm.sym -c ../../res/E1_R373_G_0E.30.DAR/elfloader.sym -n &> prepared/ep1_notfound.log

./compare.py -s prepared/ep2_gsm.sym -c ../../res/E1_R373_G_0E.30.DAR/library.sym -ec EP2 -n &> prepared/ep2_notfound.log

cat prepared/ep1_notfound.log | grep 0x | awk '{print $4 " " $5 " " $6;}' | tr -d '"' > prepared/ep1.def

cat prepared/ep2_notfound.log | grep 0x | awk '{print $4 " " $5 " " $6;}' | tr -d '"' > prepared/ep2.def

./forge.py -sn -s prepared/portkit.sym -d prepared/ep1.def -e EP1 -pf 'E1_R373_G_0E.30.DAR' -o prepared/ep1_chunk.sym
./forge.py -sn -s prepared/portkit.sym -d prepared/ep2.def -e EP2 -pf 'E1_R373_G_0E.30.DAR' -o prepared/ep2_chunk.sym

cat prepared/ep1_gsm.sym prepared/ep1_chunk.sym > prepared/ep1_combined.sym
cat prepared/ep2_gsm.sym prepared/ep2_chunk.sym > prepared/ep2_combined.sym


./compare.py -s prepared/ep1_combined.sym -c ../../res/E1_R373_G_0E.30.DAR/elfloader.sym -n
./compare.py -s prepared/ep2_combined.sym -c ../../res/E1_R373_G_0E.30.DAR/library.sym -ec EP2 -n

```

