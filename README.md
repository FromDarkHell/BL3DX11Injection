# BL3DX11Injection
 A quick/simple way of having plugins get loaded into (any game probably) Borderlands 3.

# Installation

1. Drag into `Borderlands3\OakGame\Binaries\Win64`
2. Any DLL based plugins go into `Borderlands3\OakGame\Binaries\Win64\Plugins`

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
