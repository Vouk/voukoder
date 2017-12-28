# voukoder
Use x264, x265 and other encoders of the libav or ffmpeg library directly in Adobe Premiere (Pro and Elements). Currently supporting:
- x264 (8 bit only)
- x265 (8, 10 and 12bit)
- ProRes (experimental)
- FFV1 (experimental)

See the Wiki page for more details.

This plugin is under heavy development and not yet ready for production use - ... although the results are pretty much useable already.

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
* Adobe Premiere SDK - http://download.macromedia.com/pub/developer/premiere/sdk/premiere_pro_cs6_r2_sdk_win.zip

### Paths

All directories are relative to the checkout directory.

#### ../../external/include/

* "lib*" - LibAV or ffmpeg directories containing headers
* "premiere_cs6" - Premiere SDK directory
* "x264.h", "x264_config.h"
* "x265.h", "x265_config.h"

#### ../../external/lib/[debug|release]

* LibAV or ffmpeg static libs
* libx264.lib
* x265.lib

Note: These libs are created by ffmpeg or libav build scripts.

### Preprocessor

Tell the preprocessor to either use LibAV or ffmpeg (/DLIB_FFMPEG or /DLIB_LIBAV).

## Donations

### Paypal
Although you can use this plugin for free i do appreciate getting donations. Working on the plugin consumes alot of time after my regular work. Press the button below to donate.

[![Donate](https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=A997BF8PGLGR8)

### Non-monetary support
I really need good **hardware with high performance** on compiling and rendering videos. This would speed up development alot. I currently compile, develop and test on a Intel NUC Core i3. I could also use some video equipment like an **Elgato Cam Link** and a **DSLM (GH4/5 or similar)**. If you want to support me with this please get in direct contact with me (daniel dot stankewitz at gmail dot com).

## Contact ##

Contact me on github, by email (daniel dot stankewitz at gmail dot com) or you can even follow me on twitter: https://twitter.com/LordVouk
