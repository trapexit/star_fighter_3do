#
#
#
# Copyright (C) 1995, an unpublished work by The 3DO Company. All rights reserved.
# This material contains confidential information that is the property of The 3DO Company.
# Any unauthorized duplication, disclosure or use is prohibited.
# $Id: cdrom.tcl,v 1.6 1994/11/15 00:36:57 dplatt Exp $
#
# $Log: cdrom.tcl,v $
# Revision 1.6  1994/11/15  00:36:57  dplatt
# Add parameters for Catapult file.  Add commentary elsewhere to
# clarify avatar issues, recommend large filesystem size when
# optimizing.
#
# Revision 1.5  1994/05/13  00:05:48  dplatt
# Warn developers not to override 2048-byte default filesystem block size.
#
# Revision 1.4  1994/01/27  00:23:14  dplatt
# Change default to 8 megabytes
#
# Revision 1.3  1993/05/31  03:46:39  dplatt
# Dragon changes, speedup, etc.
#
# Revision 1.2  1993/03/04  22:42:58  dplatt
# Improve support for unused-block preinitialization
#
# Revision 1.1  1993/02/08  23:58:30  dplatt
# Initial revision
#
# Revision 1.5  1992/12/22  08:02:39  dplatt
# Magneto 3 changes and CD-ROM support
#
# Revision 1.4  1992/12/08  06:00:19  dplatt
# Magenta changes
#
# Revision 1.3  1992/10/24  00:48:10  dplatt
# Preserve extension, better demo disc image contents
#
# Revision 1.2  1992/09/11  00:57:13  dplatt
# Add log and ID headers
#
#

#
#
# This script sets the parameters for an Opera filesystem layout,
# and then invokes the script functions which drive the layout process
# itself.
#
# Copy this file and edit it according to your needs.
#

#
# fstype - set this to either "ramdisk", "romdisk" or "cd-rom".  This will
# result in the use of reasonable defaults for any parameters which are not
# explicitly specified.
#

set fstype "cd-rom"

#
# imagefile - set to the name of the Mac or Unix file into which the
# new filesystem image should be written.  Defaults to "cdrom.image",
# "ramdisk.image", or "romdisk.image" depending on the fstype.
#

set imagefile "MegaMac:StarImage"

#
# label - set to the label (name) of the filesystem.  You should normally
# let the defaults be used.  See the CD-ROM Mastering Guide for exceptions.
#

# set label "MyDisk"

#
# kilobytes, megabytes, or blocks - set to the size of the desired
# filesystem.  Set only one of these.  If you request too small a
# value, the filesystem will run out of space and the layout process
# will abort.
#
# When doing a test layout of a filesystem, you can set the size field to
# a value somewhat larger than that of the "takeme" folder - 110% of the
# takeme folder size is a good starting point.
#
# When doing a final, optimized/clustered/Catapulted layout for submission
# to 3DO, it is best to specify a large filesystem size.  The larger the
# filesystem image (and the more "unused" space in excess of the size
# of your "takeme" folder), the better a job the optimizer can do in
# making multiple avatars of your commonly-used files, and creating a big
# Catapult file to speed up the CD-booting process.  Unless you have a
# severe space shortage on your development-station hard disk, I recommend
# that you set the filesystem size to 600 megabytes or larger when doing
# your final pre-encryption layout.
#

# set kilobytes 1024
set megabytes 440
# set blocks 512

#
# blocksize - set to the desired blocksize for the filesystem.
#
# Do not uncomment the following line and/or try to override the
# default blocksize for the type of filesystem you are building, unless
# you really and absolutely know what you are doing.  Using non-default
# blocksizes for CD-ROM filesystems is NOT guaranteed to work, and is
# not something which 3DO supports.  In other word, the default is
# manditory, the optional is forbidden.
#

# set blocksize 2048

#
# takedirectory - set to the pathname of the source directory which is
# to be mirrored onto the filesystem.
#
# Unix environment defaults to "takeme"
# Mac  environment defaults to ":takeme"
#

# set takedirectory "takeme"

