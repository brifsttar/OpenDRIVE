# Unreal Engine OpenDRIVE plugin

This plugin allows you to manipulate your [OpenDRIVE](https://www.asam.net/standards/detail/opendrive/) road network in Unreal Engine. It is built around [esmini](https://github.com/esmini/esmini)'s [RoadManager](https://github.com/esmini/esmini/tree/master/EnvironmentSimulator/Modules/RoadManager), which does most of the heavy work. I encourage you to check out this project, which also includes [OpenSCENARIO](https://www.asam.net/standards/detail/openscenario/) support.

## Installation

`RoadManager` is included as a submodule to this repository, so make sure to initialize and update submodules. For example:
```
git clone --recurse-submodules git@github.com:brifsttar/OpenDRIVE.git
```

## Features

### OpenDRIVE Component

![OpenDRIVE Component](Resources/odr_comp.jpg)

The OpenDRIVE component can be attached to any actor to get or set its position on OpenDRIVE's track coordinate. It also interfaces some basic OpenDRIVE functions to Blueprint.

### OpenDRIVE Vehicle Component

![OpenDRIVE Vehicle Component](Resources/odr_veh_comp.jpg)

The OpenDRIVE Vehicle component can be attached to `WheeledVehicle` and provides some metrics commonly used in driving simulation.

### OpenDRIVE Reader

![OpenDRIVE Reader](Resources/odr_reader.gif)

This small editor Blueprint actor automatically updates its track coordinate when moved around the scene, making road and junction identification much easier.

### OpenDRIVE Spline

![OpenDRIVE Spline](Resources/odr_spline.gif)

A basic spline with an added feature to align all spline point to their lane center. Useful to create car trajectories.

### Junction Spawner

![Junction Spawner](Resources/odr_junction_spawn.gif)

This Blueprint can spawn any actor (usually signs or traffic lights) for all road incoming to a junction. You can also set offsets on the sign's track coordinate.

No signs (or any prop) are included in this plugin. But you can check out [Der Sky](https://www.unrealengine.com/marketplace/en-US/profile/Der+Sky)'s Marketplace products, which probably has what you need.

### Landscape Sculpting

![Landscape Sculpting](Resources/odr_sculpt.gif)

This Editor Utility Blueprint automatically sculpts your selected landscapes based on your OpenDRIVE roads. You can set the falloff, the height offset and the layer to paint under roads.
