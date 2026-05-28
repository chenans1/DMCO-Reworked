Rework of ersh's dodge framework to SKSE menu framework, fix stamina requirement and add dodge attack functionality back. 

## Quick start

```bash
git clone https://github.com/<you>/SKSE-Plugin-Template
cd SKSE-Plugin-Template
git submodule add -b ng https://github.com/alandtse/CommonLibVR.git extern/CommonLibVR-ng
git submodule update --init --recursive
cmake -B build -S .
cmake --build build --config Release