#
# preinitialize - set to 1 if the storage for the filesystem should
# be initialized with known valuesbefore data is laid out on it.  Set 
# to 0 if the filesystem should not be preinitialized.
#
# The default for this parameter is 1 in the Macintosh environment and
# 0 in the Unix environment.
#
# 
# Mac users - you may set this parameter to 0 to speed up filesystem
# layout during the development process.  However, DO NOT EVER lay out a
# filesystem for general distribution (i.e. to the consumer market) with
# preinitialization turned off.  If you do, then "residue" from previous
# uses of blocks on your hard drive may appear in "unused" portions of the
# Opera filesystem.  This would be bad.  You could end up inadvertently
# publishing your source code this way.  You do not want this to occur.
# 
# Sun users do not need to worry about this, because SunOS scrubs all
# hard-disk blocks the first time they are used.  I believe this is true
# of all BSD systems.  It wouldn't hurt to turn preinitialization on
# during your final layout run, just to make sure.
#
# You have been warned.
#
# Writers of ROMdisks may wish to set this parameter to 1.  The default
# preinitialization value for ROMdisks is 0xFF, which happens to be the
# value stored in a standard EPROM which has just been erased.  Many
# PROM-programmers are intelligent enough to skip over bytes which
# contain 0xFF - hence, you can probably speed up the PROM-programming
# process by making the filesystem image size identical to that of the
# EPROM and turning on preinitialization.

set preinitialize 0

#
# elephantstomp - set to the value to which you wish to have unused
# blocks bytes in the filesystem image initialized.  Defaults to 0
# for RAM disks, 0xFF for ROMdisks, and "iamaduck" for CD-ROMs.  Can be
# set to a one-byte value (0x00 - 0xFF), a 32-bit integer value, or
# a string.
#

# set elephantstomp 0x00

#
# directoryavatars - set to the number of copies of each directory 
# (other than the root) you wish to have scattered across the filesystem.
#
# If the layout process fails with an "Unable to assign avatar for file XXX",
# and file XXX is a large one (say, more than 100 megabytes), you may
# wish to set directoryavatars to 1 and try the layout again.  This will
# eliminate free-space fragmentation due to the placement of multiple
# directory avatars.
#
# If you do this, please set directoryavatars back to a higher value
# (3 is good) when doing your final optimized layout per the instructions
# in the CD-ROM mastering guide.
#

# set directoryavatars 3

#
# rootavatars - set to the number of copies of the root directory that
# you wish to have scattered across the disc.  May be reduced to 2, but
# no lower.
#

# set rootavatars 7

#
# labelavatars - set to the number of copies of the label that you
# wish to have scattered across the disc.
#

# set labelavatars 2

#
# Define the size and characteristics of the Catapult file.  These
# parameters specify the number of megabytes of space which the Catapult
# file may use, the number of table-of-contents index pages which can
# be placed in the file, and the size of the longest "run" of
# consecutive blocks from a single file which can be placed in the
# file.
#
# If any of these parameters is left undefined, the Catapult file
# will not be built, and you will sacrifice the boot-time speedups which
# Catapult optimization can provide.
#
# See the CD-ROM mastering guide for information about the Catapult
# technology and tuning process.
#

# set catapultmegabytes 50
# set catapultpages 10
# set catapultrunlimit 5120

#
# At this point, you may include statements which exclude individual
# files, or entire directories from inclusion in the Catapult file.
# It is often desireable to exclude long FMV sequences, or long
# audio background-music tracks, from the Catapult file - this retains
# space in the Catapult file for smaller and more seek-intensive files.
#
# The following statement would exclude a specific file:
#
#    excludecatapult "data/intro.stream"
#
# The following statement would exclude all files in a specific directory:
#
#    excludecatapult "data/sounds/music/"

#
# OK, invoke deep magick and do the layout.
#

if {0 == [catch {doit} whoops]} then {
  echo ""
  echo "Layout successful."
  echo ""
} else {
  echo ""
  echo $whoops
  echo ""
  echo "***** Layout failed *****"
  echo ""
}
