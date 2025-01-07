# OpenDRIVE Unreal Engine plugin - Release notes

## Currently on `master`

Unreal Engine compatibility
* `5.5`
* Might be compatible with `5.4` and even `5.3` (not before), but untested

New features:
* Added OpenDRIVE Gizmo ([PR #24](https://github.com/brifsttar/OpenDRIVE/pull/24))
  * [README description](https://github.com/brifsttar/OpenDRIVE?tab=readme-ov-file#opendrive-gizmo) 
  * Includes migration of old Editor Mode to newer Scriptable Tools API

Improvements and fixes:
* Added `GetRealT` and `SetRealT`
  * Workaround to the fact that `GetT` and `SetT` actually work with offset (to lane center), and not actual T. Those methods are now deprecated in favor of `GetOffset` and `SetOffset`.
* Fixed `SetS` and `SetT` not refreshing the inertial coordinates


## 2024-01-07

New tags:
* `5.3`
* `5.4`

New features:
* N/A

Improvements and fixes:
* N/A

## Previously

Tags from `4.26` to `5.2`, with incremental features supported along the way (see the README for each tag for more details).