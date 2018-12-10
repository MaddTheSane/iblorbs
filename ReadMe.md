# iblorbs

Documentation for the Inform-Blorb frontend, Blorb Resource Compiler, and
Blorb Packager by L. Ross Raszewski, Version .4b 

New in this version: bdiff -- the blorb diff. see section 6.


The Inform-Blorb system was designed to make it easy to generate blorb files
for inform source code.  The first section of the document describes the
frontend.  Casual users will only need to read this section.
The remaining sections describe in detail the individual tools which make up
this package.

Questions, Bugs, concerns, donations, etc, may be sent to the author:
rraszews@hotmail.com

This program is copyright (c) 2000 by L. Ross Raszewski, but is freely
distributable and usable for all non-commercial applications (contact the
author if you wish to use it for a commerical product.)

iblorbb.zip contains MS-DOS executables for this sytem

iblorbs.zip contains source code for this system

## Justification
 Some people have questioned my choice of using separate tools and file
 formats for building blorbs in place of a unified format and single
 program, such as the "blurb" language (created by Graham Nelson for
 describing Blorb files). I thought it was worthwhile explaining myself
 here.
 
 - Why BLC instead of Blurb?
  blurb is a *descriptive* language. It describes the contents of a blorb
  file. BLC is a *control* language; it gives a set of orders to the
  blorb packager. The blc program, therefore, has no "intrinsic" knowledge
  of blorb itself, other than the general format of the file.
  Thus, if blorb were ever extended to include new resources or filetypes,
  blc could remain completely unchanged. Blurb, on the other hand, would
  need to be updated.  Blurb combines the resource specification
  with the blorb specification. As such, it is not as well suited to a
  two-phase blorbification process as a control language like blc.  A similar
  tact is taken by the "blorbtar" program. A blurb->blc converter will be
  included with iblorb.
 - Why RES instead of blurb?
  The actual resources of a game make up only a subset of what blurb allows.
  Implementign all of Blurb would therefore be overkill. RES is actually
  a simplified subset of Blurb. Blurb is a much more complex language, and
  would require a more complex parser. iblorb started life as a request for
  a painfully simple way of adding blorb resources to inform source, and
  that's what it is
 - Why three programs instead of one?
  I hadn't originally intended to write "front" at all. It's wasteful to
  run all three of the programs it calls (blc, bres, and inform) every time
  you want to compile your program; bres and blc only need to be called
  when the .res file changes (unless the story file is included in the blorb,
  in which case it's necessary to run inform and blc every time you compile,
  but not bres). Were bres and blc combined, it would be impossible to
  bundle game code with resources, as the .bli created by bres is needed
  to compile the inform source; the source code must be compiled **after** the
  resource file has been processed, but **before** the blorb is bundled.
 - Why bpal?
  I know of no other tools for constructing blorb palettes.
  
### Survey of other blorbifiers:
  * blorbtar, by Evin Roberts
   blorbtar is a blorbifier that uses a **tar(1)**-like interface. Not being
   saddled with configuration files is generally helpful. If your
   concern is bundling a blorb for an already-written game, this is a very nice
   tool. It does not generate inform headers, and does require that users be
   conversant with tar syntax.
 * blurb, by Iain Merrick
   blurb is a blorbifier similar in many respects to iblorb. It uses a blurb
   file for input, and does not call inform in the middle of blorb.  In
   most respects, blurb is indistinguishable from iblorb in terms of
   functionality.
 * perlblorb, by Graham Nelson
   perlblorb is much like blurb. Its major limitation is that it was written
   in perl for an Acorn machine, and therefore requires some effort to
   get running on any other machine.

## The resource control file (.res) format
 The resource control file tells the system what resources you want to
 include in the zcode file.  Each resource line has this format:

    TYPE IDENT FILE

 For example, to add "C:\MyPic.png" to the resources for this story, with the
 inform name of "White_House", the line would be:

    PICTURE White_House C:\MyPic.png

 You can also add other blorb resoruces to this file:
 
* CODE File
  Tells the resource manager that you want to include the specified
  Z-Code or Glulx file in the blorb-file for this program (If the
  code is included in the blorb, you need only distribute one file
  for your users to get both the game itself and its picture/sound data,
  however, users without a blorb-capable interpreter will not be able to
  play the game)
* COPYRIGHT Text
  The copyright information for the resource file
* AUTHOR Text
  The name of the resource file's author
* NOTE Text
  Any other information you want to include about the resources.
* RELEASE Number
  The release number of this blorb file
* PALETTE file
  Include the pallette from the specified file, which is in blorb palette
  format.
* RESOLUTION file
  Include the resolution table from the specified file, which is in blorb
  resolution format.
* HIGHCOLOR
  Include directions to use a 16-bit palette
* TRUECOLOR
  include directions to use a 32-bit palette
  You can also put comments in the file by startign a line with
  an exclamation point (`!`)

This is a sample of a resource file:

