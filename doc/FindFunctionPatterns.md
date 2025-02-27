Find Function Patterns
======================

Just an examples of creating and finding function binary patterns.

## Motorola RAZR V3xx

```bash
cd P2K-ELF-SDK/tool/kitchen

# Generate 32-byte patterns and find them.
./forge.py -s ../../res/K3_R261171LD_U_99.51.06R/elfloader.sym -f ../cg/K3_R261171LD_U_99.51.06R.smg -g 0xA0080000 -z 32 -o V3xx_32.pts
./ep1_portkit.py -P V3xx_R26111LD_U_96.A0.0ER -B -k -m -p V3xx_32.pts
mv build/Combined.sym V3xx_32.sym

# Generate 16-byte patterns and find them.
./forge.py -s ../../res/K3_R261171LD_U_99.51.06R/elfloader.sym -f ../cg/K3_R261171LD_U_99.51.06R.smg -g 0xA0080000 -z 16 -o V3xx_16.pts
./ep1_portkit.py -P V3xx_R26111LD_U_96.A0.0ER -B -k -m -p V3xx_16.pts
mv build/Combined.sym V3xx_16.sym

# Generate 8-byte patterns and find them.
./forge.py -s ../../res/K3_R261171LD_U_99.51.06R/elfloader.sym -f ../cg/K3_R261171LD_U_99.51.06R.smg -g 0xA0080000 -z 8 -o V3xx_8.pts
./ep1_portkit.py -P V3xx_R26111LD_U_96.A0.0ER -B -k -m -p V3xx_8.pts
mv build/Combined.sym V3xx_8.sym

# Arrange *.sym manually by hands.

# Combine all symbol files to one.
./combiner.py -o V3xx_ALL.sym -i V3xx_8.sym V3xx_16.sym V3xx_32.sym
```

## Motorola C390

```bash
cd P2K-ELF-SDK/tool/kitchen

# Generate 32-byte patterns and find them.
./forge.py -s ../../res/C650_R365_G_0B.D3.08R/elfloader.sym -f ../cg/C650_R365_G_0B.D3.08R.smg -g 0x10080000 -z 32 -o C390_32.pts
./ep1_portkit.py -P C390_R368_G_0B.A0.0FR -B -k -p C390_32.pts
mv build/Combined.sym C390_32.sym

# Generate 16-byte patterns and find them.
./forge.py -s ../../res/C650_R365_G_0B.D3.08R/elfloader.sym -f ../cg/C650_R365_G_0B.D3.08R.smg -g 0x10080000 -z 16 -o C390_16.pts
./ep1_portkit.py -P C390_R368_G_0B.A0.0FR -B -k -p C390_16.pts
mv build/Combined.sym C390_32.sym

# Generate 8-byte patterns and find them.
./forge.py -s ../../res/C650_R365_G_0B.D3.08R/elfloader.sym -f ../cg/C650_R365_G_0B.D3.08R.smg -g 0x10080000 -z 8 -o C390_8.pts
./ep1_portkit.py -P C390_R368_G_0B.A0.0FR -B -k -p C390_32.pts
mv build/Combined.sym C390_32.sym

# Arrange *.sym manually by hands.

# Combine all symbol files to one.
./combiner.py -o C390_ALL.sym -i C390_8.sym C390_16.sym C390_32.sym
```
