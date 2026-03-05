# Contributing

## Workflow

1. Fork the repository.
2. Create a feature branch from `main`.
3. Keep changes focused (`firmware`, `hardware`, or `docs`).
4. Run local checks listed below.
5. Open a pull request with test evidence.

## Local Checks

- Firmware: compile with STM8duino target in Arduino IDE.
- Hardware docs: verify BOM/schematic/PCB docs are updated together.
- Markdown: check links and formatting.

## Pull Request Checklist

- [ ] Change is clearly described.
- [ ] Safety impact was considered.
- [ ] Calibration constants were documented if changed.
- [ ] README/docs were updated.

## Coding Style

- Keep firmware deterministic and small.
- Avoid dynamic memory allocation.
- Use integer math where possible.
- Keep all battery safety thresholds explicit and documented.
