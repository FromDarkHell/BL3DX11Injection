## BL3 DX11 Injection
A quick/simple way of having plugins get loaded into (any game probably) Borderlands 3.

# Installation
1. Download `D3D11.zip` from [Releases](https://github.com/FromDarkHell/BL3DX11Injection/releases)
2. Drag into `Borderlands3\OakGame\Binaries\Win64`
![Example Installation](/docs/explorer_b23pLEq9zz.png)
3. Any DLL based plugins go into `Borderlands3\OakGame\Binaries\Win64\Plugins`

### Notes for Developers:
If you need to delay the loading of your DLL, edit `Plugins\pluginLoader.ini` to contain something like:
```
[PluginLoader]

[HelloWorldDLL.dll]
delaySeconds=1
```
where `delaySeconds` is how many seconds you want to delay loading (A better solution would generally be hooking into stuff btw)  

As Borderlands 3 is compiled for x64 all of your libraries need to be x64 (or x64 compatible)   
If you want a default way to setup your solution, take a look at `HelloWorldDLL` or `IntroLogoRemover` in the [Source](https://github.com/FromDarkHell/BL3DX11Injection/tree/master/IntroLogoRemover)  
If you wanna see a debug console without having to call `AllocConsole()` yourself and do some checking, launch the game with the argument `--debug`
