# DX7

![image](https://user-images.githubusercontent.com/1408749/45313836-f67e5480-b4fd-11e8-832e-7550b04f7419.png)

Note: One of the algorithms is incorrect due to a missing operator. Need to update the image. Will have to get on that soon.

These are the original 32 algorithms as used in **Yamaha DX7**.

The later Yamaha FS1R and Yamaha SY77 may have compatibility with these algorithms, but that's beyond the current scope. The FS1R contains 88 algorithms, while the SY77 contains 45 algorithms. 

# DX9
![image](https://user-images.githubusercontent.com/1408749/45317517-3f3b0b00-b508-11e8-81b3-c44d60247177.png)

The DX9 uses the original DX7 chips, but its firmware ignores operators 1 and 2, thus effectively turning a 6-op synth into a 4-op synth. Algorithms 1 through 8 of the DX9 are hard-coded to specific DX7 algorithms: 1, 14, 8, 7, 5, 22, 31, 32.

These algorithms are chosen because operators 1-2 form an independent 2-op layer, which can be eliminated.

These 8 algorithms form the basis for all later 4-op devices. This includes DX21, DX27, DX100, DX11, V50, YS100, YS200. It was also used in many soundchips for computers, arcades and home consoles: YM2151/OPM, YM2612/OPN2, YM2608/OPNA, YM2203/OPN.

Despite having the same algorithms, it should be noted that there are differences between these devices.

# OPL
![image](https://user-images.githubusercontent.com/1408749/45318375-bb365280-b50a-11e8-8305-834182607a84.png)


The OPL series is quite different, because it originally had only 2 operators per channel. This means that only two algorithms are possible. The OPL2 tried to improve sound quality by adding additional waveforms other than just a sine wave. This was pretty new, the DX7 and many of the 4-op synths only had sine waves (aside from DX11 and some others for example). The OPL3 expanded on this to allow for 4-op modes. This added a custom set of 4 additional algorithms. Two from the DX9 series, and two completely unique to OPL3. 

Later in 2002, the MA-3/MA-5 synth chip used in cell phones expanded on the OPL3 greatly, adding much more waveforms and two extra algorithms. It also modifies algorithm 6, adding a second feedback loop. It's quite an oddity in that these chips are the only Yamaha FM chips that use more than one feedback loop in the algorithms.

# Reface DX
![image](https://user-images.githubusercontent.com/1408749/45399462-11dc8300-b616-11e8-9b62-8ca47d99f243.png)

The Reface DX is an interesting device. I haven't looked heavily into it, but it uses 12 algorithms instead of 8!
It uses 7 of the 8 DX9 algorithms. ALG4 from DX9 is removed because it is a duplicate due to Reface's extended feedback support.
Two of the algorithms are custom. Three are additional 4-op algorithms derived from DX7:  ALG 25, 13 and 30.

All operators have modifiable feedback. This allows for all 4 operators to have a different waveform, modulated from SAW<-SINE->SQUARE. This extends the capabilities of the DX in an interesting way.


# ADSR envelope (OPM/OPL/MA)

The original DX7 line had an envelope generator with 4 levels and 4 rates, not quite a traditional ADSR, but more flexible:

![image](https://user-images.githubusercontent.com/1408749/55983144-758c6480-5c68-11e9-80a2-b47e0f1c47f4.png)

However, the 4-op synths (YM2151, DX21/DX100/DX27/DX11 + many others) use a traditional ADSR with a bit of a twist:

![adsr_opm](https://user-images.githubusercontent.com/1408749/55982715-7ffa2e80-5c67-11e9-88d6-a1e0faec9898.png)

The "Sustain Rate" or "Sustain Decay Rate" is a secondary decay rate targetting the sustain period, while the key is pressed. If enabled, it will slowly decay back down to 0. This allows for greater modulation control without requring any automation of parameters (like TL). However this part of the envelope does not exist in the OPL series of chips, or MA-1 and MA-2 of the MA series (which are largely OPL based).

There are also variations in paramater resolution, which means converting between them can only be approximations.

Here are some mnemonic strings for the different envelope profiles. I've omitted the "Sustain Level" because it uses 4-bit resolution universally. Definitions: _T=Total Level, A=Attack Rate, D=Decay Rate, R=Release Rate, S=Sustain Rate_

### A = OPL/OPL2/OPL3 + MA-1/MA-2

`T6-A4-D4-R4`

### B = MA-3/MA-5

`T6-A4-D4-R4-S4`

_(Same as A but with Sustain Rate)_

### C = MA-7

`T6-A5-D5-R5-S5`

_(The only one with a 5-bit Release Rate)_

### D = OPM + OPN/OPNA/OPN2 + DX 4-op series (DX21/27/100 + DX11)

`T7-A5-D5-R4-S5`

_(The only one with a 7-bit Total Level. This is the envelope used in Sega Genesis and arcades via YM2151..)_


Notes:

- The `TL` parameter is supposedly compatible, just that OPM has an extra bit for -48 dB, which is very quiet and probably doesn't come up as a problem in practice. But I haven't looked into it.


### Frequency Multiplier differences

Unfortunately, there are different sets of frequency multipliers. The differences are minor, essentially OPM/OPN can do 11, 13 and 14 while OPL/MA cannot. MA1 is limited to 7, and MA7 redefines the list to include inharmonic frequencies.

Model | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F
| - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | -
MA1 | .5 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | | | | | | | |
OPL | .5 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |10 |10 |12 |12 |15 |15
OPM | .5 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |10 |**11** |12 |**13** |**14** |15
OPN | .5 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |10 |**11** |12 |**13** |**14** |15
MA2—MA5 | .5 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |10 |10 |12 |12 |15 |15
MA7 | .5 | .891 | 1 | 1.414 | 1.498 | 2 | 3 | 4 | 5 | 6 | 7 |8 |9 |10 |12 |15

Note that multipliers 0.5, 1, 2, 4, and 8 represent frequency doublers, and thus are octave switches. You can go one octave down and 2 octaves up. The other multipliers are still supposed to be harmonic despite not being in tune.

### DT2 + DX21/27/100

The 4-op series of DX synths have a supposedly carefully hand-picked list of ratios, 64 in total. In comparison, the OPM has only 15, but supposedly the chip used in the 4-op DX synths are based on the OPM...

Here is that list:

![dx27](https://user-images.githubusercontent.com/1408749/56002617-1abb3300-5c91-11e9-938e-a0835c623fe7.png)

It turns out that the `DT2` ("Detune 2") parameter of OPM is what is used to produce the ratio list in 4-op line, although their list appears to have been sorted by value.

Here is a little script that calculates the ratios based on `MUL` and `DT2` values.

```js
var DT2TAB = [0/768, 384/768, 500/768, 608/768], OUT = [];

for(let c = 0, MUL = 0; MUL <= 15; MUL++) {
	let d = MUL === 0 ? 0.5 : MUL;
	for(let DT2 = 0; DT2 <= 3; DT2++, c++) {
		let ratio =  ""+(Math.pow(2,s[DT2]) * d).toFixed(2);
		console.log(c.toString(2).padStart(6,0), ratio);
		OUT.push((ratio));
    }
}
```
Some strange rounding errors aside, it is quite close to the values in the DX user manuals.

Interesting to note that the resulting pitches aren't linear. When `MUL >= 4` and `DT2 != 0`, the resulting ratio is higher than when `DT2 = 0`. This could be why they sorted the numbers in the list. I wonder if the firmware has a hard-coded 6-bit number that iterates over the sorted list?


