# Changelog

## [0.1.0] — 2026-06-11

## [0.0.0] — 2026-03-27

### Added
- Cross-platform environment variable operations (`sn_env_get`, `sn_env_set`, `sn_env_unset`)
- Environment variable enumeration (`sn_env_iterate`)
- Process query functions (`sn_env_pid`, `sn_env_exe_path`, `sn_env_cwd`)
- POSIX backend (`setenv` / `unsetenv` / `environ`)
- Windows backend (`SetEnvironmentVariableW` / `GetEnvironmentVariableW` / `GetCommandLineW`)
- User-provided memory for all string outputs
- Test suite for environment operations and process queries
- CI workflows (Linux, macOS, Windows, formatting)
