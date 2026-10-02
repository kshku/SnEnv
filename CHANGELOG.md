# Changelog

## [0.2.2] - 2026-10-02

### Changed
- -Wconversion and -Wsign-conversion are on for gcc and clang. The large
  variable test left the narrowing from 'A' + (i % 26) to char implicit, where
  every value is 'A' to 'Z'

## [0.2.1] - 2026-09-28

### Changed
- Take sncore v0.3.1 rather than v0.2.0

## [0.2.0] - 2026-06-29

### Changed
- Updated the dependency versions

## [0.1.0] - 2026-06-11

- First release. See [0.0.0] section in CHANGELOG.md for full changelog.

## [0.0.0] - 2026-03-27

### Added
- Cross-platform environment variable operations (`sn_env_get`, `sn_env_set`, `sn_env_unset`)
- Environment variable enumeration (`sn_env_iterate`)
- Process query functions (`sn_env_pid`, `sn_env_exe_path`, `sn_env_cwd`)
- POSIX backend (`setenv` / `unsetenv` / `environ`)
- Windows backend (`SetEnvironmentVariableW` / `GetEnvironmentVariableW` / `GetCommandLineW`)
- User-provided memory for all string outputs
- Test suite for environment operations and process queries
- CI workflows (Linux, macOS, Windows, formatting)
