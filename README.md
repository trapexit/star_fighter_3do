# Star Fighter

This repository contains the source code and resources to the 1995 3DO
title Star Fighter.

* https://en.wikipedia.org/wiki/Star_Fighter_(video_game)
* https://en.wikipedia.org/wiki/Krisalis_Software

> _**Star Fighter**_ or _**Star Fighter 3000**_ is a 3D flight-based
> shoot-em-up developed and published by UK company Fednet Software,
> and released in 1994 for the [Acorn
> Archimedes](https://en.wikipedia.org/wiki/Acorn_Archimedes "Acorn
> Archimedes"). The gameplay is mission based and involves elements of
> strategy and planning. The player can order wingmen to fly in
> formation and attack specific
> targets.[[2]](https://en.wikipedia.org/wiki/Star_Fighter_(video_game)#cite_note-2)
>
> The 3DO version was developed by Tim Parry and Andrew Hutchings. It
> was developed after the original Acorn version was released. This
> version is slightly different from the original RISC OS game. The
> map screen is in 3D, not 2D as in the Acorn RISC OS version. Also,
> to upgrade the ship the player must collect a series of 3D shapes
> after blowing up certain objects. In the Acorn RISC OS version, the
> player collects and spends money on ship upgrades. Another
> difference is that the player can blast pathways through mountain
> ranges with the laser.

The source code was made available by Nathan Atkinson with permission
by Andrew Hutchings on 2024-04-24 after Mr. Atkinson was contacted by
3DO community member `shaun_3dohd` and asked about the possibility of
releasing the source.

## Contents

The code appears to be complete with most if not all media assets in
their final processed forms (CEL files for images, AIFC for audio, 3DO
Cinepak Stream files for video, etc.)

There are a number of miscellaneous utilities / tools present but
given the title was ported from Acorn Archimedes and RISC OS tools for
level design and original media creation are not included here (and
their whereabouts are unknown.)

The 3DO SDK ran on classic MacOS so all text files (.c, .s, .h,
makefiles, etc.) are in a classic MacOS format in the original archive
and therefore the [initial
commit.](https://github.com/trapexit/star_fighter_3do/tree/initial_commit)

The archive also included executables, object and debug files,
etc. which have been kept in the initial commit for thoroughness and
posterity.

## Future Plans

Like with other 3DO source code releases there is a intent to port the
title to build against the [3do-devkit
project](https://github.com/trapexit/3do-devkit) in order to provide a
convenient base for new development.
