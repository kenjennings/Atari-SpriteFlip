# Atari-SpriteFlip
Simple utility to mirror an Atari sprite image

---

A simple utility to re-express text which represents an 8-pixel-wide sprite (Atari player/missile) image.  

The sprite data is mirrored horizontally and vertically and each orientation is output in my preferred format for use in mads.  Comments are included documenting the image.   Labels are output as default stub names.

This tool/toy helps me expedite creating sprites that rotate or need to appear in left-handed and right handed version.

The program creates the MADS assembly language bitmaps based on only a picture of the sprite as text characters.   (Saves me a lot of time.  Your mileage may vary.)

The image (text) data describes overlapping Atari Player/Missile graphics to generate a third color.   The output will be separated into bitmaps for Player 0 and Player 1.
 
 Sprite data input is expressed as
 - (0) or (.) period for empty space/transparent
 - (1) for Player 0 bit, -
 - (2) for Player 1 bit, and
 - (3) for overlap of Player 0 and Player 1.

The Rules:
- At this time Player data is only 8 bits per line.
- Any characters other than the characters described will be discarded.
- More than 8 usable bytes will be discarded.
- A short line of data will be padded with zeros to the right.
- An outright empty line (no convertible characters) will be ignored.
- Up to 256 lines of data may be provided. The program will ignore further data. But, why would you do this?

Maybe in some distant future . . . 
- Expand to allow 10 bits to include the missiles in the image.
- maybe 16 bits or 20 bits to include all four players and missiles.
- input specifying horizontal offset of each player.
- support up to the 23 colors possible when ovelapping players and playfield data with GTIA normal color interpretation and mixing enabled.
 
Given input data:   
```
. 1 . 2 3 . . 2
```
The output will resemble:
```
; 0 1 2 3 4 5 6 7   PM0               PM1
; . 1 . 2 3 . . 2   0 1 0 0 1 0 0 0   0 0 0 1 1 0 0 1

 _PM0
 
   .by %01001000

 _PM1
 
   .by %00011001
```
The block above will be output for each orientation. (Horizontally mirrored, vertically mirrored, and horizontally+vertically mirrored.

---
