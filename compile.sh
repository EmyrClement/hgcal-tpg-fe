#!/usr/bin/env bash
#g++ -I common/inc -I TPGStage1Emulation/ -I . read_hgcroc-econd.cpp  -l yaml-cpp `root-config --libs --cflags` -I`root-config --incdir` -o read_hgcroc-econd.exe
#g++ -I common/inc -I TPGStage1Emulation/ -I . validation_lpGBTs.cpp  -l yaml-cpp `root-config --libs --cflags` -I`root-config --incdir` -o validation_lpGBTs.exe
# g++ -I common/inc -I TPGStage1Emulation/ -I . validation_lpGBTs_debug.cpp  -l yaml-cpp `root-config --libs --cflags` -I`root-config --incdir` -o validation_lpGBTs_debug.exe
#g++ -I common/inc -I TPGStage1Emulation/ -I . read_lpGBT64bits.cpp  -l yaml-cpp `root-config --libs --cflags` -I`root-config --incdir` -o read_lpGBT64bits.exe
g++ -I common/inc -I TPGStage1Emulation/ -I . plotTPG.cpp  -l yaml-cpp `root-config --libs --cflags` -I`root-config --incdir` -o  plotTPG.exe
# g++ -I common/inc emul_test-beam_Sep23.cpp  -l yaml-cpp `root-config --libs --cflags` -I`root-config --incdir` -o emul_test-beam_Sep23.exe
# g++ -I common/inc findEMax.cpp  -l yaml-cpp `root-config --libs --cflags` -I`root-config --incdir` -o findEMax.exe
# g++ -I common/inc read_econt_Jul24.cpp  -l yaml-cpp `root-config --libs --cflags` -I`root-config --incdir` -o read_econt_Jul24.exe
