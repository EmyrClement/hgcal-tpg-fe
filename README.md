# hgcal-tpg-fe

## Motivation
Emulation of the Trigger Primitive Generation (TPG) by HGCAL frontend ASICs.

## Download and Setup
Run the setup code to download additional dependencies and setup. This setup is prepared for lxplus machines.

```
./setup.sh
```
This setup script downloads files from https://gitlab.cern.ch/pdauncey/hgcal10glinkreceiver.git that are required for FE emulation using the test-beam data.
In case of non-lxplus machines, you may need ROOT package in your path for compilation.

## Compile and run

### Test-beam data of August 2024
To process the test-beam binary files
```
make
./emul_Jul24.exe 1722698259 1722698259 1 0 1 1
```
The arguments from left to right are relay number, run number, link number, (1/0:trimming ON/OFF), density, dropLSB respectively.

### Estimation of maximum energy output of ECONT
Modify the module setup inside the code,
```cpp
  //===============================================================================================================================
  //input paramaters for testing
  //===============================================================================================================================
  uint32_t zside = 0, sector = 0, link = 0, econt = 0;
  
  //A combinatin of following three will uniquiely identfy the module type in HGCAL
  //(outlook : some modification expected for the ECONTs connected to different types of modules, which are not considered in current tests)
  //det: (0,1) = (Si,Sci)
  //selTC4: (0,1) = (HD,LD);
  //module:
  //Si:==
  //LD: Full (F/0), Top (T/1), Bottom (B/2), Left (L/3), Right (R/4), Five (5/5)
  //HD: Full (F/0), Top (T/1) aka Half Minus, Bottom (B/2) aka Chop Two, Left (L/3), Right(R/4)
  //Sci:==
  //LD: A5A6(0), B11B12(1), C5(2), D8(3), E8(4), G3(5), G5(6), G7(7), G8(8)
  //HD: K6(0), K8(1), K11(2), K12(3), J12(4)
  uint32_t det = 0, selTC4 = 1, module = 0;
  
  uint32_t multfactor = 15;//TOT mult factor other values could be 31 or 8
  uint32_t inputLSB = 1; //lsb at the input TC from ROC
  uint32_t dropLSB = 1;  //lsb at the output during the packing
  uint32_t select = 1 ;  //0 = Threshold Sum (TS), 1 = Super Trigger Cell (STC), 2 = Best Choice (BC), 3 = Repeater, 4=Autoencoder (AE).
  uint32_t stc_type = 0; // 0 = STC4B(5E+4M), 1 = STC16(5E+4M), 3 = STC4A(4E+3M)
  uint32_t nelinks = 4; //1 = BC1, 2 = BC4, 3 = BC6, 4 = BC9, 5 = BC14..... (see details https://edms.cern.ch/ui/#!master/navigator/document?P:100053490:100430098:subDocs)
  uint32_t calibration = 0xFFF; //0x400 = 0.5, 0x800 = 1.0, 0xFFF = 1.99951 (max)
  
  uint32_t maxADC = 0x3FF ; //10 bit input in TPG path
  uint32_t maxTOT = 0xFFF ; //12 bit input in TPG path
  //===============================================================================================================================
```

then edit Makefile to compile for findEMax.exe, and run as
```
./findEMax.exe
```

### Read a single ECONT event and print

After compilation run as,
```
./read_econt_Jul24.exe $relay $run $linknumber
```

The code has been tested with the files of September 2023 files. The output for a Best Choice run is shown below,

```
[idas@lxplus915 hgcal-tpg-fe]$ ./read_econt_Jul24.exe 1695829376 1695829376 1
Configuration::readEconTConfigYaml:: ECONT init config file : cfgmap/init_econt.yaml
FileReader::open()  opening file dat/Relay1695829376/Run1695829376_Link0_File0000000000.bin
RecordContinuing::print()
 RecordHeader::print()  Data = 0x33120002651469ca
  Pattern = 0x33 = valid
  State   = Continuing     = valid
  Sequencer counter = 1695836618
  Payload length =      2 eight-byte words 
  Record length   =     24 bytes 
   Payload word  = 0x0000000065144d80
   Payload word  = 0x6a55ae48ffffffff
  Run number       = 1695829376
  File number      =          0
  Number of events = 4294967295
  Number of bytes  = 1784000072
FileReader::read() closing file Run1695829376_Link0_File0000000000.bin
FileReader::read() opening file Run1695829376_Link0_File0000000001.bin
RecordHeader::print()  Data = 0x33120002651469ca
 Pattern = 0x33 = valid
 State   = Continuing     = valid
 Sequencer counter = 1695836618
 Payload length =      2 eight-byte words 
 Record length   =     24 bytes 
Event: 10, Module: 256, size: 10
TcRawData(0xfee9b0)::print(): Data = 0x167f, type = BestC,   module sum, energy =  89
TcRawData(0xfee9b2)::print(): Data = 0x0d4a, type = BestC, address = 10, energy =  53
TcRawData(0xfee9b4)::print(): Data = 0x0da0, type = BestC, address = 32, energy =  54
TcRawData(0xfee9b6)::print(): Data = 0x0c61, type = BestC, address = 33, energy =  49
TcRawData(0xfee9b8)::print(): Data = 0x0ee2, type = BestC, address = 34, energy =  59
TcRawData(0xfee9ba)::print(): Data = 0x1423, type = BestC, address = 35, energy =  80
TcRawData(0xfee9bc)::print(): Data = 0x0ee4, type = BestC, address = 36, energy =  59
TcRawData(0xfee9be)::print(): Data = 0x0e26, type = BestC, address = 38, energy =  56
TcRawData(0xfee9c0)::print(): Data = 0x0d28, type = BestC, address = 40, energy =  52
TcRawData(0xfee9c2)::print(): Data = 0x0c69, type = BestC, address = 41, energy =  49
FileReader::close() closing file dat/Relay1695829376/Run1695829376_Link0_File0000000001.bin
[idas@lxplus915 hgcal-tpg-fe]$
```

