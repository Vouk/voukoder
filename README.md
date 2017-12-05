# voukoder
Use x264, x265 and other encoders of the libav or ffmpeg library directly in Adobe Premiere (Pro and Elements). Currently supporting:
- x264 (8 bit only)
- x265 (8, 10 and 12bit)
- ProRes (experimental)
- FFV1 (experimental)

This plugin is under heavy development and not yet ready for production use.

## Installation

Copy the file Voukoder.prm to your plugins directory.

### Premiere CS6

    C:\Program Files\Adobe\Adobe Premiere Pro CS6\Plug-ins\Common       // Available to Premiere CS6
    C:\Program Files\Adobe\Adobe Media Encoder CS6\Plug-ins\Common      // Available to Media Encoder CS6
    C:\Program Files\Adobe\Common\Plug-ins\CS6\MediaCore                // Available to both Premiere CS6 and Media Encoder CS6

### Premiere CC 2017 / 2018

    C:\Program Files\Adobe\Adobe Premiere Pro CC 2017\Plug-Ins\Common   // Available to Premiere CC 2017
    C:\Program Files\Adobe\Adobe Media Encoder CC 2017\Plug-Ins\Common  // Available to Media Encoder CC 2017
    
### Premiere Elements 15

    C:\Program Files\Adobe\Adobe Premiere Elements 15\Plug-Ins\Common   // Available to Premiere Elements 15
    
## Deinstallation

Just delete the Voukoder.prm file.

## Compilation

This project makes use of:

* x264 - http://x264.org
* x265 - http://x265.org
* libav - https://libav.org
* ffmpeg - https://ffmpeg.org
* boost - http://www.boost.org
* Adobe Premiere SDK

### Paths

All directories are relative to the checkout directory.

#### ../../external/include/

* "boost" - Directory containing boost headers
* "lib*" - LibAV or ffmpeg directories containing headers
* "premiere_cs6" - Premiere SDK directory
* "x264.h", "x264_config.h"
* "x265.h", "x265_config.h"

#### ../../external/lib/[debug|release]

* LibAV or ffmpeg static libs
* Boost static libs
* libx264.lib
* x265.lib

### Preprocessor

Tell the preprocessor to either use LibAV or ffmpeg (/DLIB_FFMPEG or /DLIB_LIBAV).

## Donations

At this point please contact me directly if you want to support my work on this project.

What i currently need:
- Better hardware. I currently compile, develop and test on a Intel NUC Core i3
- Compensation for software licenses (At this moment my CC plan is only for developing this plugin, not using it on actual video projects)