````
CODE foo.z5
PICTURE White_House C:\MyPic.png
SOUND Theme_Song C:\Music\theme.mod
SOUND Bang C:\Music\Bang.aif
PICTURE Plover_Egg C:\Pictures\Nature\plover.jpg
AUTHOR L. Ross Raszewski
COPYRIGHT (c) 2000
NOTE The Picture of the white house is a screencap from ZGI.
TRUECOLOR
RELEASE 1
To use the resources in your game, you include the .bli file created by
the system. In the above example, for example, you could do this:

Include "Foo.bli"; ! Created by frontend
Include "Sound";   ! Shameless self promotion
...

PlaySound(Theme_Song);
````


## Using the Frontend
1. Download and install an inform compiler
1. Place the programs "front", "blc", and "bres" in your executable
 search path (That is, the current directory, or one of the directories
 listed by typing "PATH" from a DOS prompt, or "echo $path" from a
 *nix shell)
1. The Front-end program must be invoked with at least one argument, but you
 will almost certainly want more.
1.  `front foo`
 will run the front-end with front.res as the resource definitions file, and
 will produce foo.bli and foo.blb.  (and a complaint from inform)
1. All other parameters to front will be passed on to inform.  


To compile "foo.z5" and "foo.blb" from "foo.inf", using resource file
 "foo.res", showing inform's compilation statistics, do this:

    front foo foo.inf foo.z5 -s

Frontend assumes that bres, blc, and inform are all present, named thus, and
in the search path.  If there is a file in the current directory called
"infb.rc", it will be consulted first.

An infb.rc file containing these lines:

````
INFORM  infrmw32
BRES    c:\inform\bres
````

Will cause front to use "c:\inform\bres" for the resource compiler, and
`infrmw32` as the inform compiler (you can also use this to make your
favorite inform switches the default)