Similarly the output for a STC4A(4E+3M) run looks as
```
[idas@lxplus915 hgcal-tpg-fe]$ ./read_econt_Jul24.exe 1695733045 1695733046 1
Configuration::readEconTConfigYaml:: ECONT init config file : cfgmap/init_econt.yaml
FileReader::open()  opening file dat/Relay1695733045/Run1695733046_Link0_File0000000000.bin
RecordContinuing::print()
 RecordHeader::print()  Data = 0x331200026512f197
  Pattern = 0x33 = valid
  State   = Continuing     = valid
  Sequencer counter = 1695740311
  Payload length =      2 eight-byte words 
  Record length   =     24 bytes 
   Payload word  = 0x000000006512d536
   Payload word  = 0x6a55ae48ffffffff
  Run number       = 1695733046
  File number      =          0
  Number of events = 4294967295
  Number of bytes  = 1784000072
FileReader::read() closing file Run1695733046_Link0_File0000000000.bin
FileReader::read() opening file Run1695733046_Link0_File0000000001.bin
RecordHeader::print()  Data = 0x331200026512f197
 Pattern = 0x33 = valid
 State   = Continuing     = valid
 Sequencer counter = 1695740311
 Payload length =      2 eight-byte words 
 Record length   =     24 bytes 
Event: 10, Module: 256, size: 12
TcRawData(0xdbb9b0)::print(): Data = 0x4643, type = STC4A, address =  3, energy =  25
TcRawData(0xdbb9b2)::print(): Data = 0x4083, type = STC4A, address =  3, energy =   2
TcRawData(0xdbb9b4)::print(): Data = 0x4d02, type = STC4A, address =  2, energy =  52
TcRawData(0xdbb9b6)::print(): Data = 0x4940, type = STC4A, address =  0, energy =  37
TcRawData(0xdbb9b8)::print(): Data = 0x4903, type = STC4A, address =  3, energy =  36
TcRawData(0xdbb9ba)::print(): Data = 0x4303, type = STC4A, address =  3, energy =  12
TcRawData(0xdbb9bc)::print(): Data = 0x4f02, type = STC4A, address =  2, energy =  60
TcRawData(0xdbb9be)::print(): Data = 0x47c0, type = STC4A, address =  0, energy =  31
TcRawData(0xdbb9c0)::print(): Data = 0x4803, type = STC4A, address =  3, energy =  32
TcRawData(0xdbb9c2)::print(): Data = 0x4083, type = STC4A, address =  3, energy =   2
TcRawData(0xdbb9c4)::print(): Data = 0x5202, type = STC4A, address =  2, energy =  72
TcRawData(0xdbb9c6)::print(): Data = 0x4801, type = STC4A, address =  1, energy =  32
FileReader::close() closing file dat/Relay1695733045/Run1695733046_Link0_File0000000001.bin
[idas@lxplus915 hgcal-tpg-fe]$

```

### Testing running the TC processor on the unpacker output

After compilation, this can be run as

`./TestUnpackerTCProcInterface.exe`

The code will,
1. Generate some raw TC data (as in `GenerateEmpRxFile.cpp`). For simplicity only one BX is considered.
2. Converts the raw TC data to `UnpackerOutputStreamPair`s
3. Converts `UnpackerOutputStreamPair` objects to `HGCalTriggerCellSA` objects - these are identical to what is used in CMSSW. Some of the variables of the TC objects are not filled; the phi variable used in simulation is filled with the TC address. A module ID is assigned to each module; the TC object carries this information as well.
4. A mapping is created that defines TC/column budgets for the different module IDs (mapping based on budgets for different mod types). This, together with the `HGCalTriggerCellSA`s, is used as input to the TC processor emulator.
5. The TC processor emulator runs, orders the TCs in each module by address, and assigns a column/channel/frame combination to each TC (channel/frame always 0 for the moment). The output of the TC processor emulator is a new, ordered collection of `HGCalTriggerCellSA`s, with additional information stored in the column/channel/frame variables. The information stored in this TC collection is also printed to screen.
