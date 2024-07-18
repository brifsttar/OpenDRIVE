# Unreal Engine OpenDRIVE plugin

This plugin allows you to manipulate your [OpenDRIVE](https://www.asam.net/standards/detail/opendrive/) road network in Unreal Engine. It is built around [esmini](https://github.com/esmini/esmini)'s [RoadManager](https://github.com/esmini/esmini/tree/master/EnvironmentSimulator/Modules/RoadManager), which does most of the heavy work. I encourage you to check out this project, which also includes [OpenSCENARIO](https://www.asam.net/standards/detail/openscenario/) support.

## Installation

`RoadManager` is included as a submodule to this repository, so make sure to initialize and update submodules. For example:
```
git clone --recurse-submodules https://github.com/brifsttar/OpenDRIVE.git
```

If you're using a standard engine release (i.e., you're not building it yourself), do not clone the project in the engine's `Plugins` directory, but instead to a C++ projet's `Plugins` directory. After that, you'll need to rebuild your project in order to build this plugin.

Currently, this plugin  supports **Windows only**. For Linux support, it is being discussed in [#8](https://github.com/brifsttar/OpenDRIVE/issues/8).

The plugin's `master` branch is tested against the latest Unreal version. If you work with a previous release of the engine, check the tags for a compatible plugin version (earliest is 4.26).

*For [CARLA](https://github.com/carla-simulator/carla) users*: due to conflicting classes name, the plugin doesn't work as-is alongside CARLA. The simplest workaround is to delete this plugin's `ATrafficLightController` class to remove the conflict.

## Disclaimer about coordinate systems

OpenDRIVE uses a right-handed coordinate system, whereas Unreal Engine uses a left-handed coordinate system. To convert between those, the plugin assumes X and Z are common, and Y is flipped.

This assumption can differ from the one made by various other tools provider. For instance, Mathworks' [RoadRunner](https://www.mathworks.com/products/roadrunner.html) instead maps OpenDRIVE's (X, Y) axes to Unreal Engine's (Y, X). In this case, it means there ends up being a 90° rotation mismatch along the Z axis between a mesh and an OpenDRIVE file both exported from RoadRunner. The easiest workaround for this is to rotate the imported scene's root actor -90° around the Z axis in Unreal Engine.

## How to use

There are a few steps to follow before jumping into the features.

### Import the OpenDRIVE file

The plugin adds a new `OpenDRIVE` asset type, meaning you can (and should) import your `.xodr` file into the engine. The import process is similar to all other types of assets, so the [official documentation](https://docs.unrealengine.com/4.27/en-US/WorkingWithContent/Importing/HowTo/) should be able to guide you.

### Change your default World Settings class

Since OpenDRIVE files are tightly linked to their related scenes, the current workflow is to set the OpenDRIVE asset used for each world/level. To do that, you have to use the plugin's own `World Settings` class.

This is done by adding the following to your `DefaultEngine.ini`.

```ini
[/Script/Engine.Engine]
WorldSettingsClassName=/Script/OpenDRIVE.OpenDriveWorldSettings
```

### Add the OpenDRIVE file to your level

Once you've set the plugin `World Settings` class as default, you can open or create a level, and set its `OpenDRIVE Asset` via a property in the [World Settings](https://docs.unrealengine.com/4.27/en-US/Basics/Levels/WorldSettings/) tab.

![OpenDRIVE World Settings](Resources/odr_world_settings.jpg)

Once that is done, your OpenDRIVE file should be properly loaded, and you can start using the plugin's features, or add your own using the full power of [esmini](https://github.com/esmini/esmini)'s [RoadManager](https://github.com/esmini/esmini/tree/master/EnvironmentSimulator/Modules/RoadManager).

## Features

### OpenDRIVE Viewer

![OpenDRIVE Viewer](Resources/odr_viewer.gif)

You can visualize your OpenDRIVE network using the plugin's OpenDRIVE Editor Mode, accessible from the Editor Mode menu. In this mode, you can select individual lanes and get access their OpenDRIVE information (e.g., connection, ID).

### OpenDRIVE Position

![OpenDRIVE Position](Resources/odr_methods.png)

The [OpenDRIVE Position](Source/OpenDRIVE/Public/OpenDrivePosition.h) object can be used to manipulate OpenDRIVE coordinates, and convert back and forth with Unreal's coordinate system.

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

This Blueprint can spawn any actor (usually signs or traffic lights) for all roads incoming to a junction. You can also set offsets on the sign's track coordinate.

No signs (or any prop) are included in this plugin. But you can check out [Der Sky](https://www.unrealengine.com/marketplace/en-US/profile/Der+Sky)'s Marketplace products, which probably has what you need.

### Landscape Sculpting

![Landscape Sculpting](Resources/odr_sculpt.gif)

This Editor Utility Blueprint automatically sculpts your selected landscapes based on your OpenDRIVE roads. You can set the falloff, the height offset and the layer to paint under roads.

**This feature doesn't include drawing the actual road network**, it only sculpts the landscape to perfectly fit along the roads. In the video above, the road network was generated using [RoadRunner](https://www.mathworks.com/products/roadrunner.html)

Known limitations:

*  Sculpting can miss parts of the landscape in junctions, leaving spikes. Those can be manually removed with standard sculpting tools
* Layer painting won't remove foliage from the previous layer. It seems to be an Unreal Engine bug, and the current workaround is to manually apply a single paint touch to each landscape grid unit, which will instantly remove all foliage on painted roads for that grid unit.

### Road Decal Spawner

![Road Decal Spawner](Resources/odr_decal_spawner.gif)

This Blueprint can randomly spawn a selection of [Decal](https://docs.unrealengine.com/4.27/en-US/Resources/ContentExamples/Decals/) on the drivable lanes; ideal for placing asphalt defects, tar lines, oil splats, etc.

No material are included with the plugin, but many can be found for free on [Quixel](https://quixel.com/megascans/home?category=decal&category=street).

### Duplicate Actor Along Road

![Road Duplicate Actor](Resources/odr_repeat.gif)

This Editor Utility Blueprint allows any actor to be duplicated along any road at a fixed step. The process runs until it encounters a junction, so it can actually span multiple roads.

For this to work, actors are assumed to use the standard Unreal Engine alignment (X+ forward, Z+ up), except Decals, for which Y- forward and X- up are expected.