## Bres: the Blorb Resource Compiler
The format of a bres input file is discussed in section 0.
Bres takes a .res file, and produces two additional files, a BLC
control file, and an inform file (the default extension, .bli,
was chosen to decrease the likilhood of accidentally writing over
one's own source).

Typing `bres` with no arguments will explain its various invocations

## BLC: the Blorb Packager
BLC generates a blorb (.blb) file, based on the contents of a blorb
control file (.blc), which is usually produced by bres, but can also be
made by hand.

Each line in a BLC control file describes one chunk of the blorb file, and
has the following format:

`Use Number Type Text`

* Use is the type of resource usage: `Pict`, `Snd`, or `Exec`.  If the chunk is
    not a resource, this should be 0, 1, or 2
    * a Use of 0 tells blc that the chunk data is a newline-terminated string
    which follows the chunk type
    * a use of 1 tells blc that the chunk data is a 2-byte integer whose
    which appears printed out after the chunk type 
    * a Use of 2 tells blc that the chunk data is in a file whose name is a
    newline terminated string which follows the chunk type
* Number is the resource number for the chunk, or any number if it is
    not a resource.
* Type is the chunk type: `JPEG`, `PNG`, `FORM` (Aiff), `MOD`, `ZCOD`, `GLUL`, `ANNO`, `(c)`,
    etc.
* Text: For a resource, this is the name of the file to be included as this
    chunk's data.  For other chunks, this is the literal text of the chunk,
    which ends at the first newline.

A sample BLC file (based on the example .res file above):

````
Pict 1 PNG C:\MyPic.png
Snd 3 MOD C:\Music\theme.mod
Snd 4 FORM C:\Music\Bang.aif
Pict 2 JPEG C:\Pictures\Nature\plover.jpg
0 0 AUTH L. Ross Raszewski
0 0 (c) (c) 2000
0 0 ANNO The Picture of the white house is a screencap from ZGI.
1 0 RelN 123
2 0 Plte C:\pictures\nat_pal.plt
````

## BPal: The Blorb Palette Editor
BPal is a versatile tool for the creation and manipulation of blorb palette
chunks. The format produced by bpal is identical to the blorb Plte chunk data
segment.  Palettes generated by bpal can be included in a bres resource or
BLC control file as explained above.

BPal's interface is a simple command-line interpreter. The following
functions can be performed:

| Option | Description |
|-------:|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `l` | load a palette file into bpal for manipulation |
| `i` | extract the palette from a PNG image if possible |
| `m` | merge a palette file into the current palette. Entries are,not duplicated, and the user is warned if some entries were not,copied due to space limitations (a palette can have no more than,256 colors according to the blorb standard) |
| `v` | Toggle output of bpal; in verbose mode, bpal prints confirmation,messages for most operations. In quiet mode, bpal prints messages,only on severe errors. |
| `s` | Save the current palette to a file |
| `a` | Add a specific RGB color entry to the palette |
| `e` | Change the value of an entry in the palette |
| `o` | Organize palette entries by color |
| `d` | Remove an entry from the palette |
| `c` | clear the palette |
| `h` | command summary |
| `q` | quit |

Probably the most useful thing to do with bpal is to extract the palette
from a PNG image; after constructing an image with the optimal palette for
the game (via some graphic manipulation tool), the PNG palette of this image
can be extracted for use in a blorb file.
(One ancient technique for ensuring that images looked correct on an 8-bit
display was to paste all the images from a game onto a single image, and let
the imaging tool construct the optimal palette. This image's palette could
then be used by the production as the palette for the entire game.
Such a method can be used with the images for a blorb file, and
the resulting PNG palette should represent the colors which the
interpreter should render)

To build foo.plt from the palette of image foo.png:

````
>bpal
bpal: Blorb Palette Editor (Feb  4 2000)
Version .1b by L. Ross Raszewski
Starting command interpreter...
type 'h' for command summary

i foo.png                                       ( import foo.png )
o                                               ( optional; sort the palette )
s foo.plt                                       ( save the palette )
q
````


## Bmerge - the blorb merger
 `bmerge` is designed to take several separate blorb files, and combine them
 into a single blorb.  This is primarily intended to ease distribution;
 you can create several small blorb files by deleting lines from the BLC
 file for a program, then recombine them later. For example:

````
Exec 0 GLUL C:\test.ulx
Pict 1 PNG C:\MyPic.png
Snd 3 MOD C:\Music\theme.mod
Snd 4 FORM C:\Music\Bang.aif
Pict 2 JPEG C:\Pictures\Nature\plover.jpg
0 0 AUTH L. Ross Raszewski
0 0 (c) (c) 2000
0 0 ANNO The Picture of the white house is a screencap from ZGI.
1 0 RelN 123
2 0 Plte C:\pictures\nat_pal.plt
````

Suppose this is the BLC file generated for a game. Because blorb files can be
very large, you may prefer to split this into several parts:

````
Exec 0 GLUL C:\test.ulx
Pict 1 PNG C:\MyPic.png
Pict 2 JPEG C:\Pictures\Nature\plover.jpg
0 0 AUTH L. Ross Raszewski
0 0 (c) (c) 2000
0 0 ANNO The Picture of the white house is a screencap from ZGI.
1 0 RelN 123
2 0 Plte C:\pictures\nat_pal.plt
````

This blc file will generate a blorb containing only the images, and this:

````
Exec 0 GLUL C:\test.ulx
Snd 3 MOD C:\Music\theme.mod
Snd 4 FORM C:\Music\Bang.aif
0 0 AUTH L. Ross Raszewski
0 0 (c) (c) 2000
1 0 RelN 123
````

Contains only the sounds. Both contain the executable code and
supplementary information, and a player could download the first to play
the game with graphics, but not sound, and the second to play the game
with sound but not graphics.

If the player later wants to play the game with all of its multimedia, he
need not download a (very lage) combined file. Instead, he may download the
two independent parts (or however many parts you've split the blorb into),
and use bmerge:

    bmerge file1.blb file2.blb out.blb

will combine the blorb files 'file1' and 'file2' into 'out.blb'

If the blorb files given as input contain overlapping chunks (In this example,
both blorb files contain the Glulx resource, and identical `AUTH`, `(c)` and `RelN`
chunks), the user will be prompted to select one of the two versions.

The optional command line parameter `-1` or `-2` will suppress this prompt;
`bmerge -1 file1.blb file2.blb out.blb` will merge files 1 and 2, using
resources from file 1 if there is an overlap, and `-2` will use resources
from file 2.

You can also use `bmerge` to distribute updates; you could release a new blorb
file which contains only those resources which have been changed.
`bmerge -2 old new out`
would create a new file which replaces the resources in 'old' with those
in 'new', leaving the unchanged resources alone.

## BDiff - Blorb Diff 

Blorb Diff is a program for generating patches to existing blorb files
to include updated information.

bdiff takes two blorb files, and composes a "delta blorb" -- this
is a normal blorb file encapsulating the differences between the two
input blorbs.

Suppose you've released a game in blorb format. Later, you wish to release
a bugfix. Let's say that this bugfix changes one image and a ZCode resource.
If the game is very large, some users may prefer not to download the entire
blorb again.

    bdiff mygame_v1.blb mygame_v2.blb patch.blb

The file `patch.blb` will contain only the game code and the changed picture
(A more useful case would be when you have changed several resources without
noting carefully which have changed.)

Users who already had `mygame_v1.blb` could reconstruct `mygame_v2.blb` using
blorb merge:

    bmerge -2 mygame_v1.blb patch.blb mygame_patched.blb

(Note: the output of bmerge may not be identical to the original
mygame_v2.blb; blorb diff and blorb merge may re-organize the blorb file
to speed processing. However, the files will contain all the same information
and will operate identically.)

(Another note: If mygame_v1 contains a resource which does not appear
in mygame_v2, that resource will also appear in mygame_patched; blorb
merge will not discard chunks. Presumably, however, any chunk which does
not appear in mygame_v2 is unused, so it can be safely ignored)


Version History:
.1b: First release

.2b: Fixed spurious constant definition for bres CODE directive, added
     makefile, various code cleaning and commenting

.3b: Various code cleaning, added support for RelN, Plte and Reso chunks

.3b+: added bpal version 1.0

.3b++: added bmerge 1.0

.4b: added bdiff
