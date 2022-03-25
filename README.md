# Framed
Work in progress. Collaborative (network) drawing application.  
Build using c++ & libcinder. Runs on OSX, Windows & Linux.  
Pen pressure support on OSX & Windows tested on Wacom & Huion hardware but should run with other pen-based devices.

# Installation
Download latest binary from the **[Releases](https://github.com/lab101/Framed/releases )** for your platform.  
Choose the framed.zip from assets.

Minimum osx 10.15 (Catalina) for Apple users.

If you work on OSX there will be a lot of security issues.
1. you need to allow the downloaded file in the filevault
2. the app can't access to assets folder because it runs in quarontine. Move the app to a different folder and than back to the original location. After this osx will mark it as a safe file.

Check the **[this video](https://vimeo.com/508517646/c90b0e026f )** on how to do the above.


# Template
If you want load files as background sequence put them in assets/template.
The software will scale them to scene size so make sure they have the same ratio. 

# Hotkeys
- press **f** to toggle fullscreen
- press **d** to toggle debug
- press **s** to save application settings.
- press **p** projector mode (view the animation fullscreen).
- press **x** to send a clear all participants. 

Arrow buttons to go through different frames.

# Webcam
There is experimental support for webcams on OSX & Windows in the *overlay* section. 
Enable the camera in settings panel.  
When working use **spacebar** to put a image on a layer and freeze it.

# Overlay
To use it drag a image in the app. 
Go to the debug menu and active checkbox overlay active.

# Spout
Windows version has spout support.
The spout.dll and spout.lib needs to be next toe .exe in the framed folder in order to make it work.


