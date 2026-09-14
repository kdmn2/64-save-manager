# Save Manager (libdragon MVP)

This project is a real-hardware-first Nintendo 64 save management app.

Current MVP supports:

- EEPROM backup to SD and restore from SD
- Controller Pak (Port 1) backup to SD and restore from SD
- Verify-after-restore by reading back and comparing bytes

## Build

Prerequisites:

- libdragon toolchain installed
- `N64_INST` set to your libdragon install path

Build ROM:

```sh
make
```

This builds a single combined ROM:

- `savemgr.z64` (auto-detects EEPROM 4K or 16K at runtime)

## Controls

- D-Pad Up/Down: move cursor
- A: run selected operation

## Save files on SD

Backups are currently written to fixed paths:

- `sd:/savemgr_eeprom.sav`
- `sd:/savemgr_cpak_p1.sav`

Each file contains a small custom header with source metadata and a checksum.

## Module layout

- `src/main.c`: app bootstrap
- `src/ui_menu.c`: console UI and actions
- `src/device_status.c`: scans SD/EEPROM/Controller Pak status
- `src/eeprom_io.c`: raw EEPROM dump/readback writes
- `src/cpak_io.c`: full Controller Pak sector dump/readback writes
- `src/sd_store.c`: save file format, checksum, SD read/write
- `src/save_manager.c`: backup/restore orchestration and verification

## Next milestones

1. Add pre-restore auto-backup and explicit confirmation prompts.
2. Support all 4 Controller ports and target selection in UI.
3. Add backup catalog (timestamped names and list picker) instead of fixed paths.
4. Improve error codes and diagnostics for SD/cart differences.
5. Add metadata sidecar export for easier host tooling.
