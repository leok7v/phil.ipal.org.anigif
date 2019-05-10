# phil.ipal.org.anigif
https://web.archive.org/web/20150222123613/http://phil.ipal.org/tc.html

Copyright © 1999 by Philip Howard. 
Licensing: GNU Public License

True-Color GIF Example

The three images to the left demonstrate the difference between a spectrum of 32697 colors and the same spectrum reduced by quantization to only 256 colors. All three images are in GIF format. All three conform to the GIF89A format specification as published by Compuserve. The big difference is that the ANGIF library was used to generate the true color GIF directly from the data buffer in the program that created the spectrum of colors in the 217 x 217 pixel image.

The mistaken belief that GIF has a limit of 256 colors probably comes from the way GIF was first used when it came out. In the late 1980's, PC video cards generally supported no more than 256 colors. Image exchanges were becoming popular among BBS systems and the Internet and viewer programs were quickly produced. No one tried or needed to generate images with more than 256 colors since they could not be viewed on anything less than high priced graphics workstations. Programs that converted images to GIF worked up a number of methods to reduce the number of colors to 256 or fewer. Some actually did a very good job. GIF files were constructed with just a single image block, even though the GIF standard placed no limit on the number of blocks. Since there was no use for more than 256 colors, there was no use for more than one image block. This practice became effectively ingrained into the computer culture and eventually everyone "knew" that GIF supported no more than 256 colors. The fact is, the programs that generated GIF files supported no more than one image block, and thus didn't have a means to deal with more than 256 colors. The top image shows that a GIF file really can have more than 256 colors.

The more colors a picture has, the larger the GIF file will be. Multiple image blocks do have the overhead of another block header and another local color table, in addition to the poor compression. And in the case of ANGIF, there is no compression at all (no LZW implementation due to patent restrictions). So a GIF file with more than 256 colors gets very large very quickly. Unless there is a specific need for a lossless true color picture to be viewed on a browser not capable of PNG, true-color GIF should not be used.

While using more than 256 colors in GIF is in most cases a bad practice, and should be limited to certain technical cases where extreme size can be tolerated, a statement that the GIF image file format is limited to 256 colors is simply false.

Description

The ANGIF library generates GIF formatted data streams without using LZW compression. Because the owner of the LZW does not license freeware software to make use of the LZW compression algorithm, this library provides a tool to generate GIF formatted image files without requiring the license. Such GIF files will not benefit from compression, so the resultant data will be larger in size than if the same image were formatted using LZW compression, as specified in the GIF 89a standard. 

This library API is not compatible with other GIF libraries. In particular, no tools are provided for creating or generating images. That is left up to other tools.
Features

No LZW compression patent infringing code
24-bit true-color support using multiple blocks
Web animation support

