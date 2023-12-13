# v5.2.4
- Updated dependencies (OpenSSL 3, ...)
- Miner won't try to mine Diardi's block

# v5.2.3
- Removed the True Core functionnality (too buggy at this time)
- Removed code for GPU's (OpenCL/Cuda)
- Removed non-randomx algorythms
- Rebased from XMRig v6.10.0 [(changelog)](doc/CHANGELOG.md)
- Added CPU stats for pool owners

# v5.2.2
- Rebased from XMRig v6.8.1
- Full RandomX integration for Panthera
- Better algo detection (it only use the "True" CPU Cores, since HT ones only add electricity costs if used) 
- New Feature : Pool auto-redirection

# v5.2.1
- Rebased from XMRig v6.7.0
- Added Compatibiliy with Apple M1 CPU's
- Several optimisations for the latest x86 CPU's (mainly Ryzen's)


# v5.2.0
- Updated Panthera algo
- Removed DefyX algo


# v5.1.0
- Added Panthera algo
- Moved DefyX config data into RandomX.
- Rebased from XMRig v5.11.1

# v5.0.2

- Rebase from XMRig v5.5.1 


# v5.0.1

- Rebase from XMRig v5.4.0


# v5.0.0

- Rebase from XMRig v5.3.0
- Some extra DefyX optimizations
- Disabled OpenCL/Cuda
- Updated Readme and changelog
