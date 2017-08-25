# voukoder
Use x264 from the FFmpeg library directly in Adobe Premiere (Elements).

This plugin is under heavy development and not yet ready for prodction use.

## Installation

Copy the file Voukoder.prm to your plugins directory.

### Premiere CS6

    C:\Program Files\Adobe\Adobe Premiere Pro CS6\Plug-ins\Common       // Available to Premiere CS6
    C:\Program Files\Adobe\Adobe Media Encoder CS6\Plug-ins\Common      // Available to Media Encoder CS6
    C:\Program Files\Adobe\Common\Plug-ins\CS6\MediaCore                // Available to both Premiere CS6 and Media Encoder CS6

### Premiere CC 2017

    C:\Program Files\Adobe\Adobe Premiere Pro CC 2017\Plug-Ins\Common   // Available to Premiere CC 2017
    C:\Program Files\Adobe\Adobe Media Encoder CC 2017\Plug-Ins\Common  // Available to Media Encoder CC 2017
    
### Premiere Elements 15

    C:\Program Files\Adobe\Adobe Premiere Elements 15\Plug-Ins\Common   // Available to Premiere Elements 15
    
## Deinstallation

Just delete the Voukoder.prm file again.

## Compilation

This project makes use of:

* x264 - http://www.videolan.org/developers/x264.html
* libav - https://libav.org/

To compile this you need to create one folder "3rdparty" and copy some packages there:

The official SDK
- 3rdparty\Premiere Pro CS6 r2 Win SDK

A static build of libav (incl. x264)
- 3rdparty\libav



