Rebuild EP Libraries
====================

## R373_G_0E.30.DAR ElfLoader EP1/EP2 libraries rebuilding recipe

1. Get `release/gsm_flash_dev.sym` symbols file from building system.

```bash
# Create building directory.
mkdir prepared

# Generate EP1 library from patterns.
./ep1_portkit.py -pf E1_R373_G_0E.30.DAR -t -i -r -a -p ../../ep1/pts/E1_R373_G_0E.30.DAR.pts -f E1_R373_G_0E.30.DAR.smg
cp build/Lib.sym prepared/portkit.sym

# Get from `gsm_flash_dev.sym` only the necessary entities.
./forge.py -sn -s prepared/gsm_flash_dev.sym -d ../../res/E1_R373_G_0E.30.DAR/elfloader.sym -e EP1 -pf 'E1_R373_G_0E.30.DAR' -o prepared/ep1_gsm.sym
./forge.py -sn -s prepared/gsm_flash_dev.sym -d ../../res/E1_R373_G_0E.30.DAR/library.sym -e EP2 -pf 'E1_R373_G_0E.30.DAR' -o prepared/ep2_gsm.sym

# Found all missing symbols and constants then save it.
./compare.py -s prepared/ep1_gsm.sym -c ../../res/E1_R373_G_0E.30.DAR/elfloader.sym -n
./compare.py -s prepared/ep1_gsm.sym -c ../../res/E1_R373_G_0E.30.DAR/elfloader.sym -n &> prepared/ep1_notfound.log
./compare.py -s prepared/ep2_gsm.sym -c ../../res/E1_R373_G_0E.30.DAR/library.sym -ec EP2 -n
./compare.py -s prepared/ep2_gsm.sym -c ../../res/E1_R373_G_0E.30.DAR/library.sym -ec EP2 -n &> prepared/ep2_notfound.log
cat prepared/ep1_notfound.log | grep 0x | awk '{print $4 " " $5 " " $6;}' | tr -d '"' > prepared/ep1.def
cat prepared/ep2_notfound.log | grep 0x | awk '{print $4 " " $5 " " $6;}' | tr -d '"' > prepared/ep2.def
cat prepared/ep2_notfound.log | grep 0x | grep ' C ' | awk '{print $4 " " $5 " " $6;}' | tr -d '"' > prepared/ep2_c.def

# Generate combined EP1 library symbols file.
./forge.py -sn -s prepared/portkit.sym -d prepared/ep1.def -e EP1 -pf 'E1_R373_G_0E.30.DAR' -o prepared/ep1_chunk.sym
cat prepared/ep1_gsm.sym prepared/ep1_chunk.sym > prepared/ep1_combined.sym

# Generate combined EP2 library symbols file.
./forge.py -sn -s prepared/portkit.sym -d prepared/ep2.def -e EP2 -pf 'E1_R373_G_0E.30.DAR' -o prepared/ep2_chunk.sym
./forge.py -sn -c -s ../../res/E1_R373_G_0E.30.DAR/library.sym -d prepared/ep2_const.def -e EP2 -pf 'E1_R373_G_0E.30.DAR' -o prepared/ep2_const.sym
cat prepared/ep2_gsm.sym prepared/ep2_chunk.sym prepared/ep2_const.sym > prepared/ep2_combined.sym

# Compare and check missing symbols.
./compare.py -s prepared/ep1_combined.sym -c ../../res/E1_R373_G_0E.30.DAR/elfloader.sym -n
./compare.py -s prepared/ep2_combined.sym -c ../../res/E1_R373_G_0E.30.DAR/library.sym -ec EP2 -n
```

2. Add `Class_dal` and `_region_table` function to the `ep1_combined.sym` and `ep2_combined.sym` symbol files manually.

```bash
# EP1:
# cat gsm_flash_dev.sym | grep 'D dal'
# using SimTech MOOSE emulator when ROM is run:
# dw 0x12230F50 100
0x121075D0 D Class_dal

# EP2:
# cat gsm_flash_dev.sym | grep 'D dal'
0x12230F50 D Class_dal

# readelf -s gsm_flash_dev.elf | grep _region_table
0x10A5B8A4 D _region_table
```

3. Generate final EP1/EP2 libraries and clean listings.

```bash
./ep1_libgen.py -sn -s prepared/ep1_combined.sym -o prepared/elfloader.lib
./ep2_libgen.py -sn -s prepared/ep2_combined.sym -pf 'E1_R373_G_0E.30.DAR' -o prepared/library.bin

./ep1_libgen.py -sn -s prepared/elfloader.lib -pf 'E1_R373_G_0E.30.49R' -o prepared/elfloader.sym
./ep2_libgen.py -sn -s prepared/library.bin -p 'E1' -o prepared/library.sym
```

4. Fix `SBCM_ATOD_temp` and `SBCM_ATOD_vltg` addresses and modes in `library.sym` listing.

```bash
./compare.py -es EP2 -s ep2_combined.sym -c library.sym -ec EP2
31-Mar-2024 13:24:36 INFO: Start Comparator utility, mode: "SYM_TO_SYM".
31-Mar-2024 13:24:36 INFO: Modes missmatch:
31-Mar-2024 13:24:36 INFO: Addresses missmatch:
31-Mar-2024 13:24:36 INFO: 	"0x127BBECB D SBCM_ATOD_temp" in "prepared0/ep2_combined.sym" file.
31-Mar-2024 13:24:36 INFO: 	"0x127BBECA T SBCM_ATOD_temp" in "prepared/library.sym" file.
31-Mar-2024 13:24:36 INFO: Addresses missmatch:
31-Mar-2024 13:24:36 INFO: 	"0x120099F1 D SBCM_ATOD_vltg" in "prepared0/ep2_combined.sym" file.
31-Mar-2024 13:24:36 INFO: 	"0x120099F0 D SBCM_ATOD_vltg" in "prepared/library.sym" file.
31-Mar-2024 13:24:36 INFO: Done.
```

5. All done!
